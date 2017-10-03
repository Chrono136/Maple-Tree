// Created: 2017/10/02 12:39 PM
// Updated: 2017/10/02 1:34 PM
// 
// Project: MapleLib
// Filename: TitleKey.cs
// Created By: Jared T

namespace MapleLib.Structs
{
    public struct TitleKey
    {
        public string titleID { get; set; }

        public string titleKey { get; set; }

        public string name { get; set; }

        public string region { get; set; }

        public string ticket { get; set; }

        public string ContentType
        {
            get {
                switch (Upper8Digits())
                {
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

        public string Upper8Digits()
        {
            return string.IsNullOrEmpty(titleID) ? string.Empty : titleID.Substring(0, 8).ToUpper();
        }

        public string Lower8Digits()
        {
            return string.IsNullOrEmpty(titleID) ? string.Empty : titleID.Substring(8).ToUpper();
        }
    }
}