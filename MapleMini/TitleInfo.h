#pragma once

class TitleInfo
{
public:
	TitleInfo(char* str, size_t len, const char * outputDir);
	TitleInfo();
	~TitleInfo();

	static void CreateDatabase();

	int DownloadContent();

	std::string workingDir;
	std::string outputDir;

	char* uid;
	char* id;
	char* key;
	char* name;
	char* region;
	char* versions;
	char* hasdlc;
	char* haspatch;
	char* contenttype;
	char* cdn;
	char* pcode;
	char* ccode;
	char* iloc;
	char* notes;

private:
};

