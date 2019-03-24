#include "stdafx.h"
#include "Tools.h"


bool CommonTools::ContentExists(std::string f, long s) {
	if (!CommonTools::FileExists(f))
	{
		return 0;
	}
	if (CommonTools::GetFileSize(f) != s)
	{
		return 0;
	}
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

long CommonTools::GetFileSize(std::string filename)
{
	struct stat stat_buf;
	int rc = stat(filename.c_str(), &stat_buf);
	return rc == 0 ? stat_buf.st_size : -1;
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
