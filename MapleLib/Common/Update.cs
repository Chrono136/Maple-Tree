// Project: MapleLib
// File: Update.cs
// Updated By: Jared
// 

using System.Linq;
using MapleLib.Enums;
using MapleLib.Network;

namespace MapleLib.Common
{
    public class Update
    {
        public Update(UpdateType build)
        {
            var versionStrs = Web.DownloadString(VersionUrl).Split('\n');

            if (build == UpdateType.MapleSeed) {
                CurrentVersion = System.Reflection.Assembly.GetEntryAssembly().GetName().Version.ToString();

                if (versionStrs.Length == 2)
                    LatestVersion = versionStrs[0];
            }

            if (build == UpdateType.MapleSeed2) {
                CurrentVersion = Settings.Version;

                if (versionStrs.Length == 2)
                    LatestVersion = versionStrs[1];
            }

            if (CurrentVersion != LatestVersion)
                IsAvailable = true;
        }

        private static string VersionUrl => "https://raw.githubusercontent.com/Tsume/Maple-Tree/master/version";

        public string LatestVersion { get; }

        public string CurrentVersion { get; }

        public bool IsAvailable { get; }
    }
}