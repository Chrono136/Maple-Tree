#pragma once

class TitleInfo
{
public:
	TitleInfo(char* str, size_t len);
	TitleInfo();
	~TitleInfo();

	static void CreateDatabase();

	int DownloadContent(const char * outputDir);

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
	const char* _outputPath;
};

