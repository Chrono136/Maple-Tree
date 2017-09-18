// Project: MapleLib
// File: WiiuTitleDatabase.cs
// Updated By: Jared
// 

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using LiteDB;
using MapleLib.Collections;
using MapleLib.Common;
using MapleLib.Interfaces;
using MapleLib.Network;
using MapleLib.Properties;
using MapleLib.Structs;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace MapleLib.Databases
{
    public class WiiuTitleDatabase : IDatabase<Title>
    {
        public WiiuTitleDatabase(LiteDatabase db)
        {
            LiteDatabase = db;
            Load();
        }

        private LiteCollection<Title> Col => LiteDatabase?.GetCollection<Title>(CollectionName);

        public MapleDictionary TitleLibrary { get; } = new MapleDictionary(Settings.LibraryDirectory);

        #region IDatabase<Title> Members

        /// <inheritdoc />
        public LiteDatabase LiteDatabase { get; }

        /// <inheritdoc />
        public string CollectionName => "titles";

        /// <inheritdoc />
        public int Count => LiteDatabase?.GetCollection<Title>(CollectionName).Count(Query.All()) ?? 0;

        /// <inheritdoc />
        public void Load()
        {
            Task.Run(() => InitDatabase());
        }

        /// <inheritdoc />
        public async void InitDatabase()
        {
            if (Database.Time2Update(Settings.LastTitleDbUpdate) || Count < 1) {
                TextLog.Write("[Title Database] Building database...");

                LiteDatabase.DropCollection(CollectionName);

                var db = await Create();
                foreach (var item in db) {
                    if (Col.Find(x => x.ID == item.ID).Any())
                        continue;

                    Col.Insert(item);
                    Col.EnsureIndex(x => x.Name);
                }

                Settings.LastTitleDbUpdate = DateTime.Now;
            }

            LoadLibrary(Settings.LibraryDirectory);

            TextLog.Write($"[Title Database] Loaded {Count} entries");
            Database.DatabaseCount++;
        }

        /// <inheritdoc />
        public MapleList<Title> Find(string id)
        {
            id = id.ToUpperInvariant();
            var col = LiteDatabase.GetCollection<Title>(CollectionName);

            if (!col.Exists(x => x.ID == id))
                return new MapleList<Title>();

            var titles = col.Find(x => x.ID == id);
            return new MapleList<Title>(titles);
        }

        #endregion
        
        public IEnumerable<Title> All()
        {
            var col = LiteDatabase.GetCollection<Title>(CollectionName);
            var items = col.Find(x => x.Name != null);
            items = items.Where(x => x.AvailableOnCDN);
            return new MapleList<Title>(items);
        }

        private static async Task<MapleList<Title>> Create()
        {
            var eShopTitlesStr = Resources.eShopAndDiskTitles; //index 12
            var eShopTitleUpdates = Resources.eShopTitleUpdates; //index 9

            var db = new MapleList<Title>();

            await Task.Run(() => {
                var titlekeys = WiiUTitleKeys();
                foreach (var wiiutitleKey in titlekeys) {
                    var id = wiiutitleKey["titleID"].Value<string>()?.ToUpper();
                    var key = wiiutitleKey["titleKey"].Value<string>()?.ToUpper();
                    var name = wiiutitleKey["name"].Value<string>();
                    var region = wiiutitleKey["region"].Value<string>();

                    if (string.IsNullOrEmpty(id) || string.IsNullOrEmpty(key))
                        continue;

                    db.Add(new Title {ID = id, Key = key, Name = name, Region = region});
                }

                var lines = eShopTitlesStr.Replace("|", "").Split('\n').ToList();
                for (var i = 11; i < lines.Count; i++) {
                    var id = lines[i++].Replace("-", "").Trim().ToUpper();
                    var title = db.FirstOrDefault(x => x.ID == id) ?? new Title();

                    title.ID = id;
                    title.Name = lines[i++].Trim();
                    title.ProductCode = lines[i++].Trim();
                    title.CompanyCode = lines[i++].Trim();
                    title.Notes = lines[i++].Trim();
                    title.Versions = lines[i++].ToIntList(',');
                    title.Region = lines[i++].Trim();

                    var num = i++;
                    var line = lines[num].ToLower().Trim();

                    if (!line.Contains("yes") && !line.Contains("no"))
                        continue;

                    title.AvailableOnCDN = lines[num].ToLower().Contains("yes");

                    if (!db.Contains(title))
                        db.Add(title);
                }

                lines = eShopTitleUpdates.Replace("|", "").Split('\n').ToList();
                for (var i = 9; i < lines.Count; i++) {
                    var line = lines[i].Trim();

                    if (!line.Contains("-10"))
                        continue;

                    var versionStr = lines[i + 3].Trim();
                    var versions = versionStr.ToIntList(',');

                    var titleId = line.Replace("-", "").ToUpper();
                    var titles = db.ToList().FindAll(t => t.ID.Contains(titleId.Substring(8)));

                    foreach (var title in titles)
                        if (title.ContentType.Contains("eShop"))
                            title.Versions = versions;
                }

                foreach (var title in db.Where(x => x.ContentType == "eShop/Application")) {
                    var id = $"0005000C{title.Lower8Digits().ToUpper()}";

                    JObject jtitle;
                    if ((jtitle = WiiUTitleKey(titlekeys, id)) != null)
                        title.HasDLC = jtitle.HasValues;
                }
            });

            return db;
        }

        private void LoadLibrary(string titleDirectory)
        {
            if (Count < 1)
                throw new DirectoryNotFoundException("Title Database is empty. This should not happen!!!");

            if (string.IsNullOrEmpty(titleDirectory))
                throw new DirectoryNotFoundException($"TitleDir: '{titleDirectory}' cannot be null or empty");

            if (titleDirectory.FilePathHasInvalidChars())
                throw new DirectoryNotFoundException($"TitleDir: '{titleDirectory}' is an invalid directory path");

            var xmlFiles = Helper.GetFiles(titleDirectory, "meta.xml");

            foreach (var xmlFile in xmlFiles) {
                var rootDir = Path.GetFullPath(Path.Combine(xmlFile, "../../"));
                var titleId = Helper.XmlGetStringByTag(xmlFile, "title_id");

                Title title;
                if ((title = Database.FindTitle(titleId)) == null) {
                    TextLog.Write($"Could not find title using ID {titleId}");
                    continue;
                }

                title.FolderLocation = rootDir;
                title.MetaLocation = xmlFile;
                TitleLibrary.AddOnUI(title);
            }
        }

        private static List<JObject> WiiUTitleKeys()
        {
            var url_1 = "http://wiiu.titlekeys.gq/json";
            var url_2 = "https://wiiu.titlekeys.com/json";

            var urls = new[] {url_1, url_2};

            var jsonStr = string.Empty;

            for (var i = 0; i < urls.Length; i++) {
                var url = urls[i];

                if (Web.UrlExists(url)) {
                    jsonStr = Web.DownloadString(url);
                    break;
                }

                TextLog.Write($"Failed to download db from {url}, falling back to embedded option");
                jsonStr = Resources.wiiutitlekey;
            }

            var jsonTitles = JsonConvert.DeserializeObject<ICollection<JObject>>(jsonStr);
            return jsonTitles.ToList();
        }

        private static JObject WiiUTitleKey(IEnumerable<JObject> jsonTitles, string id)
        {
            return jsonTitles.ToList().Find(x => x["titleID"].Value<string>().ToUpper() == id);
        }
    }
}