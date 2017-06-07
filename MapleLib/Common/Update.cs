// Project: MapleLib
// File: Update.cs
// Updated By: Jared
// 

using System;
using System.Drawing;
using System.Reflection;
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

        private static string VersionUrl => "http://raw.githubusercontent.com/Tsume/Maple-Tree/master/version";

        private UpdateType UpdateType { get; }

        private string LatestVersion { get; }

        private string CurrentVersion { get; }

        private bool IsAvailable { get; }

        #region IDisposable Members

        public void Dispose() {}

        #endregion

        public void CheckForUpdate()
        {
            if (!IsAvailable) return;

            TextLog.MesgLog.WriteLog($"[New Update] Latest Released Version: {LatestVersion}", Color.Green);

#if !DEBUG
            MessageBox.Show(@"Please visit https://github.com/Tsume/Maple-Tree/releases for the latest release.",
                $@"Version Mis-Match - Latest: {LatestVersion}");
#endif
        }
    }
}