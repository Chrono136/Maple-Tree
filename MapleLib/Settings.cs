// Created: 2017/03/27 11:19 AM
// Updated: 2017/09/29 1:52 AM
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
using System.Threading;
using System.Windows.Forms;
using System.Windows.Media.Imaging;
using MapleLib.BaseClasses;
using MapleLib.Common;
using MapleLib.Properties;

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
            AppDomain.CurrentDomain.UnhandledException += CurrentDomain_UnhandledException;
            Application.ThreadException += Application_ThreadException;
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

        public static Dictionary<string, BitmapFrame> ImageCache
        {
            get { return Config.ImageCache; }
            set
            {
                Config.ImageCache = value;
                Database.SaveConfig();
            }
        }

        private static string ConfigName => "MapleTree";
        private static string AppFolder => Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);
        public static string ConfigDirectory => Path.Combine(AppFolder, ConfigName);

        public static string BasePatchDir => GetBasePatchDir();

        private static void Application_ThreadException(object sender, ThreadExceptionEventArgs e)
        {
            var ex = e.Exception;
            File.WriteAllText("error.log", string.Format(Resources.ThreadException, ex.Message, ex.StackTrace));
            MessageBox.Show(@"error.log has been created containing details of this error.");
        }

        private static void CurrentDomain_UnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            File.WriteAllText("error.log", $@"{e.ExceptionObject}");
            MessageBox.Show($@"{e.ExceptionObject}");
        }

        private static string GetBasePatchDir()
        {
            if (!Directory.Exists(CemuDirectory))
                throw new DirectoryNotFoundException("Settings.CemuDirectory path could not be found");

            return Path.GetFullPath(Path.Combine(CemuDirectory, "mlc01/usr/title/00050000"));
        }
    }
}