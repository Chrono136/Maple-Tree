// Created: 2017/05/14 12:53 PM
// Updated: 2017/09/29 1:54 AM
// 
// Project: MapleLib
// Filename: Config.cs
// Created By: Jared T

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

        public DateTime LastTitleDbUpdate { get; set; } = DateTime.Now.AddDays(-30);

        public DateTime LastPackDbUpdate { get; set; } = DateTime.Now.AddDays(-30);
    }
}