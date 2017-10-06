// Project: CSDecrypt
// File: CDecrypt.cs
// Updated By: Jared
// 

namespace CSDecrypt
{
    public class CDecrypt
    {
        #region ContentType enum

        public enum ContentType
        {
            CONTENT_REQUIRED = 1 << 0, // not sure
            CONTENT_SHARED = 1 << 15,
            CONTENT_OPTIONAL = 1 << 14
        }

        #endregion

        private byte[] dec_title_key = new byte[16];
        private byte[] dkey = new byte[16];

        private byte[] enc_title_key = new byte[16];

        private ulong H0Count = 0;
        private ulong H0Fail = 0;
        private byte[] title_id = new byte[16];

        private byte[] WiiUCommonDevKey =
        {
            0x2F, 0x5C, 0x1B, 0x29, 0x44, 0xE7, 0xFD, 0x6F, 0xC3, 0x97, 0x96, 0x4B, 0x05,
            0x76, 0x91, 0xFA
        };

        private byte[] WiiUCommonKey =
        {
            0xD7, 0xB0, 0x04, 0x02, 0x65, 0x9B, 0xA2, 0xAB, 0xD2, 0xCB, 0x0D, 0xB2, 0x7F,
            0xA2, 0xB6, 0x56
        };

        #region Nested type: Content

        public class Content
        {
            public uint ID; //	0	 0xB04
            public ushort Index; //	4  0xB08
            public byte[] SHA2 = new byte[32]; //  16 0xB14
            public ulong Size; //	8	 0xB0C
            public ushort Type; //	6	 0xB0A
        }

        #endregion

        #region Nested type: ContentInfo

        public class ContentInfo
        {
            public ushort CommandCount; //	2	 0x206
            public ushort IndexOffset; //	0	 0x204
            public byte[] SHA2 = new byte[32]; //  12 0x208
        }

        #endregion

        #region Nested type: FST

        public class FST
        {
            public uint EntryCount;

            private FSTInfo[] FSTInfos;
            public uint MagicBytes;
            public uint Unknown;

            public uint[] UnknownB = new uint[5];
        }

        #endregion

        #region Nested type: FSTInfo

        public class FSTInfo
        {
            public uint Size;
            public uint Unknown;
            public uint UnknownB;
            public uint[] UnknownC = new uint[6];
        }

        #endregion

        #region Nested type: TitleMetaData

        public class TitleMetaData
        {
            public uint AccessRights; // 0x1D8
            public ushort BootIndex; // 0x1E0
            public byte CACRLVersion; // 0x181
            public ushort ContentCount; // 0x1DE 

            private ContentInfo[] ContentInfos = new ContentInfo[64]; //

            private Content[] Contents; // 0x1E4 
            public ushort GroupID; // 0x198 
            public byte[] Issuer = new byte[0x40]; // 0x140

            public byte[] Padding0 = new byte[0x3C]; // 0x104
            public byte Padding1; // 0x183
            public byte[] Padding3 = new byte[2]; // 0x1E2 
            public byte[] Reserved = new byte[62]; // 0x19A 
            public byte[] SHA2 = new byte[32]; // 0x1E4
            public byte[] Signature = new byte[0x100]; // 0x004
            public uint SignatureType; // 0x000
            public byte SignerCRLVersion; // 0x182

            public ulong SystemVersion; // 0x184
            public ulong TitleID; // 0x18C 
            public uint TitleType; // 0x194 
            public ushort TitleVersion; // 0x1DC 

            public byte Version; // 0x180
        }

        #endregion

        public class FEntry
        {
            class EntryType
            {

                ushort FType = 8;
                ushort NameOffset = 24;
            }

            ushort TypeName;

            struct FileEntry
            {
                ushort FileOffset;
                ushort FileLength;
            }

            struct DirEntry
            {
                ushort ParentOffset;
                ushort NextOffset;
            }

            ushort[] entry = new ushort[2];

            public ushort Flags;
            public ushort ContentID;
        }
    }
}