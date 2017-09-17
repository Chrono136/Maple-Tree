// Project: MapleLib
// File: TitleEntry.cs
// Updated By: Jared
// 

using System;
using System.ComponentModel;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using MapleLib.Common;
using MapleLib.Network;
using MapleLib.Properties;
using MapleLib.WiiU;

namespace MapleLib.Structs
{
    [Serializable]
    public class Title : eShopTitle
    {
        [Browsable(false)]
        public string FolderLocation { get; set; }

        [Browsable(false)]
        public string MetaLocation { get; set; }

        [Browsable(false)]
        public GraphicPack SelectedGraphicPack { private get; set; }

        public override string ToString()
        {
            var cType = ContentType.Contains("App") ? "" : $"[{ContentType}]";
            return Toolbelt.RIC($"{cType}[{Region}] {Name}");
        }

        public void PlayTitle()
        {
            if (MetaLocation.IsNullOrEmpty())
                return;

            new Thread(() => {
                if (!Toolbelt.LaunchCemu(MetaLocation, SelectedGraphicPack)) return;
                TextLog.MesgLog.WriteLog($"Now Playing: {Name}");
            }).Start();
        }

        public BindingList<GraphicPack> GetGraphicPacks()
        {
            var packs = Database.FindGraphicPacks(ID?.ToUpper());
            packs.Insert(0, new GraphicPack {Name = "No Graphic Pack", TitleIds = {"0000000000000000"}});

            return new BindingList<GraphicPack>(packs);
        }

        public int GetTitleVersion()
        {
            if (!File.Exists(MetaLocation) || Path.GetExtension(MetaLocation) != ".xml")
                return 0;

            var versionStr = Helper.XmlGetStringByTag(MetaLocation, "title_version");

            int version;
            return int.TryParse(versionStr, out version) ? version : Versions[0];
        }

        public int GetUpdateVersion()
        {
            var metaLocation = Path.Combine(Settings.BasePatchDir, Lower8Digits(), "meta", "meta.xml");
            if (!File.Exists(metaLocation) || Path.GetExtension(metaLocation) != ".xml")
                return 0;

            var versionStr = Helper.XmlGetStringByTag(metaLocation, "title_version");

            int version;
            return int.TryParse(versionStr, out version) ? version : Versions[0];
        }

        public int GetDLCVersion()
        {
            var metaLocation = Path.Combine(Settings.BasePatchDir, Lower8Digits(), "aoc", "meta", "meta.xml");
            if (!File.Exists(metaLocation))
                return 0;

            var versionStr = Helper.XmlGetStringByTag(metaLocation, "title_version");

            int version;
            return int.TryParse(versionStr, out version) ? version : 0;
        }

        public async Task DownloadContent(string version = "0")
        {
            await this.DownloadContent(ContentType, version);
        }

        public async Task DownloadUpdate(string version = "0")
        {
            await this.DownloadContent("Patch", version);
        }

        public async Task DownloadDLC()
        {
            await this.DownloadContent("DLC", "0");
        }

        public bool DeleteContent()
        {
            var ret = false;
            var path = Path.GetFullPath(FolderLocation);

            var result = MessageBox.Show(string.Format(Resources.ActionWillDeleteAllContent, path),
                Resources.PleaseConfirmAction, MessageBoxButtons.OKCancel);

            if (result == DialogResult.OK)
                Directory.Delete(path, ret = true);

            return ret;
        }

        public void DeleteUpdateContent()
        {
            var updatePath = Path.Combine(Settings.BasePatchDir, Lower8Digits());

            if (Directory.Exists(Path.Combine(updatePath, "code")))
                Directory.Delete(Path.Combine(updatePath, "code"), true);

            if (Directory.Exists(Path.Combine(updatePath, "meta")))
                Directory.Delete(Path.Combine(updatePath, "meta"), true);

            if (Directory.Exists(Path.Combine(updatePath, "content")))
                Directory.Delete(Path.Combine(updatePath, "content"), true);

            if (File.Exists(Path.Combine(updatePath, "result.log")))
                File.Delete(Path.Combine(updatePath, "result.log"));
        }

        public void DeleteAddOnContent()
        {
            var updatePath = Path.Combine(Settings.BasePatchDir, Lower8Digits(), "aoc");

            if (Directory.Exists(updatePath))
                Directory.Delete(updatePath, true);
        }
    }
}