// Created: 2017/05/13 3:44 PM
// Updated: 2017/10/01 6:35 PM
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
                LiteDatabase = new LiteDatabase(Helper.FileOpenStream(dbFile));
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
            SettingsCollection.Update(Settings.Config.Index, Settings.Config);
            return value;
        }

        public static bool Time2Update(DateTime lastUpdate)
        {
            return (DateTime.Today - lastUpdate).TotalDays > 14 || Settings.CacheDatabase;
        }

        public static void AddTitle(Title title)
        {
            WiiuTitleDatabase.TitleLibrary.Add(title);
        }

        public static async Task<Title> FindTitle(string id)
        {
            return (await WiiuTitleDatabase.Find(id.ToUpperInvariant())).FirstOrDefault();
        }

        private static async Task<IEnumerable<Title>> GetTitles()
        {
            return await WiiuTitleDatabase.All();
        }

        public static void Dump(string dumpTo = null)
        {
            if (string.IsNullOrEmpty(dumpTo))
            {
                var curDir = Directory.GetCurrentDirectory();
                dumpTo = Path.Combine(curDir, "WiiU.json");
            }

            var titles = GetTitles();
            var json = JsonConvert.SerializeObject(titles);
            File.WriteAllText(dumpTo, json);
        }

        public static MapleList<GraphicPack> FindGraphicPacks(string id)
        {
            return GraphicPacks.Find(id.ToUpperInvariant());
        }

        public static MapleDictionary GetLibrary()
        {
            return WiiuTitleDatabase.TitleLibrary;
        }

        public static void Dispose()
        {
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