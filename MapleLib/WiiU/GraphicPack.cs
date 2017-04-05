// Project: MapleCake
// File: GraphicsPack.cs
// Updated By: Jared
// 

using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using IniParser.Parser;
using MapleLib.Collections;
using MapleLib.Common;
using MapleLib.Network;

namespace MapleLib.WiiU
{
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

        private string Name { get; set; }

        private string Rules { get; set; }

        public List<string> TitleIds { get; } = new List<string>();

        public static event EventHandler<GraphicPack> NewGraphicPack;

        public void Apply()
        {
            if (!Settings.GraphicPacksEnabled)
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

        public static GraphicPack FindPack(string title_id)
        {
            return GraphicPacks.FirstOrDefault(x => x.TitleIds.ToList().Contains(title_id.ToUpper()));
        }

        public static async void Init(bool force = false)
        {
            if (!Settings.GraphicPacksEnabled)
                return;

            var databaseFile = Path.Combine(Settings.ConfigDirectory, "graphicPacks");
            GraphicPacks = new MapleList<GraphicPack>();

            if (!File.Exists(databaseFile) || Settings.CacheDatabase || force) {
                TextLog.MesgLog.WriteLog(@"Building graphic pack database...");

                const string url = "https://github.com/slashiee/cemu_graphic_packs/archive/master.zip";
                var data = await Web.DownloadDataAsync(url);
                File.WriteAllBytes(databaseFile, data);
            }

            try {
                if (!File.Exists(databaseFile)) return;
                TextLog.MesgLog.WriteLog(@"Loading graphic pack database...");
                var data = File.ReadAllBytes(databaseFile);

                using (var zipArchive = new ZipArchive(new MemoryStream(data))) {
                    zipArchive.Entries.Where(x => x.Name.Length == 0 && x.FullName.EndsWith("/"))
                        .ToList()
                        .ForEach(Process);
                }
            }
            catch (Exception e) {
                if (RetryCount >= 3) {
                    TextLog.MesgLog.WriteLog(
                        $"GraphicPacks Init() failed too many times, cancelling...\n\n{e.Message}\n{e.StackTrace}");
                    return;
                }

                RetryCount++;
                File.Delete(databaseFile);
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
                //MessageBox.Show($@"{e.Message}\n{e.StackTrace}");
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

        private struct GraphicPackSource
        {
            public string FileName { get; set; }
            public byte[] Data { get; set; }
        }

        #endregion
    }
}