#include "stdafx.h"
#include "Tools.h"


void CommonTools::ParseUrl(std::string url, std::string &serverName, std::string &filepath, std::string &filename)
{
	string::size_type n;

	if (url.substr(0, 7) == "http://")
		url.erase(0, 7);

	if (url.substr(0, 8) == "https://")
		url.erase(0, 8);

	n = url.find('/');
	if (n != string::npos)
	{
		serverName = url.substr(0, n);
		filepath = url.substr(n);
		n = filepath.rfind('/');
		filename = filepath.substr(n + 1);
	}

	else
	{
		serverName = url;
		filepath = "/";
		filename = "";
	}
}

bool CommonTools::ContentValid(std::string path, unsigned long len, unsigned char* hash)
{
	if (CommonTools::FileExists(path) != true) return 0;
	if (CommonTools::GetFileSize(path) != len) return 0;
	return 1;
}

std::vector<unsigned char> CommonTools::HexToBytes(const std::string & hex)
{
	std::vector<unsigned char> bytes;

	for (unsigned int i = 0; i < hex.length(); i += 2) {
		std::string byteString = hex.substr(i, 2);
		char byte = (char)strtol(byteString.c_str(), NULL, 16);
		bytes.push_back(byte);
	}

	return bytes;
}

std::string CommonTools::string_to_hex(const std::string& input)
{
	static const char* const lut = "0123456789abcdef";
	size_t len = input.length();

	std::string output;
	output.reserve(2 * len);
	for (size_t i = 0; i < len; ++i)
	{
		const unsigned char c = input[i];
		output.push_back(lut[c >> 4]);
		output.push_back(lut[c & 15]);
	}
	return output;
}

bool CommonTools::FileExists(std::string name) {
	struct stat buffer;
	return stat(name.c_str(), &buffer) == 0;
}

int CommonTools::DirExists(const char *path)
{
	struct stat info;

	if (stat(path, &info) != 0)
		return 0;
	else if (info.st_mode & S_IFDIR)
		return 1;
	else
		return 0;
}

unsigned long CommonTools::GetFileSize(std::string filename)
{
	struct stat stat_buf;
	int rc = stat(filename.c_str(), &stat_buf);
	return rc == 0 ? (unsigned long)stat_buf.st_size : -1;
}

std::wstring CommonTools::s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

std::string CommonTools::getUserInput()
{
	std::string input = "";

	getline(std::cin, input);

	return input;
}

//TODO: probably a memory leak, don't care(yet)
char * CommonTools::StringToCharArray(std::string str)
{
	char *cstr = new char[str.length() + 1];
	strcpy(cstr, str.c_str());

	return cstr;
	delete[] cstr;
}

struct CommonStructures::DataInfo CommonTools::DownloadBytes(const char * url)
{
	DataInfo di;
	DownloadClient dc;
	
	if ((dc = DownloadClient(url)).len > 0)
	{
		di.data = dc.buf;
		di.len = dc.len;
	}

	return di;
}

void CommonTools::WriteLine(char const * const fmt, ...)
{
	char buffer[1024];

	va_list args;
	va_start(args, fmt);
	vsprintf(buffer, fmt, args);

	cout << buffer << rang::style::reset << endl;

	va_end(args);
}

void CommonTools::WriteLineBold(char const * const fmt, ...)
{
	char buffer[1024];

	va_list args;
	va_start(args, fmt);
	vsprintf(buffer, fmt, args);

	cout << rang::style::bold << buffer << rang::style::reset << endl;

	va_end(args);
}

void CommonTools::WriteLineRed(char const * const fmt, ...)
{
	char buffer[1024];

	va_list args;
	va_start(args, fmt);
	vsprintf(buffer, fmt, args);

	cout << rang::fg::red << fmt << rang::style::reset << endl;

	va_end(args);
}

char * CommonTools::ReadFile(const char * Name, u32 * Length)
{
	FILE* in = fopen(Name, "rb");
	if (in == nullptr)
	{
		//perror("");
		return nullptr;
	}

	fseek(in, 0, SEEK_END);
	*Length = ftell(in);

	fseek(in, 0, 0);

	char* Data = new char[*Length];

	size_t read = fread(Data, 1, *Length, in);

	fclose(in);

	return Data;
}

void CommonTools::SaveFile(const char* fn, const char* buf, u32 len)
{
	if (buf == nullptr)
	{
		printf("zero ptr");
		return;
	}
	if (len == 0)
	{
		printf("zero sz");
		return;
	}
	std::ofstream os(fn);
	os.seekp(os.beg);
	os.write(buf, len);
	os.close();
}
