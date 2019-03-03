#pragma once

namespace CommonTools
{
	void WriteLine(char const * const fmt, ...);
	void WriteLineBold(char const* const _format, ...);
	void WriteLineRed(char const * const fmt, ...);

	bool FileExists(const std::string& name);
	int DirExists(const char *path);
	long GetFileSize(std::string filename);
	std::wstring s2ws(const std::string& s);
	std::string getUserInput();
	char* StringToCharArray(std::string);

	//A shortcut to DownloadClient::DownloadData
	struct CommonStructures::DataInfo DownloadBytes(const char* url);

	char* ReadFile(const char* Name, u32* Length);
	void SaveFile(const char* Name, void* Data, u32 Length);
}