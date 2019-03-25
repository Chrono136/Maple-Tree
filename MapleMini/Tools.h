#pragma once

namespace CommonTools
{
	void WriteLine(char const * const fmt, ...);
	void WriteLineBold(char const* const _format, ...);
	void WriteLineRed(char const * const fmt, ...);

	void ParseUrl(std::string url, std::string &serverName, std::string &filepath, std::string &filename);

	bool ContentValid(std::string filePath, unsigned long len, unsigned char* s);

	bool FileExists(std::string name);
	int DirExists(const char *path);
	unsigned long GetFileSize(std::string filename);
	std::wstring s2ws(const std::string& s);
	std::string getUserInput();
	char* StringToCharArray(std::string);

	//A shortcut to DownloadClient::DownloadData
	struct CommonStructures::DataInfo DownloadBytes(const char* url);

	char* ReadFile(const char* Name, u32* Length);
	void SaveFile(const char* Name, void* Data, u32 Length);
}