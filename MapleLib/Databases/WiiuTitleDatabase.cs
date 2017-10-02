// Created: 2017/05/14 6:05 AM
// Updated: 2017/10/01 6:38 PM
// 
// Project: MapleLib
// Filename: WiiuTitleDatabase.cs
// Created By: Jared T

using System.Collections.Generic;
using System.IO;
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
        public static MapleDictionary TitleLibrary { get; } = new MapleDictionary(Settings.LibraryDirectory);
        
        private static async Task<IEnumerable<Title>> GetJObjects(string query)
        {
            var url = Database.API_BASE_URL + query;
            var json = await Web.DownloadStringAsync(url);

            return string.IsNullOrEmpty(json) ? null : JsonConvert.DeserializeObject<IEnumerable<Title>>(json);
        }

        public static async Task<IEnumerable<Title>> All()
        {
            return new MapleList<Title>(await GetJObjects("title/all"));
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

            var xmlFiles = Helper.GetFiles(titleDirectory, "meta.xml");

            foreach (var xmlFile in xmlFiles)
            {
                var rootDir = Path.GetFullPath(Path.Combine(xmlFile, "../../"));
                var titleId = Helper.XmlGetStringByTag(xmlFile, "title_id");

                Title title;
                if ((title = await Database.FindTitle(titleId)) == null)
                {
                    TextLog.Write($"Could not find title using ID {titleId}");
                    continue;
                }

                title.FolderLocation = rootDir;
                title.MetaLocation = xmlFile;
                TitleLibrary.AddOnUi(title);
            }
        }

        public static async void Load()
        {
            await LoadLibrary(Settings.LibraryDirectory);

            TextLog.Write($"[Title Database] Loaded {await GetCount()} entries");
            Database.DatabaseCount++;
        }

        public static async Task<MapleList<Title>> Find(string id)
        {
            var titles = await GetJObjects($"title/{id.ToUpperInvariant()}");

            return new MapleList<Title>(titles);
        }
    }
}