// Project: MapleLib
// File: WiiuTitleDatabase.cs
// Updated By: Jared
// 

using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net;
using System.Threading.Tasks;
using System.Windows.Forms;
using libWiiSharp;
using LiteDB;
using MapleLib.Collections;
using MapleLib.Common;
using MapleLib.Interfaces;
using MapleLib.Network;
using MapleLib.Properties;
using MapleLib.Structs;
using MapleLib.WiiU;
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
            if (Database.Time2Update(Settings.LastTitleDBUpdate) || Count < 1) {
                TextLog.Write("[Title Database] Building database...");

                LiteDatabase.DropCollection(CollectionName);

                var db = await Create();
                for (var i = 0; i < db.Count; i++) {
                    var item = db[i];

                    if (Col.Find(x => x.ID == item.ID).Any())
                        continue;

                    Col.Insert(item);
                    Col.EnsureIndex(x => x.Name);
                }

                Settings.LastTitleDBUpdate = DateTime.Now;
            }

            LoadLibrary(Settings.LibraryDirectory);

            TextLog.Write($"[Title Database] Loaded {Count} entries");
        }

        /// <inheritdoc />
        public MapleList<Title> Find(string id)
        {
            var col = LiteDatabase.GetCollection<Title>(CollectionName);

            var title = col.Find(x => x.ID.StartsWith(id));

            return new MapleList<Title>(title);
        }

        #endregion

        public event EventHandler<ProgressReport> ProgressReport;

        private Title SearchById(string id)
        {
            return Find(id)?.First();
        }

        private async Task<MapleList<Title>> Create()
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
                    var title = db.ToList().Find(t => t.ID.Contains(titleId.Substring(8)));

                    if (title != null)
                        title.Versions = versions;
                }

                foreach (var title in db.Where(x => x.ContentType == "eShop/Application")) {
                    var id = $"0005000C{title.Lower8Digits().ToUpper()}";

                    JObject _title;
                    if ((_title = WiiUTitleKey(titlekeys, id)) != null)
                        title.HasDLC = _title.HasValues;
                }
            });

            return db;
        }

        private void CleanUp(string outputDir, TMD tmd)
        {
            try {
                if (!Settings.StoreEncryptedContent) {
                    Toolbelt.AppendLog("  - Deleting Encrypted Contents...");
                    foreach (var t in tmd.Contents) {
                        if (!File.Exists(Path.Combine(outputDir, t.ContentID.ToString("x8")))) continue;
                        File.Delete(Path.Combine(outputDir, t.ContentID.ToString("x8")));
                    }
                }

                Toolbelt.AppendLog("  - Deleting CDecrypt, libeay32, and msvcr120d...");
                File.Delete(Path.Combine(outputDir, "CDecrypt.exe"));
                File.Delete(Path.Combine(outputDir, "libeay32.dll"));
                File.Delete(Path.Combine(outputDir, "msvcr120d.dll"));
            }
            catch {
                // ignored
            }
        }

        private static async Task<byte[]> DownloadData(string url)
        {
            byte[] data = {};

            try {
                data = await Web.DownloadDataAsync(url);
            }
            catch (WebException e) {
                TextLog.MesgLog.AddHistory($"{e.Message}\n{e.StackTrace}");
            }

            return data;
        }

        private async Task<TMD> DownloadTmd(string url, string saveTo)
        {
            var data = await DownloadData(url);
            if (data.Length <= 0) return null;
            var tmd = TMD.Load(data);

            if (tmd.TitleVersion > 999) return null;
            Toolbelt.AppendLog("  - Parsing TMD...");
            Toolbelt.AppendLog($"    + Title Version: {tmd.TitleVersion}");
            Toolbelt.AppendLog($"    + {tmd.NumOfContents} Contents");

            File.WriteAllBytes(saveTo, data);
            return tmd;
        }

        private async Task<TMD> LoadTmd(string id, string key, string outputDir, string titleUrl, string version)
        {
            var tmdFile = Path.Combine(outputDir, "tmd");

            if (string.IsNullOrEmpty(id) || string.IsNullOrEmpty(key))
                return null;

            version = int.Parse(version) == 0 ? "" : $".{version}";
            if (await DownloadTmd(titleUrl + $"tmd{version}", tmdFile) == null) {
                var url = $"http://192.99.69.253/?key={key.ToLower()}&title={id.ToLower()}&type=tmd";
                await DownloadTmd(url, tmdFile);
            }

            var file = new FileInfo(tmdFile);
            if (!file.Exists || file.Length <= 0)
                return null;

            return TMD.Load(tmdFile);
        }

        public async Task DownloadTitle(string id, string outputDir, string contentType, string version)
        {
            #region Setup

            var workingId = id.ToUpper();

            if (contentType == "Patch") {
                workingId = $"0005000E{workingId.Substring(8)}";

                if (Settings.Cemu173Patch)
                    outputDir = Path.Combine(Settings.BasePatchDir, workingId.Substring(8));
            }

            if (contentType == "DLC") {
                workingId = $"0005000C{workingId.Substring(8)}";

                if (Settings.Cemu173Patch)
                    outputDir = Path.Combine(Settings.BasePatchDir, workingId.Substring(8), "aoc");
            }

            Title title;
            if ((title = SearchById(workingId)) == null)
                throw new Exception("Could not locate the title key");

            var key = title.Key;
            var name = title.Name;
            if (string.IsNullOrEmpty(key) || string.IsNullOrEmpty(name)) return;

            if (!Directory.Exists(outputDir))
                Directory.CreateDirectory(outputDir);

            var str = $"Download {contentType} content to the following location?\n\"{outputDir}\"";
            var result = MessageBox.Show(str, name, MessageBoxButtons.YesNo);

            if (result != DialogResult.Yes)
                return;

            Toolbelt.AppendLog($"Output Directory '{outputDir}'");

            #endregion

            #region TMD

            Toolbelt.AppendLog("  - Loading TMD...");
            TMD tmd = null;

            var nusUrls = new List<string>
            {
                "http://ccs.cdn.wup.shop.nintendo.net/ccs/download/",
                "http://nus.cdn.shop.wii.com/ccs/download/",
                "http://ccs.cdn.c.shop.nintendowifi.net/ccs/download/"
            };

            foreach (var nusUrl in nusUrls) {
                string titleUrl = $"{nusUrl}{workingId}/";
                tmd = await LoadTmd(id, key, outputDir, titleUrl, version);

                if (tmd != null)
                    break;
            }

            if (tmd == null) {
                TextLog.MesgLog.WriteError("Could not locate TMD. Is this content request valid?");
                return;
            }

            #endregion

            #region Ticket

            Toolbelt.AppendLog("Generating Ticket...");

            var tikData = MapleTicket.Create(SearchById(id));
            if (tikData == null)
                throw new Exception("Invalid ticket data. Verify Title ID.");

            var ticket = Ticket.Load(tikData);
            ticket.Save(Path.Combine(outputDir, "cetk"));

            #endregion

            #region Content

            Toolbelt.AppendLog($"[+] [{contentType}] {name} v{tmd.TitleVersion}");
            Toolbelt.SetStatus($"Output Directory: {outputDir}");

            foreach (var nusUrl in nusUrls) {
                var url = nusUrl + workingId;
                if (await DownloadContent(tmd, outputDir, url) != 1)
                    continue;

                Toolbelt.AppendLog(string.Empty);
                Toolbelt.AppendLog("  - Decrypting Content");
                Toolbelt.AppendLog("  + This may take a minute. Please wait...");
                Toolbelt.SetStatus("Decrypting Content. This may take a minute. Please wait...", Color.OrangeRed);

                if (await Toolbelt.CDecrypt(outputDir) != 0) {
                    CleanUp(outputDir, tmd);
                    Toolbelt.AppendLog($"Error while decrypting {name}");
                    return;
                }

                CleanUp(outputDir, tmd);
                break;
            }

            #endregion

            Web.ResetDownloadProgressChanged();
            Toolbelt.AppendLog($"[+] [{contentType}] {name} v{tmd.TitleVersion} Finished.");
            Toolbelt.SetStatus($"[+] [{contentType}] {name} v{tmd.TitleVersion} Finished.");
        }

        private async Task<int> DownloadContent(TMD tmd, string outputDir, string titleUrl)
        {
            var result = 0;
            for (var i = 0; i < tmd.NumOfContents; i++) {
                var i1 = i;
                result = await Task.Run(async () => {
                    var numc = tmd.NumOfContents;
                    var size = Toolbelt.SizeSuffix((long) tmd.Contents[i1].Size);
                    Toolbelt.AppendLog($"Downloading Content #{i1 + 1} of {numc}... ({size})");
                    var contentPath = Path.Combine(outputDir, tmd.Contents[i1].ContentID.ToString("x8"));

                    if (!Toolbelt.IsValid(tmd.Contents[i1], contentPath))
                        try {
                            var downloadUrl = $"{titleUrl}/{tmd.Contents[i1].ContentID:x8}";
                            await Web.DownloadFileAsync(downloadUrl, contentPath);
                        }
                        catch (Exception ex) {
                            Toolbelt.AppendLog($"Downloading Content #{i1 + 1} of {numc} failed...\n{ex.Message}");
                            return 0;
                        }
                    ReportProgress(0, tmd.NumOfContents - 1, i1);
                    return 1;
                });
                if (result == 0)
                    break;
            }
            ReportProgress(0, 100, 0);
            return result;
        }

        private void ReportProgress(int min, int max, int value)
        {
            ProgressReport?.Invoke(null, new ProgressReport {Min = min, Max = max, Value = value});
        }

        private void LoadLibrary(string titleDirectory)
        {
            if (Count < 1)
                throw new DirectoryNotFoundException("Title Database is empty. This should not happen!!!");

            if (string.IsNullOrEmpty(titleDirectory))
                throw new DirectoryNotFoundException($"TitleDirectory: '{titleDirectory}' cannot be null or empty");

            if (titleDirectory.FilePathHasInvalidChars())
                throw new DirectoryNotFoundException($"TitleDirectory: '{titleDirectory}' is an invalid directory path");

            var xmlFiles = Directory.GetFiles(titleDirectory, "meta.xml", SearchOption.AllDirectories);

            foreach (var xmlFile in xmlFiles) {
                var rootDir = Path.GetFullPath(Path.Combine(xmlFile, "../../"));
                var titleID = Helper.XmlGetStringByTag(xmlFile, "title_id");

                Title title;
                if ((title = SearchById(titleID)) == null) {
                    TextLog.Write($"Could not find title using ID {titleID}");
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