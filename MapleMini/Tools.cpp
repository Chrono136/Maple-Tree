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

	char hexstr[SHA_DIGEST_LENGTH * 2 + 1];
	for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
		sprintf(&hexstr[i * 2], "%02x", (unsigned int)hash[i]);

	unsigned char digest[SHA_DIGEST_LENGTH];
	SHA_CTX ctx;
	SHA1_Init(&ctx);
	std::ifstream bigFile(path);
	const int bufferSize = 1024 * 10;
	while (bigFile)
	{
		char buf[bufferSize];
		bigFile.read(buf, bufferSize);
		SHA1_Update(&ctx, buf, bufferSize);
	}
	SHA1_Final(digest, &ctx);
	char hex_str[SHA_DIGEST_LENGTH * 2 + 1];
	for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
		sprintf(&hex_str[i * 2], "%02x", (unsigned int)digest[i]);

	if (memcmp(hash, digest, SHA_DIGEST_LENGTH) != 0)
		return 0;

	return 1;
}

bool CommonTools::FileExists(std::string name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
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
	
	if ((dc = DownloadClient(url)).length > 0)
	{
		di.data = dc.dataBytes;
		di.len = dc.length;
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

void CommonTools::SaveFile(const char* Name, void* Data, u32 Length)
{
	if (Data == nullptr)
	{
		printf("zero ptr");
		return;
	}
	if (Length == 0)
	{
		printf("zero sz");
		return;
	}
	FILE* Out = fopen(Name, "wb");
	if (Out == nullptr)
	{
		perror("");
		return;
	}

	if (fwrite(Data, 1, Length, Out) != Length)
	{
		perror("");
	}

	fclose(Out);
}
