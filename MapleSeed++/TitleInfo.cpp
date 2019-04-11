#include "stdafx.h"
#include "TitleInfo.h"


static map<string, TitleInfo*> database;

TitleInfo::TitleInfo(const char *_id)
{
	try {
		string url = string("http://api.tsumes.com/title/" + string(_id));

		if (_id[7] == 'e' || _id[7] == 'E')
			url = string("http://api.tsumes.com/titlekey/" + string(_id));

		auto jd = DownloadBytes(url.c_str());
		if (jd.data == nullptr) return;

		ParseTitleInfo(jd.data, jd.len);
	}
	catch (std::exception const& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

TitleInfo::TitleInfo(char * str, size_t len)
{
	try {
		ParseTitleInfo(str, len);
	}
	catch (std::exception const& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

TitleInfo::TitleInfo()
{
}

TitleInfo::~TitleInfo()
{
}

TitleInfo * TitleInfo::GetTitleInfo(const char * id)
{
	string url = string("http://api.tsumes.com/title/" + string(id));
	auto jd = DownloadBytes(url.c_str());
	if (jd.data == nullptr) return nullptr;

	TitleInfo* ti = new TitleInfo(jd.data, jd.len);

	return ti;
}

TitleInfo * TitleInfo::ParseTitleInfo(const char * str, size_t len)
{
	try {
		string _str(str);
		UTF16toUnicode(_str);

		struct json_token t;

		for (int i = 0; json_scanf_array_elem(_str.c_str(), (int)len, "", i, &t) > 0; i++) {
			json_scanf(t.ptr, t.len, "{uid: %Q}", &uid);
			json_scanf(t.ptr, t.len, "{ID: %Q}", &id);
			json_scanf(t.ptr, t.len, "{Key: %Q}", &key);
			json_scanf(t.ptr, t.len, "{Name: %Q}", &name);
			json_scanf(t.ptr, t.len, "{Region: %Q}", &region);
			json_scanf(t.ptr, t.len, "{Versions: %Q}", &versions);
			json_scanf(t.ptr, t.len, "{HasDLC: %Q}", &hasdlc);
			json_scanf(t.ptr, t.len, "{HasPatch: %Q}", &haspatch);
			json_scanf(t.ptr, t.len, "{ContentType: %Q}", &contenttype);
			json_scanf(t.ptr, t.len, "{AvailableOnCDN: %Q}", &cdn);
			json_scanf(t.ptr, t.len, "{ProductCode: %Q}", &pcode);
			json_scanf(t.ptr, t.len, "{CompanyCode: %Q}", &ccode);
			json_scanf(t.ptr, t.len, "{ImageLocation: %Q}", &iloc);
			json_scanf(t.ptr, t.len, "{Notes: %Q}", &notes);
			database[id] = this;
		}

		if (id) UpdateContentType();

		return this;
	}
	catch (std::exception const& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return nullptr;
	}
}

char * TitleInfo::GenerateTMD(std::string _librarypath, std::string id, string version)
{
	string tmdPath = _librarypath + string("/tmd");
	string tmdURL = string("http://ccs.cdn.wup.shop.nintendo.net/ccs/download/") + id + string("/tmd");

	if (!version.empty())
		tmdURL += string(".") + version;

	DownloadClient dc_tmd;
	if (!FileExists(tmdPath))
	{
		dc_tmd.DownloadFile(tmdURL.c_str(), tmdPath.c_str());
	}

	char* TMD = ReadFile(tmdPath.c_str(), &dc_tmd.len);
	if (TMD == nullptr)
	{
		perror("Failed to open tmd\n");
		return NULL;
	}

	return TMD;
}

char * TitleInfo::GenerateTicket(std::string id)
{
	MapleTicket mt = MapleTicket::Create(id);
	return GenerateTicket(mt.info);
}

char * TitleInfo::GenerateTicket(TitleInfo * ti)
{
	if (ti)
	{
		MapleTicket mt = MapleTicket::Create(ti);
		auto dir = mt.info->GetLibraryPath();

		if (!DirExists(dir.c_str()))
			create_directory(dir);

		char* data = mt.data;
		int len = mt.len;

		SaveFile((dir + string("/cetk")).c_str(), data, len);

		return data;
	}
	
	return nullptr;
}

string TitleInfo::SetLibraryPath(string path)
{
	auto p = filesystem::absolute(path);

	return LibraryPath = p.generic_string();
}

string TitleInfo::GetLibraryPath()
{
	//auto base = Library::GetBaseDirectory();
	//auto nam = string("[") + string(region) + string("]") + string(name);
	//auto basenam = filesystem::absolute(base + string("/") + nam).generic_string();

	if (!DirExists(LibraryPath.c_str()))
		create_directory(LibraryPath);

	return LibraryPath;
}

string TitleInfo::GetMetaXmlFile()
{
	auto libpath = GetLibraryPath();
	auto metaxml = libpath + string("/meta/meta.xml");

	if (!FileExists(metaxml))
	{
		WriteLineRed("Error locating meta.xml (%s)", metaxml);
		return "";
	}

	return metaxml;
}

string TitleInfo::GetProductCode()
{
	auto metaxml = GetMetaXmlFile();

	if (FileExists(metaxml))
	{
		auto value = Library::ref->GetMetaXmlValue(metaxml, "product_code");

		auto product_code = value.substr(6);

		return product_code;
	}

	return "default";
}

string TitleInfo::GetCoverArt()
{
	auto code(GetProductCode());

	auto temp_dir(std::filesystem::temp_directory_path().generic_string() + string("mapleseed/"));
	auto cover = temp_dir + string(code + ".bmp");

	if (!DirExists(temp_dir.c_str()))
		create_directory(temp_dir);

	if (!FileExists(cover))
	{
		string url = string("http://pixxy.in/cover/?code=") + code + string("&region=") + region;

		DownloadClient().DownloadFile(url.c_str(), cover.c_str());
	}

	return cover;
}

void TitleInfo::UpdateContentType()
{
	switch (string(id).at(7))
	{
	case 'e':
	case 'E':
		_type = ContentType::Patch;
		SetLibraryPath(Library::GetBaseDirectory() + string("/[Update]") + string("[") + string(region) + string("]") + string(name));
		break;

	case 'c':
	case 'C':
		_type = ContentType::DLC;
		SetLibraryPath(Library::GetBaseDirectory() + string("/[DLC]") + string("[") + string(region) + string("]") + string(name));
		break;

	case '0':
		_type = ContentType::Game;
		SetLibraryPath(Library::GetBaseDirectory() + string("/[") + string(region) + string("]") + string(name));
		break;
	}
}

int TitleInfo::Download(string version)
{
	string baseURL = string("http://ccs.cdn.wup.shop.nintendo.net/ccs/download/");
	auto workingDir = GetLibraryPath();

	char* TMD = GenerateTMD(workingDir, id, version);
	TitleMetaData* tmd = (TitleMetaData*)TMD;

	GenerateTicket(this);

	u16 contentCount = bs16(tmd->ContentCount);
	for (int i = 0; i < contentCount; i++)
	{
		char str[1024];
		sprintf(str, "%08X", bs32(tmd->Contents[i].ID));
		auto contentID = string(str);

		auto contentPath = workingDir + string("/") + contentID;
		auto downloadURL = baseURL + id + string("/") + contentID;

		auto size = bs64(tmd->Contents[i].Size);

		if (!CommonTools::ContentValid(contentPath, (unsigned long)size, NULL))
		{
			printf("Downloading Content (%s) #%u of %u... (%lu)\n", contentID.c_str(), i + 1, contentCount, (unsigned long)size);
			auto dc = DownloadClient(downloadURL.c_str(), contentPath.c_str(), (unsigned long)size, 1, 1);
		}
		else
		{
			printf("Skipping Content (%s) #%u of %u... (%lu)\n", contentID.c_str(), i + 1, contentCount, (unsigned long)size);
		}
	}

	cout << rang::style::bold << "Download Complete!!" << rang::style::reset << endl;
	return 0;
}

void TitleInfo::Decrypt()
{
	_chdir(this->GetLibraryPath().c_str());

	if (!FileExists(".\\tmd"))
	{
		WriteLineRed("tmd file is missing. Decryption failed!");
		return;
	}

	if (!FileExists(".\\cetk"))
	{
		WriteLineRed("cetk file is missing. Decryption failed!");
		return;
	}

	startDecryption(3, "tmd", "cetk", 0);
	_chdir(Library::GetBaseDirectory().c_str());
}