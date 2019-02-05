#pragma once

class TitleInfo
{
public:
	TitleInfo();
	~TitleInfo();

	TitleInfo(char* str, size_t len);

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
};

