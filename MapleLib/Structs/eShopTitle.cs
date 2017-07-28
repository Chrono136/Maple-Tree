// Project: MapleLib
// File: eShopTitle.cs
// Updated By: Jared
// 

using System;
using System.Text.RegularExpressions;
using MapleLib.Common;
using MapleLib.Interfaces;

namespace MapleLib.Structs
{
    [Serializable]
    public class eShopTitle : ITitle
    {
        private string _name;
        public string ProductCode { get; set; }
        public string CompanyCode { get; set; }
        public string Notes { get; set; }
        public int[] Versions { get; set; } = {0};
        public string Region { get; set; }
        public string ImageLocation { get; set; }
        public bool HasDLC { get; set; }
        public bool HasPatch => Versions.Length > 1;
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

        #region ITitle Members

        public string ID { get; set; }
        public string Key { get; set; }

        public string Name {
            get {
                var name = Toolbelt.RIC(_name);
                return Regex.Replace(name, @"\s+", " ");
            }
            set { _name = value; }
        }

        #endregion

        private string Upper8Digits()
        {
            return string.IsNullOrEmpty(ID) ? string.Empty : ID.Substring(0, 8).ToUpper();
        }

        public string Lower8Digits()
        {
            return string.IsNullOrEmpty(ID) ? string.Empty : ID.Substring(8).ToUpper();
        }

        /// <inheritdoc />
        public override string ToString()
        {
            return Name;
        }
    }
}