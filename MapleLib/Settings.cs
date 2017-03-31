// Project: MapleSeed
// File: Settings.cs
// Updated By: Jared
// 

#region usings

using System;
using System.IO;
using System.Reflection;
using IniParser;
using MapleLib.Properties;

#endregion

namespace MapleLib
{
    public static class Settings
    {
        public static readonly string Version =
            Assembly.GetExecutingAssembly().GetName().Version.ToString();

        public static string TitleDirectory {
            get { return GetKeyValue("TitleDirectory"); }

            set { WriteKeyValue("TitleDirectory", Path.GetFullPath(value)); }
        }

        public static string CemuDirectory {
            get { return GetKeyValue("CemuDirectory"); }

            set { WriteKeyValue("CemuDirectory", Path.GetFullPath(value)); }
        }

        public static string Hub {
            get {
                var value = GetKeyValue("Hub");
                if (string.IsNullOrEmpty(value))
                    WriteKeyValue("Hub", value = "mapletree.tsumes.com");
                return value;
            }

            set { WriteKeyValue("Hub", value); }
        }

        public static bool FullScreenMode {
            get {
                var value = GetKeyValue("FullScreenMode");
                if (string.IsNullOrEmpty(value))
                    WriteKeyValue("FullScreenMode", false.ToString());

                return GetKeyValue("FullScreenMode") == "True";
            }

            set { WriteKeyValue("FullScreenMode", value.ToString()); }
        }

        public static bool DynamicTheme
        {
            get
            {
                var value = GetKeyValue("DynamicTheme");
                if (string.IsNullOrEmpty(value))
                    WriteKeyValue("DynamicTheme", false.ToString());

                return GetKeyValue("DynamicTheme") == "True";
            }

            set { WriteKeyValue("DynamicTheme", value.ToString()); }
        }

        public static bool Cemu173Patch {
            get {
                var value = GetKeyValue("Cemu173Patch");
                if (string.IsNullOrEmpty(value))
                    WriteKeyValue("Cemu173Patch", true.ToString());

                return GetKeyValue("Cemu173Patch") == "True";
            }

            set { WriteKeyValue("Cemu173Patch", value.ToString()); }
        }

        public static bool StoreEncryptedContent {
            get {
                var value = GetKeyValue("StoreEncryptedContent");
                if (string.IsNullOrEmpty(value))
                    WriteKeyValue("StoreEncryptedContent", true.ToString());

                return GetKeyValue("StoreEncryptedContent") == "True";
            }

            set { WriteKeyValue("StoreEncryptedContent", value.ToString()); }
        }

        private static string ConfigFile {
            get {
                var configFile = Path.Combine(ConfigDirectory, "MapleSeed.ini");

                if (!Directory.Exists(ConfigDirectory))
                    Directory.CreateDirectory(ConfigDirectory);

                if (!File.Exists(configFile) || new FileInfo(configFile).Length <= 0)
                    File.WriteAllText(configFile, Resources.Settings_DefaultSettings);

                return configFile;
            }
        }

        private static string ConfigName => "MapleTree";
        private static string AppFolder => Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);
        public static string ConfigDirectory => Path.Combine(AppFolder, ConfigName);
        
        public static string BasePatchDir => GetBasePatchDir();

        private static string GetBasePatchDir()
        {
            if (!Directory.Exists(CemuDirectory))
                throw new DirectoryNotFoundException("Settings.CemuDirectory path could not be found");

            return Path.GetFullPath(Path.Combine(CemuDirectory, "mlc01/usr/title/00050000"));
        }

        private static string GetKeyValue(string key)
        {
            var parser = new FileIniDataParser();
            var data = parser.ReadFile(ConfigFile);
            return data[ConfigName][key] ?? "";
        }

        private static void WriteKeyValue(string key, string value)
        {
            var parser = new FileIniDataParser();
            var data = parser.ReadFile(ConfigFile);
            data[ConfigName][key] = value;
            parser.WriteFile(ConfigFile, data);
        }
    }
}