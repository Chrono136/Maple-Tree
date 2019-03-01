#pragma once
class Toolbelt
{
public:
	static bool FileExists(const std::string& name);
	static int DirExists(const char *path);
	static long GetFileSize(std::string filename);
	static std::wstring s2ws(const std::string& s);
	static std::string getUserInput();
	static char* StringToCharArray(std::string);
	static char* DownloadBytes(char* url);

	static void WriteLine(const char* text);
	static void WriteLineRed(const char* text);

	static char* ReadFile(const char* Name, u32* Length);
	static void SaveFile(const char* Name, void* Data, u32 Length);

	Toolbelt();
	~Toolbelt();
};

