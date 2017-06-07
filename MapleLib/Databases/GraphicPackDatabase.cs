// Project: MapleLib
// File: GraphicPacks.cs
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
using MapleLib.Interfaces;
using MapleLib.Network;
using MapleLib.WiiU;

namespace MapleLib.Databases
{
    public class GraphicPackDatabase : IDatabase<GraphicPack>
    {
        public GraphicPackDatabase(LiteDatabase db)
        {
            LiteDatabase = db;
            Load();
        }

        private int RetryCount { get; set; }

        private LiteCollection<GraphicPack> Col => LiteDatabase?.GetCollection<GraphicPack>(CollectionName);

        #region IDatabase<GraphicPack> Members

        /// <inheritdoc />
        public LiteDatabase LiteDatabase { get; }

        /// <inheritdoc />
        public string CollectionName => "graphicpacks";

        /// <inheritdoc />
        public int Count => Col?.Count(Query.All()) ?? 0;

        /// <inheritdoc />
        public void Load()
        {
            Task.Run(() => InitDatabase());
        }

        /// <inheritdoc />
        public async void InitDatabase()
        {
            if (Database.Time2Update(Settings.LastPackDbUpdate) || Count < 1) {
                TextLog.Write("[Graphic Packs] Building database...");

                LiteDatabase.DropCollection(CollectionName);

                var db = await Create();
                if (db == null) {
                    TextLog.Write("[Graphic Packs] Error: Not available");
                    return;
                }

                foreach (var item in db) {
                    if (Col.Find(x => x.Name == item.Name).Any())
                        continue;

                    Col.Insert(item);
                    Col.EnsureIndex(x => x.Name);
                }

                Settings.LastPackDbUpdate = DateTime.Now;
            }

            TextLog.Write($"[Graphic Packs] Loaded {Count} entries");
        }

        public MapleList<GraphicPack> Find(string id)
        {
            var col = LiteDatabase.GetCollection<GraphicPack>(CollectionName);

            var title = col.Find(x => x.TitleIdString.Contains(id));
            return new MapleList<GraphicPack>(title);
        }

        #endregion

        private async Task<MapleList<GraphicPack>> Create(bool force = false)
        {
            var graphicPacks = new MapleList<GraphicPack>();
            byte[] graphicPackBytes = {0};

            if (Database.Time2Update(Settings.LastPackDbUpdate) || force) {
                const string url = "https://github.com/slashiee/cemu_graphic_packs/archive/master.zip";

                if (Web.UrlExists(url))
                    graphicPackBytes = await Web.DownloadDataAsync(url);
            }

            try {
                if (graphicPackBytes.Length <= 1)
                    return null;

                using (var zipArchive = new ZipArchive(new MemoryStream(graphicPackBytes))) {
                    var list = zipArchive.Entries?.Where(x => x.Name.Length == 0 && x.FullName.EndsWith("/")).ToList();

                    if (list == null)
                        return null;

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
    }
}