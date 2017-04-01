// Project: MapleLib
// File: Ticket.cs
// Updated By: Jared
// 

using System.Collections.Generic;
using MapleLib.Common;
using MapleLib.Properties;
using MapleLib.Structs;

namespace MapleLib.WiiU
{
    public static class MapleTicket
    {
        private static readonly int TK = 0x140;

        private static void PatchDLC(ref List<byte> ticketData)
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
        public static byte[] Create(Title title)
        {
            var TIKTEM =
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

            var tikdata = new List<byte>(TIKTEM);

            if (title.ContentType == "DLC")
                PatchDLC(ref tikdata);

            if (title.ContentType == "Demo")
                PatchDemo(ref tikdata);

            tikdata.InsertRange(TK + 0xA6, title.Versions[0].ToBytes());
            tikdata.InsertRange(TK + 0x9C, title.ID.HexToBytes());
            tikdata.InsertRange(TK + 0x7F, title.Key.HexToBytes());

            return tikdata.ToArray();
        }
    }
}