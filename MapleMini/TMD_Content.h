#pragma once
class TMD_Content
{
public:
	TMD_Content();
	~TMD_Content();

	uint contentId;
	ushort index;
	ushort type;
	ulong size;
	char* hash;
};

