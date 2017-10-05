// Created: 2017/05/14 6:05 AM
// Updated: 2017/10/05 6:07 PM
// 
// Project: MapleLib
// Filename: WiiuTitleDatabase.cs
// Created By: Jared T

using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using MapleLib.Collections;
using MapleLib.Common;
using MapleLib.Network;
using MapleLib.Structs;
using Newtonsoft.Json;

namespace MapleLib.Databases
{
    public static class WiiuTitleDatabase
    {
        public static MapleDictionary TitleLibrary { get; private set; } = new MapleDictionary();

        private static IEnumerable<Title> GetJObjects(string query)
        {
            var url = Database.API_BASE_URL + query;
            var json = Web.DownloadString(url);

            var settings = new JsonSerializerSettings();

            var titles = JsonConvert.DeserializeObject<IList<Title>>(json, settings);

            return string.IsNullOrEmpty(json) ? null : titles;
        }

        private static async Task<IEnumerable<Title>> GetJObjectsTask(string query)
        {
            return await Task.Run(() => GetJObjects(query));
        }

        public static async Task<IEnumerable<Title>> All()
        {
            return new MapleList<Title>(await GetJObjectsTask("title/all"));
        }

        public static Versions GetVersions(string id)
        {
            var url = Database.API_BASE_URL + $"title/{id}/versions";
            var response = Web.DownloadString(url);

            return response;
        }

        public static async Task<Versions> GetVersionsTask(string id)
        {
            return await Task.Run(() => GetVersions(id));
        }

        private static async Task<int> GetCount()
        {
            var countStr = await Web.DownloadStringAsync(Database.API_BASE_URL + "title/count");
            return string.IsNullOrEmpty(countStr) ? 0 : int.Parse(countStr);
        }

        private static async Task LoadLibrary(string titleDirectory)
        {
            if (await GetCount() < 1)
                throw new DirectoryNotFoundException("Title Database is empty. This should not happen!!!");

            if (string.IsNullOrEmpty(titleDirectory))
                throw new DirectoryNotFoundException($"TitleDir: '{titleDirectory}' cannot be null or empty");

            if (titleDirectory.FilePathHasInvalidChars())
                throw new DirectoryNotFoundException($"TitleDir: '{titleDirectory}' is an invalid directory path");

            var xmlFiles = Helper.GetFiles(titleDirectory, "meta.xml").ToList();

            xmlFiles.ForEach(LoadLibraryForeach);

            while (TitleLibrary.Count < xmlFiles.Count)
                await Task.Delay(100);

            TitleLibrary = new MapleDictionary(TitleLibrary.OrderBy(x => x.Name).ToList());
        }

        private static async void LoadLibraryForeach(string xmlFile)
        {
            var rootDir = Path.GetFullPath(Path.Combine(xmlFile, "../../"));
            var titleId = Helper.XmlGetStringByTag(xmlFile, "title_id");

            Title title;
            if ((title = await Database.FindTitleAsync(titleId)) == null)
            {
                TextLog.Write($"Could not find title using ID {titleId}");
                return;
            }

            title.FolderLocation = rootDir;
            title.MetaLocation = xmlFile;
            TitleLibrary.AddOnUi(title);
        }

        public static async void Load()
        {
            await LoadLibrary(Settings.LibraryDirectory);

            TextLog.Write($"[Title Database] Loaded {await GetCount()} entries");
            Database.DatabaseCount++;
        }

        public static async Task<MapleList<Title>> FindAsync(string id)
        {
            return await Task.Run(() => Find(id));
        }

        public static MapleList<Title> Find(string id)
        {
            var titles = GetJObjects($"title/{id.ToUpperInvariant()}");

            return new MapleList<Title>(titles);
        }
    }
}