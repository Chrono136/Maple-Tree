// Created: 2017/03/30 5:22 AM
// Updated: 2017/09/29 2:08 AM
// 
// Project: MapleLib
// Filename: MapleTicket.cs
// Created By: Jared T

using System.Collections.Generic;
using MapleLib.Common;
using MapleLib.Databases;
using MapleLib.Properties;
using MapleLib.Structs;

namespace MapleLib.WiiU
{
    public static class MapleTicket
    {
        private const int TK = 0x140;

        private static void PatchDlc(ref List<byte> ticketData)
        {
            ticketData.InsertRange(TK + 0x164, Resources.DLCPatch);
        }

        private static void PatchDemo(ref List<byte> ticketData)
        {
            ticketData.InsertRange(TK + 0x124, new byte[0x00 * 64]);
        }

        /// <summary>
        ///     Creates a blank ticket using the referenced title
        /// </summary>
        /// <param name="title">The title</param>
        /// <returns></returns>
        public static byte[] Create(TitleKey title)
        {
            if (string.IsNullOrEmpty(title.titleID))
                return null;

            var tiktem =
            ("00010004d15ea5ed15abe11ad15ea5ed15abe11ad15ea5ed15abe11ad15ea5ed15abe11a" +
             "d15ea5ed15abe11ad15ea5ed15abe11ad15ea5ed15abe11ad15ea5ed15abe11ad15ea5ed" +
             "15abe11ad15ea5ed15abe11ad15ea5ed15abe11ad15ea5ed15abe11ad15ea5ed15abe11a" +
             "d15ea5ed15abe11ad15ea5ed15abe11ad15ea5ed15abe11ad15ea5ed15abe11ad15ea5ed" +
             "15abe11ad15ea5ed15abe11ad15ea5ed15abe11ad15ea5ed15abe11ad15ea5ed15abe11a" +
             "d15ea5ed15abe11ad15ea5ed15abe11ad15ea5ed15abe11ad15ea5ed15abe11ad15ea5ed" +
             "15abe11ad15ea5ed15abe11ad15ea5ed15abe11ad15ea5ed15abe11ad15ea5ed15abe11a" +
             "d15ea5ed15abe11a00000000000000000000000000000000000000000000000000000000" +
             "0000000000000000000000000000000000000000000000000000000000000000526f6f74" +
             "2d434130303030303030332d585330303030303030630000000000000000000000000000" +
             "000000000000000000000000000000000000000000000000feedfacefeedfacefeedface" +
             "feedfacefeedfacefeedfacefeedfacefeedfacefeedfacefeedfacefeedfacefeedface" +
             "feedfacefeedfacefeedface010000cccccccccccccccccccccccccccccccc0000000000" +
             "0000000000000000aaaaaaaaaaaaaaaa0000000000000000000000000000000000000000" +
             "000000000000000000000000000000000000000000000000000000000000000000000000" +
             "000000000001000000000000000000000000000000000000000000000000000000000000" +
             "000000000000000000000000000000000000000000000000000000000000000000000000" +
             "000000000000000000000000000000000000000000000000000000000000000000000000" +
             "0000000000000000000000000000000000000000000000000000000000010014000000ac" +
             "000000140001001400000000000000280000000100000084000000840003000000000000" +
             "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff00000000" +
             "000000000000000000000000000000000000000000000000000000000000000000000000" +
             "000000000000000000000000000000000000000000000000000000000000000000000000" +
             "0000000000000000000000000000000000000000").HexToBytes();

            var tikdata = new List<byte>(tiktem);

            switch (title.ContentType)
            {
                case "DLC":
                    PatchDlc(ref tikdata);
                    break;
                case "Demo":
                    PatchDemo(ref tikdata);
                    break;
            }

            var versions = WiiuTitleDatabase.GetVersions($"00050000{title.Lower8Digits()}");

            tikdata.InsertRange(TK + 0xA6, versions[0].ToBytes());
            tikdata.InsertRange(TK + 0x9C, title.titleID.ToUpper().HexToBytes());
            tikdata.InsertRange(TK + 0x7F, title.titleKey.ToUpper().HexToBytes());

            return tikdata.ToArray();
        }
    }
}