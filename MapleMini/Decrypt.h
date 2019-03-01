/*
CDecrypt - Decrypt Wii U NUS content files [https://code.google.com/p/cdecrypt/]

Copyright (c) 2013-2015 crediar

CDecrypt is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Modified by: Tsumes 2019
*/
#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/sha.h>
#include "time.h"
#include <vector>
#include <direct.h>
#include <ctype.h>

#pragma comment(lib, "legacy_stdio_definitions.lib")
#pragma comment(lib,"libeay32MTd.lib")

extern "C" { FILE __iob_func[3] = { *stdin,*stdout,*stderr }; }

class Decrypt
{
public:
	int dirExists(const char *path)
	{
		struct stat info;

		if (stat(path, &info) != 0)
			return 0;
		else if (info.st_mode & S_IFDIR)
			return 1;
		else
			return 0;
	}

	void simpleDecrypt(const char* path, char* defaultDir)
	{
		_chdir(path);

		struct stat buffer;
		if ((stat("tmd", &buffer) == 0) && (stat("cetk", &buffer) == 0))
		{
			if (!(ret = _decrypt(NULL, "tmd", "cetk")))
				std::cout << "Content Decryption Complete.\n\n";

			_chdir(defaultDir);
		}
	}

	static std::thread* Threads;

	unsigned char WiiUCommenDevKey[16] =
	{
		0x2F, 0x5C, 0x1B, 0x29, 0x44, 0xE7, 0xFD, 0x6F, 0xC3, 0x97, 0x96, 0x4B, 0x05, 0x76, 0x91, 0xFA,
	};
	unsigned char WiiUCommenKey[16] =
	{
		0xD7, 0xB0, 0x04, 0x02, 0x65, 0x9B, 0xA2, 0xAB, 0xD2, 0xCB, 0x0D, 0xB2, 0x7F, 0xA2, 0xB6, 0x56,
	};

	AES_KEY key;
	u8 enc_title_key[16];
	u8 dec_title_key[16];
	u8 title_id[16];
	u8 dkey[16];

	u64 H0Count = 0;
	u64 H0Fail = 0;

#pragma pack(1)

	enum ContentType
	{
		CONTENT_REQUIRED = (1 << 0), // not sure
		CONTENT_SHARED = (1 << 15),
		CONTENT_OPTIONAL = (1 << 14),
	};

	typedef struct
	{
		u16 IndexOffset; //	0	 0x204
		u16 CommandCount; //	2	 0x206
		u8 SHA2[32]; //  12 0x208
	} ContentInfo;

	typedef struct
	{
		u32 ID; //	0	 0xB04
		u16 Index; //	4  0xB08
		u16 Type; //	6	 0xB0A
		u64 Size; //	8	 0xB0C
		u8 SHA2[32]; //  16 0xB14
	} Content;

	typedef struct
	{
		u32 SignatureType; // 0x000
		u8 Signature[0x100]; // 0x004

		u8 Padding0[0x3C]; // 0x104
		u8 Issuer[0x40]; // 0x140

		u8 Version; // 0x180
		u8 CACRLVersion; // 0x181
		u8 SignerCRLVersion; // 0x182
		u8 Padding1; // 0x183

		u64 SystemVersion; // 0x184
		u64 TitleID; // 0x18C 
		u32 TitleType; // 0x194 
		u16 GroupID; // 0x198 
		u8 Reserved[62]; // 0x19A 
		u32 AccessRights; // 0x1D8
		u16 TitleVersion; // 0x1DC 
		u16 ContentCount; // 0x1DE 
		u16 BootIndex; // 0x1E0
		u8 Padding3[2]; // 0x1E2 
		u8 SHA2[32]; // 0x1E4

		ContentInfo ContentInfos[64];

		Content Contents[9999]; // 0x1E4 
	} TitleMetaData;

	struct FSTInfo
	{
		u32 Unknown;
		u32 Size;
		u32 UnknownB;
		u32 UnknownC[6];
	};

	struct FST
	{
		u32 MagicBytes;
		u32 Unknown;
		u32 EntryCount;

		u32 UnknownB[5];

		FSTInfo FSTInfos[9999];
	};

	struct FEntry
	{
		union
		{
			struct
			{
				u32 Type : 8;
				u32 NameOffset : 24;
			};

			u32 TypeName;
		};

		union
		{
			struct // File Entry
			{
				u32 FileOffset;
				u32 FileLength;
			};

			struct // Dir Entry
			{
				u32 ParentOffset;
				u32 NextOffset;
			};

			u32 entry[2];
		};

		unsigned short Flags;
		unsigned short ContentID;
	};
	
#define bs16(s) (u16)( ((s)>>8) | ((s)<<8) )
#define bs32(s) (u32)( (((s)&0xFF0000)>>8) | (((s)&0xFF00)<<8) | ((s)>>24) | ((s)<<24) )

	u32 bs24(u32 i);

	u64 bs64(u64 i);

	char* ReadFile(const char* Name, u32* Length);

	void FileDump(const char* Name, void* Data, u32 Length);

	static char ascii(char s);

	void hexdump(void* d, s32 len);

#define	BLOCK_SIZE	0x10000
	void ExtractFileHash(FILE* in, u64 PartDataOffset, u64 FileOffset, u64 Size, char* FileName, u16 ContentID);
#undef BLOCK_SIZE
#define	BLOCK_SIZE	0x8000

	void ExtractFile(FILE* in, u64 PartDataOffset, u64 FileOffset, u64 Size, char* FileName, u16 ContentID);

	s32 _decrypt(char*, const char*, const char*);

	static int ret;

	Decrypt(char*);
	Decrypt(const Decrypt & obj);
	Decrypt();
	~Decrypt();

#pragma pack(pop)
};