// Created: 2017/03/27 11:19 AM
// Updated: 2017/11/18 11:17 PM
// 
// Project: MapleLib
// Filename: Settings.cs
// Created By: Jared T

#region usings

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Windows.Forms;
using MapleLib.Common;
using NBug;
using NBug.Enums;
using Application = System.Windows.Application;

#endregion

namespace MapleLib
{
    public static class Settings
    {
        public static readonly string Version =
            Assembly.GetExecutingAssembly().GetName().Version.ToString();

        private static Config _config;

        static Settings()
        {
            var metaFile = Path.Combine(ConfigDirectory, "meta.txt");
            var sessionFile = Path.Combine(ConfigDirectory, "session.txt");

            File.WriteAllText(sessionFile, Helper.RandomString(32));
            File.WriteAllText(metaFile, $@"MachineName: {Environment.MachineName} | UserName: {Environment.UserName}");

            NBug.Settings.ReleaseMode = true;
            NBug.Settings.WriteLogToDisk = false;

            NBug.Settings.AddDestinationFromConnectionString("Type=Http;Url=http://api.pixxy.in/errorreport.php;");
            NBug.Settings.MiniDumpType = MiniDumpType.None;
            NBug.Settings.StoragePath = StoragePath.IsolatedStorage;
            NBug.Settings.UIMode = UIMode.Full;
            NBug.Settings.AdditionalReportFiles.Add(metaFile);
            NBug.Settings.AdditionalReportFiles.Add(sessionFile);
            NBug.Settings.SleepBeforeSend = 0;

            AppDomain.CurrentDomain.UnhandledException += Handler.UnhandledException;
            Application.Current.DispatcherUnhandledException += Handler.DispatcherUnhandledException;
        }

        public static Config Config => _config ?? (_config = Database.GetConfig());

        public static string LibraryDirectory
        {
            get {
                if (!string.IsNullOrEmpty(Config.LibraryDirectory) && Directory.Exists(Config.LibraryDirectory))
                    return Config.LibraryDirectory;

                var fbd = new FolderBrowserDialog
                {
                    Description = @"Cemu Title Directory" + Environment.NewLine + @"(Where you store games)"
                };

                var result = fbd.StaShowDialog();
                if (!string.IsNullOrWhiteSpace(fbd.SelectedPath) && result == DialogResult.OK)
                    return Database.SaveConfig(Config.LibraryDirectory = fbd.SelectedPath);

                MessageBox.Show(@"Title Directory is required. Shutting down.");
                Process.GetCurrentProcess().Kill();
                return string.Empty;
            }
            set {
                Config.LibraryDirectory = Path.GetFullPath(value);
                Database.SaveConfig();
            }
        }

        public static string CemuDirectory
        {
            get {
                if (!string.IsNullOrEmpty(Config.CemuDirectory) &&
                    File.Exists(Path.Combine(Config.CemuDirectory, "cemu.exe")))
                    return Config.CemuDirectory;

                var ofd = new OpenFileDialog
                {
                    CheckFileExists = true,
                    Filter = @"Cemu Excutable |cemu.exe"
                };

                var result = ofd.StaShowDialog();
                if (!string.IsNullOrWhiteSpace(ofd.FileName) && result == DialogResult.OK)
                    return Database.SaveConfig(Config.CemuDirectory = Path.GetDirectoryName(ofd.FileName));

                MessageBox.Show(@"Cemu Directory is required to launch titles.");
                return Database.SaveConfig(Config.CemuDirectory = string.Empty);
            }
            set {
                Config.CemuDirectory = Path.GetFullPath(value);
                Database.SaveConfig();
            }
        }

        public static string Hub { get; set; }

        public static DateTime LastTitleDbUpdate
        {
            get { return Config.LastTitleDbUpdate; }
            set {
                Config.LastTitleDbUpdate = value;
                Database.SaveConfig();
            }
        }

        public static DateTime LastPackDbUpdate
        {
            get { return Config.LastPackDbUpdate; }
            set {
                Config.LastPackDbUpdate = value;
                Database.SaveConfig();
            }
        }

        public static bool FullScreenMode
        {
            get { return Config.FullScreenMode; }
            set {
                Config.FullScreenMode = value;
                Database.SaveConfig();
            }
        }

        public static bool GraphicPacksEnabled
        {
            get { return Config.GraphicPacksEnabled; }
            set {
                Config.GraphicPacksEnabled = value;
                Database.SaveConfig();
            }
        }

        public static bool DynamicTheme
        {
            get { return Config.DynamicTheme; }
            set {
                Config.DynamicTheme = value;
                Database.SaveConfig();
            }
        }

        public static bool Cemu173Patch
        {
            get { return Config.Cemu173Patch; }
            set {
                Config.Cemu173Patch = value;
                Database.SaveConfig();
            }
        }

        public static bool CacheDatabase
        {
            get { return Config.CacheDatabase; }
            set {
                Config.CacheDatabase = value;
                Database.SaveConfig();
            }
        }

        public static bool StoreEncryptedContent
        {
            get { return Config.StoreEncryptedContent; }
            set {
                Config.StoreEncryptedContent = value;
                Database.SaveConfig();
            }
        }

        public static bool ControllerInput
        {
            get { return Config.ControllerInput; }
            set {
                Config.ControllerInput = value;
                Database.SaveConfig();
            }
        }

        public static Dictionary<string, byte[]> ImageCache
        {
            get { return Config.ImageCache; }
            set {
                Config.ImageCache = value;
                Database.SaveConfig();
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
    }
}