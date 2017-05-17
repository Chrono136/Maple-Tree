// Project: MapleLib
// File: Update.cs
// Updated By: Jared
// 

using System;
using System.Drawing;
using System.Reflection;
using System.Windows.Forms;
using MapleLib.Enums;
using MapleLib.Network;

namespace MapleLib.Common
{
    public class Update : IDisposable
    {
        public Update(UpdateType build)
        {
            UpdateType = build;

            var versionStrs = Web.DownloadString(VersionUrl).Split('\n');

            if (UpdateType == UpdateType.MapleSeed) {
                CurrentVersion = Assembly.GetEntryAssembly().GetName().Version.ToString();

                if (versionStrs.Length == 2)
                    LatestVersion = versionStrs[0];
            }

            if (UpdateType == UpdateType.MapleSeed2) {
                CurrentVersion = Settings.Version;

                if (versionStrs.Length == 2)
                    LatestVersion = versionStrs[1];
            }

            if (CurrentVersion != LatestVersion)
                IsAvailable = true;
        }

        public void CheckForUpdate()
        {
            if (!IsAvailable) return;

            TextLog.MesgLog.WriteLog($"Update Available!! Latest Version: {LatestVersion}", Color.Green);

            MessageBox.Show(@"Please visit https://github.com/Tsume/Maple-Tree/releases for the latest release.",
                $@"Version Mis-Match - Latest: {LatestVersion}");
        }

        private static string VersionUrl => "http://raw.githubusercontent.com/Tsume/Maple-Tree/master/version";

        private UpdateType UpdateType { get; }

        public string LatestVersion { get; }

        public string CurrentVersion { get; }

        public bool IsAvailable { get; }

        public void Dispose() {}
    }
}