#include "decrypt.h"

Decrypt::Decrypt(QObject *parent) : QObject(parent)
{
}

void Decrypt::start(QString dir, bool confirm)
{
    QDir().setCurrent(dir);
    this->doDecrypt(3, "tmd", "cetk", confirm);
    QDir().setCurrent(dir);
}

quint32 Decrypt::bs24( quint32 i )
{
    return ((i&0xFF0000)>>16) | ((i&0xFF)<<16) | (i&0x00FF00);
}

qulonglong Decrypt::bs64( qulonglong i )
{
    return static_cast<qulonglong>(((static_cast<qulonglong>(bs32(i&0xFFFFFFFF)))<<32) | (bs32(i>>32)));
}

char *Decrypt::_ReadFile( const char *Name, quint32 *Length )
{
    FILE *in;
    fopen_s(&in, Name, "rb");
    if( in == nullptr )
        return nullptr;

    fseek( in, 0, SEEK_END );
    *Length = static_cast<quint32>(ftell(in));

    fseek( in, 0, 0 );

    char *Data = new char[*Length];

    fread( Data, 1, *Length, in );

    fclose( in );

    return Data;
}

void Decrypt::FileDump( const char *Name, void *Data, quint32 Length )
{
    if( Data == nullptr )
    {
        printf("zero ptr");
        return;
    }
    if( Length == 0 )
    {
        printf("zero sz");
        return;
    }
    FILE *Out;
    fopen_s(&Out, Name, "wb");
    if( Out == nullptr )
    {
        perror("");
        return;
    }

    if( fwrite( Data, 1, Length, Out ) != Length )
  {
        perror("");
  }

    fclose( Out );
}

char Decrypt::ascii(char s)
{
  if(s < 0x20) return '.';
  if(s > 0x7E) return '.';
  return s;
}

void Decrypt::hexdump(void *d, qint32 len)
{
  quint8 *data;
  qint32 i, off;
  data = static_cast<quint8*>(d);
  for (off=0; off<len; off += 16)
  {
    printf("%08x  ",off);
    for(i=0; i<16; i++)
      if((i+off)>=len)
          printf("   ");
      else
          printf("%02x ",data[off+i]);

    printf(" ");
    for(i=0; i<16; i++)
      if((i+off)>=len) printf(" ");
      else printf("%c",ascii(static_cast<char>(data[off+i])));
    printf("\n");
  }
}

#define	BLOCK_SIZE	0x10000
void Decrypt::ExtractFileHash( FILE *in, qulonglong PartDataOffset, qulonglong FileOffset, qulonglong Size, char *FileName, quint16 ContentID )
{
    char encdata[BLOCK_SIZE];
    char decdata[BLOCK_SIZE];
    quint8 IV[16];
    quint8 hash[SHA_DIGEST_LENGTH];
    quint8 H0[SHA_DIGEST_LENGTH];
    quint8 Hashes[0x400];

    qulonglong Wrote			= 0;
    qulonglong WriteSize = 0xFC00;	// Hash block size
    qulonglong Block			= (FileOffset / 0xFC00) & 0xF;

    FILE *out;
    fopen_s(&out, FileName, "wb");
    if( out == nullptr )
    {
        printf("Could not create \"%s\"\n", FileName );
        perror("");
        exit(0);
    }

    qulonglong roffset = FileOffset / 0xFC00 * BLOCK_SIZE;
    qulonglong soffset = FileOffset - (FileOffset / 0xFC00 * 0xFC00);

    if( soffset+Size > WriteSize )
        WriteSize = WriteSize - soffset;

    _fseeki64( in, static_cast<qlonglong>(PartDataOffset+roffset), SEEK_SET );
    while(Size > 0)
    {
        if( WriteSize > Size )
            WriteSize = Size;

        fread( encdata, sizeof( char ), BLOCK_SIZE, in);

        memset( IV, 0, sizeof(IV) );
        IV[1] = static_cast<quint8>(ContentID);
        AES_cbc_encrypt( reinterpret_cast<const quint8 *>(encdata), static_cast<quint8 *>(Hashes), 0x400, &_key, IV, AES_DECRYPT );

        memcpy( H0, Hashes+0x14*Block, SHA_DIGEST_LENGTH );

        memcpy( IV, Hashes+0x14*Block, sizeof(IV) );
        if( Block == 0 )
            IV[1] ^= ContentID;
        AES_cbc_encrypt( reinterpret_cast<const quint8 *>(encdata+0x400), reinterpret_cast<quint8 *>(decdata), 0xFC00, &_key, IV, AES_DECRYPT );

        SHA1( reinterpret_cast<const quint8 *>(decdata), 0xFC00, hash );
        if( Block == 0 )
            hash[1] ^= ContentID;
        H0Count++;
        if( memcmp( hash, H0, SHA_DIGEST_LENGTH ) != 0 )
        {
            H0Fail++;
            hexdump( hash, SHA_DIGEST_LENGTH );
            hexdump( Hashes, 0x100 );
            hexdump( decdata, 0x100 );
            printf("Failed to verify H0 hash\n");
            exit(0);
        }

        Size -= fwrite( decdata+soffset, sizeof( char ), WriteSize, out);

        Wrote+=WriteSize;

        Block++;
        if( Block >= 16 )
                Block = 0;

        if( soffset )
        {
            WriteSize = 0xFC00;
            soffset = 0;
        }
    }

    fclose( out );
}
#undef BLOCK_SIZE

#define	BLOCK_SIZE	0x8000
void Decrypt::ExtractFile( FILE *in, qulonglong PartDataOffset, qulonglong FileOffset, qulonglong Size, char *FileName, quint16 ContentID )
{
    char encdata[BLOCK_SIZE];
    char decdata[BLOCK_SIZE];
    qulonglong Wrote=0;

    //calc real offset
    qulonglong roffset = FileOffset / BLOCK_SIZE * BLOCK_SIZE;
    qulonglong soffset = FileOffset - (FileOffset / BLOCK_SIZE * BLOCK_SIZE);
    //printf("Extracting:\"%s\" RealOffset:%08llX RealOffset:%08llX\n", FileName, roffset, soffset );

    FILE *out;
    fopen_s(&out, FileName, "wb");
    if( out == nullptr )
    {
        printf("Could not create \"%s\"\n", FileName );
        perror("");
        exit(0);
    }
    quint8 IV[16];
    memset( IV, 0, sizeof(IV) );
    IV[1] = static_cast<quint8>(ContentID);
    qulonglong WriteSize = BLOCK_SIZE;

    if( soffset+Size > WriteSize )
        WriteSize = WriteSize - soffset;

    _fseeki64( in, static_cast<qlonglong>(PartDataOffset+roffset), SEEK_SET );

    while(Size > 0)
    {
        if( WriteSize > Size )
                WriteSize = Size;

        fread( encdata, sizeof( char ), BLOCK_SIZE, in);
        AES_cbc_encrypt( reinterpret_cast<const quint8 *>(encdata), reinterpret_cast<quint8 *>(decdata), BLOCK_SIZE, &_key, IV, AES_DECRYPT);
        Size -= fwrite( decdata+soffset, sizeof( char ), WriteSize, out);
        Wrote+=WriteSize;

        if( soffset )
        {
            WriteSize = BLOCK_SIZE;
            soffset = 0;
        }
    }

    fclose( out );
}

qint32 Decrypt::doDecrypt(qint32 argc, const char *arg1, const char *arg2, bool confirm)
{
    printf("CDecrypt v 2.0b by crediar\nModified by Tsumes for MapleSeed++\n");
    //printf("Built: %s %s\n", __TIME__, __DATE__);

    if (argc != 3)
    {
        printf("Usage:\n");
        printf(" CDecrypt.exe tmd cetk\n\n");
        return EXIT_SUCCESS;
    }

    quint32 TMDLen;
    char *TMD = _ReadFile(arg1, &TMDLen);
    if (TMD == nullptr)
    {
        perror("Failed to open tmd\n");
        return EXIT_FAILURE;
    }

    quint32 TIKLen;
    char *TIK = _ReadFile(arg2, &TIKLen);
    if (TIK == nullptr)
    {
        perror("Failed to open cetk\n");
        return EXIT_FAILURE;
    }

    TitleMetaData *tmd = reinterpret_cast<TitleMetaData*>(TMD);

    if (tmd->Version != 1)
    {
        printf("Unsupported TMD Version:%u\n", tmd->Version);
        return EXIT_FAILURE;
    }

    printf("Title version:%u\n", bs16(tmd->TitleVersion));
    printf("Content Count:%u\n", bs16(tmd->ContentCount));

    if (strcmp(TMD + 0x140, "Root-CA00000003-CP0000000b") == 0)
    {
        AES_set_decrypt_key(reinterpret_cast<const quint8*>(WiiUCommenKey), sizeof(WiiUCommenKey) * 8, &_key);
    }
    else if (strcmp(TMD + 0x140, "Root-CA00000004-CP00000010") == 0)
    {
        AES_set_decrypt_key(reinterpret_cast<const quint8*>(WiiUCommenDevKey), sizeof(WiiUCommenDevKey) * 8, &_key);
    }
    else
    {
        printf("Unknown Root type:\"%s\"\n", TMD + 0x140);
        return EXIT_FAILURE;
    }

    memset(title_id, 0, sizeof(title_id));
    memcpy(title_id, TMD + 0x18C, 8);
    memcpy(enc_title_key, TIK + 0x1BF, 16);

    AES_cbc_encrypt(enc_title_key, dec_title_key, sizeof(dec_title_key), &_key, title_id, AES_DECRYPT);
    AES_set_decrypt_key(dec_title_key, sizeof(dec_title_key) * 8, &_key);

    char iv[16];
    memset(iv, 0, sizeof(iv));

    QString _str;
    //_str = basedir+QString().sprintf("/%08x.app", _bs32(tmd->Contents[0].ID));

    char str[1024];
    sprintf(str, "%08X.app", bs32(tmd->Contents[0].ID));

    quint32 CNTLen;
    char *CNT = _ReadFile(str, &CNTLen);
    if (CNT == static_cast<char*>(nullptr))
    {
        sprintf(str, "%08X", bs32(tmd->Contents[0].ID));
        CNT = _ReadFile(str, &CNTLen);
        if (CNT == static_cast<char*>(nullptr))
        {
            printf("Failed to open content:%02X\n", bs32(tmd->Contents[0].ID));
            return EXIT_FAILURE;
        }
    }

    if (bs64(tmd->Contents[0].Size) != static_cast<qulonglong>(CNTLen))
    {
        printf("Size of content:%u is wrong: %u:%llu\n", bs32(tmd->Contents[0].ID), CNTLen, bs64(tmd->Contents[0].Size));
        return EXIT_FAILURE;
    }

    AES_cbc_encrypt(reinterpret_cast<const quint8 *>(CNT), reinterpret_cast<quint8 *>(CNT), CNTLen, &_key, reinterpret_cast<quint8*>(iv), AES_DECRYPT);

    if (bs32(*reinterpret_cast<quint32*>(CNT)) != 0x46535400)
    {
        sprintf(str, "%08X.dec", bs32(tmd->Contents[0].ID));
        FileDump(str, CNT, CNTLen);
        return EXIT_FAILURE;
    }

    FST *_fst = reinterpret_cast<FST*>(CNT);

    printf("FSTInfo Entries:%u\n", bs32(_fst->EntryCount));
    if (bs32(_fst->EntryCount) > 90000)
    {
        return EXIT_FAILURE;
    }

    FEntry *fe = reinterpret_cast<FEntry*>(CNT + 0x20 + bs32(_fst->EntryCount) * 0x20);

    quint32 Entries = bs32(*reinterpret_cast<quint32*>(CNT + 0x20 + bs32(_fst->EntryCount) * 0x20 + 8));
    quint32 NameOff = 0x20 + bs32(_fst->EntryCount) * 0x20 + Entries * 0x10;
    //quint32 DirEntries = 0;

    printf("FST entries:%u\n", Entries);

    char *Path = new char[1024];
    qint32 Entry[16];
    qint32 LEntry[16];

    qint32 level = 0;

    if (confirm)
    {
        printf("\n\nStarting decryption for title ID 00050000-%08X", static_cast<quint32>(bs64(tmd->TitleID)));
        printf("\nPress any key to confirm. Press ESC to cancel.\n");
        switch (_getch())
        {
        case 27:
            printf("\nProcess cancelled!!\n");
            return EXIT_FAILURE;
        }
    }

    emit decryptStarted();
    for (quint32 i = 1; i < Entries; ++i)
    {
        if (level)
        {
            while (static_cast<quint32>(LEntry[level - 1]) == i)
            {
                //printf("[%03X]leaving :\"%s\" Level:%d\n", i, CNT + NameOff + bs24( fe[Entry[level-1]].NameOffset ), level );
                level--;
            }
        }

        if (fe[i].u1.s1.Type & 1)
        {
            Entry[level] = static_cast<qint32>(i);
            LEntry[level++] = static_cast<qint32>(bs32(fe[i].u2.s3.NextOffset));
            if (level > 15)	// something is wrong!
            {
                printf("level error:%u\n", level);
                break;
            }
        }
        else
        {
            memset(Path, 0, 1024);

            for (qint32 j = 0; j < level; ++j)
            {
                if (j)
                    Path[strlen(Path)] = '\\';
                memcpy(Path + strlen(Path), CNT + NameOff + bs24(fe[Entry[j]].u1.s1.NameOffset), strlen(CNT + NameOff + bs24(fe[Entry[j]].u1.s1.NameOffset)));
                //_mkdir(Path);
                QDir().mkdir(Path);
            }
            if (level)
                Path[strlen(Path)] = '\\';
            memcpy(Path + strlen(Path), CNT + NameOff + bs24(fe[i].u1.s1.NameOffset), strlen(CNT + NameOff + bs24(fe[i].u1.s1.NameOffset)));

            quint32 CNTSize = bs32(fe[i].u2.s2.FileLength);
            qulonglong CNTOff = (static_cast<qulonglong>(bs32(fe[i].u2.s2.FileOffset)));

            if ((bs16(fe[i].Flags) & 4) == 0)
            {
                CNTOff <<= 5;
            }

            printf("Size:%07X Offset:0x%010llX CID:%02X U:%02X %s\n", CNTSize, CNTOff, bs16(fe[i].ContentID), bs16(fe[i].Flags), Path);

            quint32 ContFileID = bs32(tmd->Contents[bs16(fe[i].ContentID)].ID);

            sprintf(str, "%08X.app", ContFileID);

            if (!(fe[i].u1.s1.Type & 0x80))
            {
                FILE *cnt;
                fopen_s(&cnt, str, "rb");
                if (cnt == nullptr)
                {
                    sprintf(str, "%08X", ContFileID);
                    fopen_s(&cnt, str, "rb");
                    if (cnt == nullptr)
                    {
                        printf("Could not open:\"%s\"\n", str);
                        perror("");
                        return EXIT_FAILURE;
                    }
                }
                if ((bs16(fe[i].Flags) & 0x440))
                {
                    ExtractFileHash(cnt, 0, CNTOff, bs32(fe[i].u2.s2.FileLength), Path, bs16(fe[i].ContentID));
                }
                else
                {
                    ExtractFile(cnt, 0, CNTOff, bs32(fe[i].u2.s2.FileLength), Path, bs16(fe[i].ContentID));
                }
                fclose(cnt);
            }
        }
        progressReport(i, Entries-1);
    }
    emit decryptFinished();
    return EXIT_SUCCESS;
}
