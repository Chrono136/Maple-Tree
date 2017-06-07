// Project: MapleLib
// File: GraphicPackDatabase.cs
// Updated By: Jared
// 

using System;
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

namespace MapleLib
{
    public static class Database
    {
        static Database()
        {
            var dbFile = Path.GetFullPath(Path.Combine(Settings.ConfigDirectory, "mapleseed.db"));

            if (LiteDatabase == null) {
                LiteDatabase = new LiteDatabase(Helper.FileOpenStream(dbFile));
                SettingsCollection = LiteDatabase.GetCollection<Config>("Settings");
            }

            if (GraphicPacks == null)
                GraphicPacks = new GraphicPackDatabase(LiteDatabase);

            if (WiiuTitles == null)
                WiiuTitles = new WiiuTitleDatabase(LiteDatabase);

            Task.Run(async () => {
                while (DatabaseLoaded == null || DatabaseCount < MaxDatabaseCount)
                    await Task.Delay(250);

                DatabaseLoaded?.Invoke(new object[] {GraphicPacks, WiiuTitles}, EventArgs.Empty);
            });
        }

        private static GraphicPackDatabase GraphicPacks { get; }
        private static WiiuTitleDatabase WiiuTitles { get; }

        private static LiteDatabase LiteDatabase { get; }

        private static LiteCollection<Config> SettingsCollection { get; }

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
            WiiuTitles.TitleLibrary.Add(title);
        }

        public static Title FindTitle(string id)
        {
            return WiiuTitles.Find(id.ToUpperInvariant()).FirstOrDefault();
        }

        public static MapleList<GraphicPack> FindGraphicPacks(string id)
        {
            return GraphicPacks.Find(id.ToUpperInvariant());
        }

        public static MapleDictionary GetLibrary()
        {
            return WiiuTitles.TitleLibrary;
        }

        public static void Dispose()
        {
            LiteDatabase?.Dispose();
        }

        public static Task DownloadTitle(string titleID, string titleFolderLocation, string contentType, string version)
        {
            return WiiuClient.DownloadTitle(titleID, titleFolderLocation, contentType, version);
        }

        public static void RegisterEvent(EventHandler<ProgressReport> onEvent)
        {
            WiiuClient.ProgressReport += onEvent;
        }

        public static void RegisterEvent(EventHandler<AddItemEventArgs<Title>> onEvent)
        {
            WiiuTitles.TitleLibrary.AddItemEvent += onEvent;
        }
    }
}