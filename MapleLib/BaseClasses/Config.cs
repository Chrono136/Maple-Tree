// Project: MapleLib
// File: Config.cs
// Updated By: Jared
// 

using System;

namespace MapleLib.BaseClasses
{
    public class Config
    {
        public string Index { get; } = "Config";

        public string LibraryDirectory { get; set; } = "";

        public string CemuDirectory { get; set; } = "";

        public bool FullScreenMode { get; set; }

        public bool GraphicPacksEnabled { get; set; } = true;

        public bool DynamicTheme { get; set; }

        public bool Cemu173Patch { get; set; } = true;

        public bool CacheDatabase { get; set; }

        public bool StoreEncryptedContent { get; set; } = true;

        public DateTime LastTitleDBUpdate { get; set; } = DateTime.Now.AddDays(-30);

        public DateTime LastPackDBUpdate { get; set; } = DateTime.Now.AddDays(-30);
    }
}