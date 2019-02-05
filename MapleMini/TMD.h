#pragma once
class TMD
{
public:
	TMD();
	~TMD();


	bool fakeSign = false;
	uint signatureExponent = 0x00010001;
	char* signature;
	char* padding;
	char* issuer;
	char version;
	char caCrlVersion;
	char signerCrlVersion;
	char paddingByte;
	ulong startupIos;
	ulong titleId;
	uint titleType;
	ushort groupId;
	ushort padding2;
	ushort region;
	char* reserved;
	uint accessRights;
	ushort titleVersion;
	ushort numOfContents;
	ushort bootIndex;
	ushort padding3;
	TMD_Content contents[1];
};

enum ContentType
{
	Normal = 0x0001,
	DLC = 0x4001,
	Shared = 0x8001,
};

enum Regions
{
	Japan = 0,
	USA = 1,
	Europe = 2,
	Free = 3,
};