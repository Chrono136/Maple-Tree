// Project: MapleLib
// File: GraphicPackDatabase.cs
// Updated By: Jared
// 

using System;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Threading.Tasks;
using IniParser.Parser;
using LiteDB;
using MapleLib.Collections;
using MapleLib.Common;
using MapleLib.Network;
using MapleLib.WiiU;

namespace MapleLib
{
    public static class PackDatabase
    {
        public static int Count => GetCount();

        private static int RetryCount { get; set; }

        private static LiteDatabase LiteDatabase { get; set; }

        private static string CollectionName => "graphicpacks";

        public static async void Load()
        {
            if (LiteDatabase == null)
                LiteDatabase = await InitDatabase();
        }

        public static MapleList<GraphicPack> Find(string id)
        {
            var col = LiteDatabase.GetCollection<GraphicPack>(CollectionName);

            var title = col.Find(x => x.TitleIdString.Contains(id));
            return new MapleList<GraphicPack>(title);
        }

        private static async Task<LiteDatabase> InitDatabase()
        {
            var dbFile = Path.GetFullPath(Path.Combine(Settings.ConfigDirectory, $"{CollectionName}.db"));

            if (!Settings.CacheDatabase && File.Exists(dbFile))
                return new LiteDatabase(Helper.FileOpenStream(dbFile));

            TextLog.Write("Building graphic packs database...");

            using (var tdb = new LiteDatabase(dbFile)) {
                tdb.DropCollection(CollectionName);

                var col = tdb.GetCollection<GraphicPack>(CollectionName);

                var db = await Create();
                for (var i = 0; i < db.Count; i++) {
                    var item = db[i];

                    if (col.Find(x => x.Name == item.Name).Any())
                        continue;

                    col.Insert(item);
                    col.EnsureIndex(x => x.Name);
                }

                TextLog.Write("Building graphic packs database complete.");
            }

            return new LiteDatabase(Helper.FileOpenStream(dbFile));
        }

        private static async Task<MapleList<GraphicPack>> Create(bool force = false)
        {
            var dbFile = Path.Combine(Settings.ConfigDirectory, "graphicPacks");
            var graphicPacks = new MapleList<GraphicPack>();

            if (!File.Exists(dbFile) || Database.UpdateCheck() || force) {
                const string url = "https://github.com/slashiee/cemu_graphic_packs/archive/master.zip";

                if (Web.UrlExists(url)) {
                    var data = await Web.DownloadDataAsync(url);
                    File.WriteAllBytes(dbFile, data);
                }
            }

            try {
                using (var zipArchive = new ZipArchive(File.OpenRead(dbFile))) {
                    var list = zipArchive.Entries.Where(x => x.Name.Length == 0 && x.FullName.EndsWith("/")).ToList();

                    foreach (var zipArchiveEntry in list) {
                        var pack = Process(zipArchiveEntry);

                        if (pack != null && !graphicPacks.Contains(pack))
                            graphicPacks.Add(pack);
                    }
                }

                return graphicPacks;
            }
            catch (Exception e) {
                if (RetryCount >= 3) {
                    TextLog.MesgLog.WriteLog(
                        $"GraphicPacks Init() failed too many times, cancelling...\n\n{e.Message}\n{e.StackTrace}");
                    return null;
                }

                RetryCount++;
                File.Delete(dbFile);
                return await Create(true);
            }
        }

        private static GraphicPack Process(ZipArchiveEntry entry)
        {
            try {
                var entries = entry.Archive.Entries;
                var files = entries.Where(x => Match(x, entry)).ToList();

                var rules = files.Find(x => x.Name.ToLower() == "rules.txt")?.GetString();
                var sources = files.Where(x => x.Name.ToLower().Contains("source.txt")).ToList();

                if (string.IsNullOrEmpty(rules))
                    return null;

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
                    return null;

                var value = data["Definition"]?["titleIds"]?.ToUpper();
                if (string.IsNullOrEmpty(value)) return null;

                var titleIds = value.Split(',');
                var name = data["Definition"]?["name"].Replace("\"", "").Trim();

                var pack = new GraphicPack {Name = name, Rules = rules};
                pack.TitleIds.AddRange(titleIds);
                foreach (var source in sources) {
                    var graphicPackSource = new GraphicPack.GraphicPackSource
                    {
                        FileName = source.Name,
                        Data = source.GetBytes()
                    };
                    pack.Sources.Add(graphicPackSource);
                }

                return pack;
            }
            catch (Exception e) {
                TextLog.MesgLog.WriteLog($"{e.Message}\n{e.StackTrace}");
                return null;
            }
        }

        private static bool Match(ZipArchiveEntry x, ZipArchiveEntry entry)
        {
            return Path.GetFullPath($"{Path.GetDirectoryName(x.FullName)}/") == Path.GetFullPath(entry.FullName) &&
                   !string.IsNullOrEmpty(x.Name);
        }

        private static int GetCount()
        {
            return LiteDatabase.GetCollection<GraphicPack>(CollectionName).Count(Query.All());
        }
    }
}