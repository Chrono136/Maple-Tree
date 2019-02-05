#include "stdafx.h"
#include "Toolbelt.h"


int Toolbelt::DirExists(const char *path)
{
	struct stat info;

	if (stat(path, &info) != 0)
		return 0;
	else if (info.st_mode & S_IFDIR)
		return 1;
	else
		return 0;
}

std::wstring Toolbelt::s2ws(const std::string& s)
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

std::string Toolbelt::getUserInput()
{
	std::string input = "";

	getline(std::cin, input);

	return input;
}

//TODO: probably a memory leak, don't care(yet)
char* Toolbelt::StringToCharArray(std::string str)
{
	char *cstr = new char[str.length() + 1];
	strcpy(cstr, str.c_str());

	return cstr;
	delete[] cstr;
}

char * Toolbelt::DownloadBytes(char *)
{
	return nullptr;
}

void Toolbelt::WriteLine(const char * text)
{
	cout << rang::style::bold << text << rang::style::reset << endl;
}

void Toolbelt::WriteLineRed(const char * text)
{
	cout << rang::fg::red << text << rang::style::reset << endl;
}

char * Toolbelt::ReadFile(const char * Name, u32 * Length)
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

void Toolbelt::SaveFile(const char* Name, void* Data, u32 Length)
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

Toolbelt::Toolbelt()
{
}


Toolbelt::~Toolbelt()
{
}
