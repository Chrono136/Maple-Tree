#include "stdafx.h"
#include "Decrypt.h"


int Decrypt::ret = 0;
std::thread* Decrypt::Threads;

Decrypt::Decrypt(char* defaultDir)
{
	struct stat buffer;
	if ((stat("tmd", &buffer) == 0) && (stat("cetk", &buffer) == 0)) {
		simpleDecrypt("", defaultDir);
		getchar();
		return;
	}

	cout << rang::style::bold << "MapleMini Console Version 1.0\n" << endl;
	cout << rang::style::bold << "Built: " << __TIME__ << " " << __DATE__ << endl << endl;

	cout << rang::style::bold << "Command Line Options: " << "[] Required, {} Optional" << rang::style::reset << endl;
	cout << " Decypt: MapleMini.exe [\"c:\\Location\\to\\encrypted\\content\"]" << endl;
	//cout << " Download: MapleMini.exe {-dl} [Title ID]" << endl << endl;

	cout << rang::style::bold << "Input Options: " << "[] Required, {} Optional" << rang::style::reset << endl;
	//cout << " Download: [-dl] [\"Title ID\"]" << endl;
	cout << " Decrypt: [-de] [\"c:\\Location\\to\\encrypted\\content\"]" << endl << endl;

	cout << rang::style::bold << "----------------------------------------------------------" << endl << endl;

	while (true)
	{
		//-de "D:\Emulators\WiiU_Roms\[USA] Mutant Mudds Deluxe"
		auto input = Toolbelt::getUserInput();

		std::string path;
		std::regex re("\\\"(.*)\\\"");
		std::smatch match;
		if (std::regex_search(input, match, re) && match.size() > 1) {
			path = match.str(1);
		}
		else path = std::string("");

		if (input[0] == '-' && input[1] == 'd' && input[2] == 'l' && path.length() > 2)
		{
			continue;
		}

		if (input[0] == '-' && input[1] == 'd' && input[2] == 'e' && path.length() > 2)
		{
			simpleDecrypt(Toolbelt::StringToCharArray(path), defaultDir);
			continue;
		}

		if (path.length() > 2 && dirExists(Toolbelt::StringToCharArray(path)))
		{
			simpleDecrypt(Toolbelt::StringToCharArray(path), defaultDir);
			break;
		}

		if (strcmp(Toolbelt::StringToCharArray(input), "exit") == 0)
			exit(0);
	}
}

Decrypt::Decrypt(const Decrypt & obj)
{

}

Decrypt::Decrypt()
{

}

Decrypt::~Decrypt()
{
}


u32 Decrypt::bs24(u32 i)
{
	return ((i & 0xFF0000) >> 16) | ((i & 0xFF) << 16) | (i & 0x00FF00);
}

u64 Decrypt::bs64(u64 i)
{
	return static_cast<u64>(bs32(i & 0xFFFFFFFF)) << 32 | bs32(i >> 32);
}

char* Decrypt::ReadFile(const char* Name, u32* Length)
{
	FILE* in = fopen(Name, "rb");
	if (in == nullptr)
	{
		//perror("");
		return nullptr;
	}

	fseek(in, 0, SEEK_END);
	*Length = ftell(in);

	fseek(in, 0, 0);

	char* Data = new char[*Length];

	size_t read = fread(Data, 1, *Length, in);

	fclose(in);

	return Data;
}

void Decrypt::FileDump(const char* Name, void* Data, u32 Length)
{
	if (Data == nullptr)
	{
		printf("zero ptr");
		return;
	}
	if (Length == 0)
	{
		printf("zero sz");
		return;
	}
	FILE* Out = fopen(Name, "wb");
	if (Out == nullptr)
	{
		perror("");
		return;
	}

	if (fwrite(Data, 1, Length, Out) != Length)
	{
		perror("");
	}

	fclose(Out);
}

char Decrypt::ascii(char s)
{
	if (s < 0x20) return '.';
	if (s > 0x7E) return '.';
	return s;
}

void Decrypt::hexdump(void* d, s32 len)
{
	u8* data;
	s32 i, off;
	data = static_cast<u8*>(d);
	for (off = 0; off < len; off += 16)
	{
		printf("%08x  ", off);
		for (i = 0; i < 16; i++)
			if ((i + off) >= len)
				printf("   ");
			else
				printf("%02x ", data[off + i]);

		printf(" ");
		for (i = 0; i < 16; i++)
			if ((i + off) >= len) printf(" ");
			else printf("%c", ascii(data[off + i]));
			printf("\n");
	}
}

void Decrypt::ExtractFileHash(FILE* in, u64 PartDataOffset, u64 FileOffset, u64 Size, char* FileName, u16 ContentID)
{
	char encdata[BLOCK_SIZE];
	char decdata[BLOCK_SIZE];
	u8 IV[16];
	u8 hash[SHA_DIGEST_LENGTH];
	u8 H0[SHA_DIGEST_LENGTH];
	u8 Hashes[0x400];

	u64 Wrote = 0;
	u64 WriteSize = 0xFC00;	// Hash block size
	u64 Block = (FileOffset / 0xFC00) & 0xF;

	FILE *out = fopen(FileName, "wb");
	if (out == NULL)
	{
		printf("Could not create \"%s\"\n", FileName);
		perror("");
		exit(0);
	}

	u64 roffset = FileOffset / 0xFC00 * BLOCK_SIZE;
	u64 soffset = FileOffset - (FileOffset / 0xFC00 * 0xFC00);

	if (soffset + Size > WriteSize)
		WriteSize = WriteSize - soffset;

	fseeko(in, PartDataOffset + roffset, SEEK_SET);
	while (Size > 0)
	{
		if (WriteSize > Size)
			WriteSize = Size;

		fread(encdata, sizeof(char), BLOCK_SIZE, in);

		memset(IV, 0, sizeof(IV));
		IV[1] = (u8)ContentID;
		AES_cbc_encrypt((const u8 *)(encdata), (u8 *)Hashes, 0x400, &key, IV, AES_DECRYPT);

		memcpy(H0, Hashes + 0x14 * Block, SHA_DIGEST_LENGTH);

		memcpy(IV, Hashes + 0x14 * Block, sizeof(IV));
		if (Block == 0)
			IV[1] ^= ContentID;
		AES_cbc_encrypt((const u8 *)(encdata + 0x400), (u8 *)decdata, 0xFC00, &key, IV, AES_DECRYPT);

		SHA1((const u8 *)decdata, 0xFC00, hash);
		if (Block == 0)
			hash[1] ^= ContentID;
		H0Count++;
		if (memcmp(hash, H0, SHA_DIGEST_LENGTH) != 0)
		{
			H0Fail++;
			hexdump(hash, SHA_DIGEST_LENGTH);
			hexdump(Hashes, 0x100);
			hexdump(decdata, 0x100);
			printf("Failed to verify H0 hash\n");
			exit(0);
		}

		Size -= fwrite(decdata + soffset, sizeof(char), WriteSize, out);

		Wrote += WriteSize;

		Block++;
		if (Block >= 16)
			Block = 0;

		if (soffset)
		{
			WriteSize = 0xFC00;
			soffset = 0;
		}
	}

	fclose(out);
}

void Decrypt::ExtractFile(FILE* in, u64 PartDataOffset, u64 FileOffset, u64 Size, char* FileName, u16 ContentID)
{
	char encdata[BLOCK_SIZE];
	char decdata[BLOCK_SIZE];
	u64 Wrote = 0;
	u64 Block = (FileOffset / BLOCK_SIZE) & 0xF;

	//printf("PO:%08llX FO:%08llX FS:%llu\n", PartDataOffset, FileOffset, Size );

	//calc real offset
	u64 roffset = FileOffset / BLOCK_SIZE * BLOCK_SIZE;
	u64 soffset = FileOffset - (FileOffset / BLOCK_SIZE * BLOCK_SIZE);
	//printf("Extracting:\"%s\" RealOffset:%08llX RealOffset:%08llX\n", FileName, roffset, soffset );

	FILE* out = fopen(FileName, "wb");
	if (out == nullptr)
	{
		printf("Could not create \"%s\"\n", FileName);
		perror("");
		exit(0);
	}
	u8 IV[16];
	memset(IV, 0, sizeof(IV));
	IV[1] = static_cast<u8>(ContentID);

	u64 WriteSize = BLOCK_SIZE;

	if (soffset + Size > WriteSize)
		WriteSize = WriteSize - soffset;

	_fseeki64(in, PartDataOffset + roffset, SEEK_SET);

	while (Size > 0)
	{
		if (WriteSize > Size)
			WriteSize = Size;

		fread(encdata, sizeof(char), BLOCK_SIZE, in);

		AES_cbc_encrypt(reinterpret_cast<const u8 *>(encdata), reinterpret_cast<u8 *>(decdata), BLOCK_SIZE, &key, IV, AES_DECRYPT);

		Size -= fwrite(decdata + soffset, sizeof(char), WriteSize, out);

		Wrote += WriteSize;

		if (soffset)
		{
			WriteSize = BLOCK_SIZE;
			soffset = 0;
		}
	}

	fclose(out);
}

s32 Decrypt::_decrypt(char* argc, const char* arg1, const char* arg2)
{
	char str[1024];

	u32 TMDLen;
	char* TMD = ReadFile(arg1, &TMDLen);
	if (TMD == nullptr)
	{
		perror("Failed to open tmd\n");
		getchar();
		return EXIT_FAILURE;
	}

	u32 TIKLen;
	char* TIK = ReadFile(arg2, &TIKLen);
	if (TIK == nullptr)
	{
		perror("Failed to open cetk\n");
		getchar();
		return EXIT_FAILURE;
	}

	TitleMetaData* tmd = reinterpret_cast<TitleMetaData*>(TMD);
	
	if (tmd->Version != 1)
	{
		printf("Unsupported TMD Version:%u\n", tmd->Version);
		getchar();
		return EXIT_FAILURE;
	}

	printf("Title version:%u\n", bs16(tmd->TitleVersion));
	printf("Content Count:%u\n", bs16(tmd->ContentCount));

	if (strcmp(TMD + 0x140, "Root-CA00000003-CP0000000b") == 0)
	{
		AES_set_decrypt_key(static_cast<const u8*>(WiiUCommenKey), sizeof(WiiUCommenKey) * 8, &key);
	}
	else if (strcmp(TMD + 0x140, "Root-CA00000004-CP00000010") == 0)
	{
		AES_set_decrypt_key(static_cast<const u8*>(WiiUCommenDevKey), sizeof(WiiUCommenDevKey) * 8, &key);
	}
	else
	{
		printf("Unknown Root type:\"%s\"\n", TMD + 0x140);
		getchar();
		return EXIT_FAILURE;
	}

	memset(title_id, 0, sizeof(title_id));

	memcpy(title_id, TMD + 0x18C, 8);
	memcpy(enc_title_key, TIK + 0x1BF, 16);

	AES_cbc_encrypt(enc_title_key, dec_title_key, sizeof(dec_title_key), &key, title_id, AES_DECRYPT);
	AES_set_decrypt_key(dec_title_key, sizeof(dec_title_key) * 8, &key);

	char iv[16];
	memset(iv, 0, sizeof(iv));

	sprintf(str, "%08X.app", bs32(tmd->Contents[0].ID));

	u32 CNTLen;
	char* CNT = ReadFile(str, &CNTLen);
	if (CNT == static_cast<char*>(nullptr))
	{
		sprintf(str, "%08X", bs32(tmd->Contents[0].ID));
		CNT = ReadFile(str, &CNTLen);
		if (CNT == static_cast<char*>(nullptr))
		{
			printf("Failed to open content:%02X\n", bs32(tmd->Contents[0].ID));
			getchar();
			return EXIT_FAILURE;
		}
	}

	if (bs64(tmd->Contents[0].Size) != static_cast<u64>(CNTLen))
	{
		printf("Size of content:%u is wrong: %u:%I64u\n", bs32(tmd->Contents[0].ID), CNTLen, bs64(tmd->Contents[0].Size));
		getchar();
		return EXIT_FAILURE;
	}

	AES_cbc_encrypt(reinterpret_cast<const u8 *>(CNT), reinterpret_cast<u8 *>(CNT), CNTLen, &key, reinterpret_cast<u8*>(iv), AES_DECRYPT);

	if (bs32(*(u32*)CNT) != 0x46535400)
	{
		sprintf(str, "%08X.dec", bs32(tmd->Contents[0].ID));
		FileDump(str, CNT, CNTLen);
		getchar();
		return EXIT_FAILURE;
	}

	FST* _fst = reinterpret_cast<FST*>(CNT);

	printf("FSTInfo Entries:%u\n", bs32(_fst->EntryCount));
	if (bs32(_fst->EntryCount) > 90000)
	{
		getchar();
		return EXIT_FAILURE;
	}

	FEntry* fe = reinterpret_cast<FEntry*>(CNT + 0x20 + bs32(_fst->EntryCount) * 0x20);

	u32 Entries = bs32(*(u32*)(CNT + 0x20 + bs32(_fst->EntryCount) * 0x20 + 8));
	u32 NameOff = 0x20 + bs32(_fst->EntryCount) * 0x20 + Entries * 0x10;
	u32 DirEntries = 0;

	printf("FST entries:%u\n", Entries);

	char* Path = new char[1024];
	s32 Entry[16];
	s32 LEntry[16];

	s32 level = 0;

	Decrypt::Threads = new std::thread[Entries];

	printf("\n\nStarting decryption for title ID %#010x", (unsigned int)tmd->TitleID);
	printf("\nPress any key to confirm. Press ESC to cancel.\n");
	switch (_getch())
	{
	case 27:
		printf("\nProcess cancelled!!\n");
		return EXIT_FAILURE;
		break;
	}

	for (u32 i = 1; i < Entries; ++i)
	{
		if (level)
		{
			while (LEntry[level - 1] == i)
			{
				printf("[%03X]leaving :\"%s\" Level:%d\n", i, CNT + NameOff + bs24( fe[Entry[level-1]].NameOffset ), level );
				level--;
			}
		}

		if (fe[i].Type & 1)
		{
			Entry[level] = i;
			LEntry[level++] = bs32(fe[i].NextOffset);
			if (level > 15) // something is wrong!
			{
				printf("level error:%u\n", level);
				break;
			}
		}
		else
		{
			memset(Path, 0, 1024);

			for (s32 j = 0; j < level; ++j)
			{
				if (j)
					Path[strlen(Path)] = '\\';
				memcpy(Path + strlen(Path), CNT + NameOff + bs24(fe[Entry[j]].NameOffset), strlen(CNT + NameOff + bs24(fe[Entry[j]].NameOffset)));
				_mkdir(Path);
			}
			if (level)
				Path[strlen(Path)] = '\\';
			memcpy(Path + strlen(Path), CNT + NameOff + bs24(fe[i].NameOffset), strlen(CNT + NameOff + bs24(fe[i].NameOffset)));

			u32 CNTSize = bs32(fe[i].FileLength);
			u64 CNTOff = static_cast<u64>(bs32(fe[i].FileOffset));

			if ((bs16(fe[i].Flags) & 4) == 0)
			{
				CNTOff <<= 5;
			}

			printf("Size:%07X Offset:0x%010llX CID:%02X U:%02X %s\n", CNTSize, CNTOff, bs16(fe[i].ContentID), bs16(fe[i].Flags), Path);

			u32 ContFileID = bs32(tmd->Contents[bs16(fe[i].ContentID)].ID);

			sprintf(str, "%08X.app", ContFileID);

			if (!(fe[i].Type & 0x80))
			{
				FILE* cnt = fopen(str, "rb");
				if (cnt == nullptr)
				{
					sprintf(str, "%08X", ContFileID);
					cnt = fopen(str, "rb");
					if (cnt == nullptr)
					{
						printf("Could not open:\"%s\"\n", str);
						perror("");
						getchar();
						return EXIT_FAILURE;
					}
				}
				if ((bs16(fe[i].Flags) & 0x440))
				{
					int fileLen = bs32(fe[i].FileLength);
					int contentID = bs16(fe[i].ContentID);
					Decrypt::ExtractFileHash(std::ref(cnt), 0, std::ref(CNTOff), std::ref(fileLen), std::ref(Path), std::ref(contentID));

					//Threads[i] = std::thread(&Decrypt::ExtractFileHash, this, std::ref(cnt), 0, std::ref(CNTOff), std::ref(fileLen), std::ref(Path), std::ref(contentID));
					//Threads[i].join();
				}
				else
				{
					ExtractFile(cnt, 0, CNTOff, bs32(fe[i].FileLength), Path, bs16(fe[i].ContentID));
				}
				fclose(cnt);
			}
		}
	}

	//for (u32 i = 1; i < Entries; ++i) Threads[i].join();

	return EXIT_SUCCESS;
}