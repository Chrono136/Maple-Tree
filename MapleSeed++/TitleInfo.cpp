#include "stdafx.h"
#include "TitleInfo.h"

static map<int,TitleInfo*> database;
static vector<std::thread> threads;
std::string TitleInfo::outputDir;

TitleInfo::TitleInfo(char * str, size_t len)
{
	try {
		ParseTitleInfo(str, len);
		SetDirectory(outputDir);
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
		struct json_token t;

		for (int i = 0; json_scanf_array_elem(str, (int)len, "", i, &t) > 0; i++) {
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
			database[strtol(id, NULL, 0)] = this;
		}

		return this;
	}
	catch (std::exception const& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return nullptr;
	}
}

char * TitleInfo::GenerateTMD(std::string working_dir, std::string __id)
{
	string _id(__id);
	std::transform(_id.begin(), _id.end(), _id.begin(), ::tolower);

	string tmdPath = working_dir + string("/tmd");
	string tmdURL = (string("http://api.tsumes.com/title/") + _id + string("/tmd"));

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

		auto dir = TitleInfo::outputDir + string("/[")
			+ string(mt.info->region) + string("] ") + string(mt.info->name);

		if (DirExists(dir.c_str()))
		{
			char* data = mt.data;
			int len = mt.len;

			SaveFile((dir + string("/cetk")).c_str(), data, len);

			return data;
		}
	}
	
	return nullptr;
}

void TitleInfo::SetDirectory(std::string output_root)
{
	if (name != NULL && region != NULL)
		workingDir = string(output_root) + string("/[") + string(region) + string("] ") + string(name);

	boost::filesystem::path dir(workingDir);
	if (boost::filesystem::create_directory(dir)) {
		WriteLine("Created directory '%s'", workingDir.c_str());
	}
}

#pragma comment(lib,"Urlmon.lib")
int TitleInfo::DownloadContent()
{
	string baseURL = string("http://ccs.cdn.wup.shop.nintendo.net/ccs/download/");

	auto _dir = std::wstring(s2ws(workingDir));
	if (CreateDirectory(_dir.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {}

#pragma region Setup TMD
	char* TMD = GenerateTMD(workingDir, id);
	TitleMetaData* tmd = (TitleMetaData*)TMD;
#pragma endregion

#pragma region Setup Ticket
	GenerateTicket(this);
#pragma endregion

	u16 contentCount = bs16(tmd->ContentCount);
	for (int i = 0; i < contentCount; i++)
	{
		char str[1024];
		
		sprintf(str, "00050000%08X", (unsigned int)bs64(tmd->TitleID));
		auto titleID = string(str);
		
		sprintf(str, "%08X", bs32(tmd->Contents[i].ID));
		auto contentID = string(str);

		string contentPath = string(outputDir) + string("/") + contentID;
		auto downloadURL = baseURL + titleID + string("/") + contentID;

		auto filePath = workingDir + string("/") + contentID;
		auto size = bs64(tmd->Contents[i].Size);

		if (!CommonTools::ContentValid(filePath, size, NULL))
		{
			printf("Downloading Content (%s) #%u of %u... (%lu)\n", contentID.c_str(), i + 1, contentCount, (unsigned long)size);
			auto dc = DownloadClient(downloadURL.c_str(), filePath.c_str(), (unsigned long)size, 1, 1);
		}
		else
		{
			printf("Skipping Content (%s) #%u of %u... (%lu)\n", contentID.c_str(), i + 1, contentCount, (unsigned long)size);
		}
	}

	cout << rang::style::bold << "Download Complete!!" << rang::style::reset << endl;
	return 0;
}