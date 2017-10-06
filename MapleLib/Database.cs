// Created: 2017/05/13 3:44 PM
// Updated: 2017/10/06 10:14 AM
// 
// Project: MapleLib
// Filename: Database.cs
// Created By: Jared T

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using LiteDB;
using MapleLib.BaseClasses;
using MapleLib.Collections;
using MapleLib.Common;
using MapleLib.Databases;
using MapleLib.Network;
using MapleLib.Properties;
using MapleLib.Structs;
using MapleLib.WiiU;
using Newtonsoft.Json;

namespace MapleLib
{
    public static class Database
    {
        public const string API_BASE_URL = "http://api.pixxy.in/";

        static Database()
        {
            var dbFile = Path.GetFullPath(Path.Combine(Settings.ConfigDirectory, "mapleseed.db"));

            if (LiteDatabase == null)
            {
                DbFileStream = Helper.FileOpenStream(dbFile);
                LiteDatabase = new LiteDatabase(DbFileStream);
                SettingsCollection = LiteDatabase.GetCollection<Config>("Settings");
            }

            if (GraphicPacks == null)
                GraphicPacks = new GraphicPackDatabase(LiteDatabase);

            WiiuTitleDatabase.Load();

            if (Downloader == null)
                Downloader = new Downloader();

            Task.Run(async () =>
            {
                while (DatabaseLoaded == null || DatabaseCount < MaxDatabaseCount)
                    await Task.Delay(250);

                DatabaseLoaded?.Invoke(new object[] {GraphicPacks}, EventArgs.Empty);
            });
        }

        private static Stream DbFileStream { get; }

        private static List<TitleKey> TitleKeys { get; set; }

        private static GraphicPackDatabase GraphicPacks { get; }

        private static LiteDatabase LiteDatabase { get; }

        private static LiteCollection<Config> SettingsCollection { get; }

        private static Downloader Downloader { get; }

        private static int MaxDatabaseCount => 2;

        public static int DatabaseCount { get; set; }

        public static event EventHandler<EventArgs> DatabaseLoaded;

        public static Config GetConfig()
        {
            if (SettingsCollection.Count() != 0)
                return SettingsCollection.FindAll().First();

            SettingsCollection.Insert(new Config().Index, new Config());
            SettingsCollection.EnsureIndex(x => x.Index);

            return SettingsCollection.FindAll().First();
        }

        public static string SaveConfig(string value = null)
        {
            try
            {
                SettingsCollection.Update(Settings.Config.Index, Settings.Config);
            }
            catch (Exception e)
            {
                TextLog.Write(e.Message);
            }
            return value;
        }

        public static void AddTitle(Title title)
        {
            WiiuTitleDatabase.TitleLibrary.Add(title);
        }

        public static Title FindTitle(string id)
        {
            var titles = GetLibraryList().Where(x => x.ID == id.ToUpper()).ToList();

            if (!titles.Any())
                titles = WiiuTitleDatabase.Find(id).ToList();

            return titles.FirstOrDefault();
        }

        public static async Task<Title> FindTitleAsync(string id)
        {
            return await Task.Run(() => FindTitle(id));
        }

        private static async Task<IEnumerable<Title>> GetTitles()
        {
            return await WiiuTitleDatabase.All();
        }

        private static async Task<List<TitleKey>> GetTitleKeysTask()
        {
            return await Task.Run(() => GetTitleKeys());
        }

        private static List<TitleKey> GetTitleKeys()
        {
            if (TitleKeys != null)
                return TitleKeys;

            try
            {
                var json = Web.DownloadString("http://wiiu.titlekeys.gq/json");
                TitleKeys = JsonConvert.DeserializeObject<List<TitleKey>>(json);
            }
            catch
            {
                TextLog.Write("Failure accessing http://wiiu.titlekeys.gq/json, falling back.");
                TitleKeys = JsonConvert.DeserializeObject<List<TitleKey>>(Resources.wiiutitlekeys);
            }

            return TitleKeys;
        }

        private static TitleKey FindTitleKey(string id)
        {
            if (TitleKeys == null)
                TitleKeys = GetTitleKeys();

            return TitleKeys.FirstOrDefault(
                x => string.Equals(x.titleID, id, StringComparison.CurrentCultureIgnoreCase));
        }

        public static async Task<TitleKey> FindTitleKeyTask(string id)
        {
            return await Task.Run(() => FindTitleKey(id));
        }

        public static MapleList<GraphicPack> FindGraphicPacks(string id)
        {
            return GraphicPacks.Find(id.ToUpperInvariant());
        }

        public static MapleDictionary GetLibrary()
        {
            return WiiuTitleDatabase.TitleLibrary;
        }

        private static IEnumerable<Title> GetLibraryList()
        {
            if (WiiuTitleDatabase.TitleLibrary.Count > 0)
                return new List<Title>(WiiuTitleDatabase.TitleLibrary);

            return new List<Title>();
        }

        public static void Dispose()
        {
            DbFileStream?.Dispose();
            LiteDatabase?.Dispose();
        }

        public static Task DownloadTitle(string titleId, string titleFolderLocation, string contentType, string version)
        {
            return Task.Run(() => Downloader.AddToQueue(titleId, titleFolderLocation, contentType, version));
        }

        public static void RegisterEvent(EventHandler<ProgressReport> onEvent)
        {
            WiiuClient.ProgressReport += onEvent;
        }

        public static void RegisterEvent(EventHandler<AddItemEventArgs<Title>> onEvent)
        {
            WiiuTitleDatabase.TitleLibrary.AddItemEvent += onEvent;
        }
    }
}