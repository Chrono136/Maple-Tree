// Project: MapleCake
// File: GraphicsPack.cs
// Updated By: Jared
// 

using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Linq;
using IniParser.Parser;
using MapleLib.Collections;
using MapleLib.Common;
using MapleLib.Network;

namespace MapleLib.WiiU
{
    [Serializable]
    public class GraphicPack
    {
        static GraphicPack()
        {
            NewGraphicPack += GraphicPack_NewGraphicPack;
        }

        private static int RetryCount { get; set; }

        public static MapleList<GraphicPack> GraphicPacks { get; private set; }

        private string GPDirectory => Path.Combine(Settings.CemuDirectory, "graphicPacks", Toolbelt.RIC(Name));

        private MapleList<GraphicPackSource> Sources { get; } = new MapleList<GraphicPackSource>();

        public List<string> TitleIds { get; } = new List<string>();

        public string Name { private get; set; }

        private string Rules { get; set; }

        public static event EventHandler<GraphicPack> NewGraphicPack;

        public void Apply()
        {
            if (!Settings.GraphicPacksEnabled || Rules.IsNullOrEmpty())
                return;

            var path = Path.Combine(Settings.CemuDirectory, "graphicPacks");
            if (Directory.GetFileSystemEntries(path).Any()) {
                var moveTo = $"graphicPacks.{Path.GetRandomFileName()}";
                Directory.Move(path, Path.Combine(Settings.CemuDirectory, moveTo));
            }

            if (!string.IsNullOrEmpty(Name) && !Directory.Exists(GPDirectory))
                Directory.CreateDirectory(GPDirectory);

            File.WriteAllText(Path.Combine(GPDirectory, "rules.txt"), Rules);
            Sources.ToList().ForEach(x => File.WriteAllBytes(Path.Combine(GPDirectory, x.FileName), x.Data));
        }

        public void Remove()
        {
            if (Directory.Exists(GPDirectory) && !string.IsNullOrEmpty(Name))
                Directory.Delete(GPDirectory, true);
        }

        private static void GraphicPack_NewGraphicPack(object sender, GraphicPack pack)
        {
            if (!GraphicPacks.Contains(pack))
                GraphicPacks.Add(pack);
        }

        public static async void Init(bool force = false)
        {
            var dbFile = Path.Combine(Settings.ConfigDirectory, "graphicPacks");
            GraphicPacks = new MapleList<GraphicPack>();

            if (!File.Exists(dbFile) || Database.UpdateCheck() || force) {
                TextLog.Write("Downloading graphic packs...");

                const string url = "https://github.com/slashiee/cemu_graphic_packs/archive/master.zip";

                if (Web.UrlExists(url)) {
                    var data = await Web.DownloadDataAsync(url);
                    File.WriteAllBytes(dbFile, data);
                }
                
                TextLog.Write("Downloading graphic packs complete.");
            }

            try {
                using (var zipArchive = new ZipArchive(File.OpenRead(dbFile))) {
                    var list =
                        zipArchive.Entries.Where(x => x.Name.Length == 0 && x.FullName.EndsWith("/")).ToList();

                    list.ForEach(Process);
                }
            }
            catch (Exception e) {
                if (RetryCount >= 3) {
                    TextLog.MesgLog.WriteLog(
                        $"GraphicPacks Init() failed too many times, cancelling...\n\n{e.Message}\n{e.StackTrace}");
                    return;
                }

                RetryCount++;
                File.Delete(dbFile);
                Init(true);
            }
        }

        private static void Process(ZipArchiveEntry entry)
        {
            try {
                var entries = entry.Archive.Entries;
                var files = entries.Where(x => Match(x, entry)).ToList();

                var rules = files.Find(x => x.Name.ToLower() == "rules.txt")?.GetString();
                var sources = files.Where(x => x.Name.ToLower().Contains("source.txt")).ToList();

                if (string.IsNullOrEmpty(rules))
                    return;

                var parser = new IniDataParser
                {
                    Configuration =
                    {
                        SkipInvalidLines = true,
                        AllowDuplicateKeys = true,
                        AllowDuplicateSections = true
                    }
                };
                var data = parser.Parse(rules);

                if (data["Definition"]?["titleIds"] == null || data["Definition"]?["name"] == null)
                    return;

                var value = data["Definition"]?["titleIds"]?.ToUpper();
                if (string.IsNullOrEmpty(value)) return;

                var titleIds = value.Split(',');
                var name = data["Definition"]?["name"].Replace("\"", "").Trim();

                var pack = new GraphicPack {Name = name, Rules = rules};
                pack.TitleIds.AddRange(titleIds);
                foreach (var source in sources) {
                    var graphicPackSource = new GraphicPackSource {FileName = source.Name, Data = source.GetBytes()};
                    pack.Sources.Add(graphicPackSource);
                }

                NewGraphicPack?.Invoke(null, pack);
            }
            catch (Exception e) {
                TextLog.MesgLog.WriteLog($"{e.Message}\n{e.StackTrace}");
            }
        }

        private static bool Match(ZipArchiveEntry x, ZipArchiveEntry entry)
        {
            return Path.GetFullPath($"{Path.GetDirectoryName(x.FullName)}/") == Path.GetFullPath(entry.FullName) &&
                   !string.IsNullOrEmpty(x.Name);
        }

        /// <inheritdoc />
        public override string ToString()
        {
            return Name.Replace("\"", "");
        }

        #region Nested type: GraphicPackSource

        [Serializable]
        private struct GraphicPackSource
        {
            public string FileName { get; set; }
            public byte[] Data { get; set; }
        }

        #endregion
    }
}