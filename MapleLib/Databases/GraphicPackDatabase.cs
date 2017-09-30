// Created: 2017/05/14 5:33 AM
// Updated: 2017/09/29 11:07 PM
// 
// Project: MapleLib
// Filename: GraphicPackDatabase.cs
// Created By: Jared T

using System;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Threading.Tasks;
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

        private async Task<MapleList<GraphicPack>> Create()
        {
            var graphicPacks = new MapleList<GraphicPack>();

            try
            {
                const string url = "http://github.com/slashiee/cemu_graphic_packs/archive/master.zip";

                byte[] graphicPackBytes;
                if ((graphicPackBytes = await Web.DownloadDataAsync(url)) == null)
                    return null;

                if (graphicPackBytes.Length <= 1)
                    return null;

                using (var zipArchive = new ZipArchive(new MemoryStream(graphicPackBytes)))
                {
                    var list = zipArchive.Entries?.Where(x => x.Name.Length == 0 && x.FullName.EndsWith("/")).ToList();

                    if (list == null)
                        return null;

                    foreach (var zipArchiveEntry in list)
                    {
                        var pack = Process(zipArchiveEntry);

                        if (pack != null && !graphicPacks.Contains(pack))
                            graphicPacks.Add(pack);
                    }
                }

                return graphicPacks;
            }
            catch (Exception e)
            {
                if (RetryCount >= 3)
                {
                    TextLog.Write($"GraphicPacks Init() failed, cancelling...\n\n{e.Message}\n{e.StackTrace}");
                    return null;
                }

                RetryCount++;
                return await Create();
            }
        }

        private static GraphicPack Process(ZipArchiveEntry entry)
        {
            try
            {
                var entries = entry.Archive.Entries;
                var files = entries.Where(x => Match(x, entry) && !x.FullName.Contains("Cheats")).ToList();

                var rules = files.Find(x => x.Name.ToLower() == "rules.txt")?.GetString();
                var sources = files.Where(x => !x.Name.ToLower().Contains("rules.txt")).ToList();

                if (string.IsNullOrEmpty(rules))
                    return null;

                var iniFile = new IniManager(rules);
                var titleIdsValue = iniFile.GetValue("Definition", "titleIds", null);
                var nameValue = iniFile.GetValue("Definition", "name", null);

                if (titleIdsValue == null || nameValue == null) return null;

                var value = titleIdsValue.ToUpper();
                if (string.IsNullOrEmpty(value)) return null;

                var titleIds = value.Split(',');
                var name = nameValue.Replace("\"", "").Trim();

                var pack = new GraphicPack {Name = name, Rules = rules};
                pack.TitleIds.AddRange(titleIds);
                foreach (var source in sources)
                {
                    var graphicPackSource = new GraphicPack.GraphicPackSource
                    {
                        FileName = source.Name,
                        Data = source.GetBytes()
                    };
                    pack.Sources.Add(graphicPackSource);
                }

                return pack;
            }
            catch (Exception e)
            {
                TextLog.MesgLog.WriteLog($"{e.Message}\n{e.StackTrace}");
                return null;
            }
        }

        private static bool Match(ZipArchiveEntry x, ZipArchiveEntry entry)
        {
            return Path.GetFullPath($"{Path.GetDirectoryName(x.FullName)}/") == Path.GetFullPath(entry.FullName) &&
                   !string.IsNullOrEmpty(x.Name);
        }

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
            var doUpdate = Settings.LastPackDbUpdate < DateTime.Now.AddDays(-7);

            if (doUpdate || Settings.CacheDatabase || Count < 1)
            {
                TextLog.Write("[Graphic Packs] Building database...");

                LiteDatabase.DropCollection(CollectionName);

                MapleList<GraphicPack> db;
                if ((db = await Create()) == null || db.Count <= 0)
                {
                    TextLog.Write("[Graphic Packs] Error: Not available");
                    return;
                }

                foreach (var item in db)
                {
                    if (Col.Find(x => x.Name == item.Name).Any())
                        continue;

                    Col.Insert(item);
                    Col.EnsureIndex(x => x.Name);
                }

                Settings.LastPackDbUpdate = DateTime.Now;
            }

            TextLog.Write($"[Graphic Packs] Loaded {Count} entries");
            Database.DatabaseCount++;
        }

        public MapleList<GraphicPack> Find(string id)
        {
            var col = LiteDatabase.GetCollection<GraphicPack>(CollectionName);

            var title = col.Find(x => x.TitleIdString.Contains(id));
            return new MapleList<GraphicPack>(title);
        }

        #endregion
    }
}