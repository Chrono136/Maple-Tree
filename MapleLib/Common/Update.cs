// Created: 2017/05/05 9:17 PM
// Updated: 2017/09/29 11:57 PM
// 
// Project: MapleLib
// Filename: Update.cs
// Created By: Jared T

using System;
using System.Drawing;
using System.Reflection;
using System.Threading.Tasks;
using MapleLib.Enums;
using MapleLib.Network;

namespace MapleLib.Common
{
    public class Update : IDisposable
    {
        public Update(UpdateType build)
        {
            UpdateType = build;

            Task.Run(() => SetVersion()).Wait();

            switch (UpdateType)
            {
                case UpdateType.MapleSeed:
                    CurrentVersion = Assembly.GetEntryAssembly().GetName().Version.ToString();
                    if (VersionStrings.Length == 2)
                        LatestVersion = VersionStrings[0];
                    break;

                case UpdateType.MapleSeed2:
                    CurrentVersion = Settings.Version;
                    if (VersionStrings.Length == 2)
                        LatestVersion = VersionStrings[1];
                    break;

                default:
                    throw new ArgumentOutOfRangeException();
            }

            if (CurrentVersion != LatestVersion)
                IsAvailable = true;
        }

        private static string VersionUrl => "http://raw.githubusercontent.com/Tsume/Maple-Tree/master/version";

        private UpdateType UpdateType { get; }

        private string LatestVersion { get; }

        private string CurrentVersion { get; }

        private string[] VersionStrings { get; set; }

        private bool IsAvailable { get; }

        #region IDisposable Members

        public void Dispose() { }

        #endregion

        private void SetVersion()
        {
            var result = Web.DownloadString(VersionUrl);
            VersionStrings = result.Split('\n');
        }

        public void CheckForUpdate()
        {
            if (!IsAvailable) return;

            TextLog.MesgLog.WriteLog($"[New Update] Latest Released Version: {LatestVersion}", Color.Green);

#if !DEBUG
            System.Windows.Forms.MessageBox.Show(@"Please visit https://github.com/Tsume/Maple-Tree/releases for the latest release.",
                $@"Version Mis-Match - Latest: {LatestVersion}");
#endif
        }
    }
}