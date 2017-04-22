// Project: MapleLib
// File: eShopTitle.cs
// Updated By: Jared
// 

using MapleLib.Common;
using MapleLib.Interfaces;

namespace MapleLib.Structs
{
    public class eShopTitle : ITitle
    {
        public string ID { get; set; }
        public string Key { get; set; }
        public string Name { get; set; }
        public string ProductCode { get; set; }
        public string CompanyCode { get; set; }
        public string Notes { get; set; }
        public int[] Versions { get; set; } = {0};
        public string Region { get; set; }
        public string ImageLocation { get; set; }
        public bool HasDLC { get; set; }
        public bool AvailableOnCDN { get; set; }

        public string ContentType {
            get {
                switch (Upper8Digits()) {
                    case "00050010":
                    case "0005001B":
                        return "System Application";

                    case "00050000":
                        return "eShop/Application";

                    case "00050002":
                        return "Demo";

                    case "0005000E":
                        return "Patch";

                    case "0005000C":
                        return "DLC";
                }
                return "Unknown";
            }
        }

        private string Upper8Digits()
        {
            return ID?.Substring(0, 8).ToUpper();
        }

        public string Lower8Digits()
        {
            return ID?.Substring(8).ToUpper();
        }

        /// <inheritdoc />
        public override string ToString()
        {
            return Toolbelt.RIC(Name);
        }
    }
}