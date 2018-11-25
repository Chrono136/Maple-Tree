#pragma once
class Toolbelt
{
public:
	static std::string getUserInput();
	static char* StringToCharArray(std::string);
	static char* DownloadBytes(char* url);

	static void WriteLine(const char* text);
	static void WriteLineRed(const char* text);

	Toolbelt();
	~Toolbelt();
};

