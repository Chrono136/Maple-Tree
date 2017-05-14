// Project: MapleUI
// File: MainWindowViewModel.cs
// Updated By: Jared
// 

using System;
using System.Drawing;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Threading;
using MapleCake.Models;
using MapleLib;
using MapleLib.Common;
using MapleLib.Enums;
using MapleLib.Structs;
using MessageBox = System.Windows.Forms.MessageBox;

namespace MapleCake.ViewModels
{
    public class MainWindowViewModel : ViewModelBase
    {
        public MainWindowViewModel()
        {
            if (Config == null)
                Config = new ViewModelConfig(this);

            if (Instance == null)
                Instance = this;

            Init();
        }

        public static MainWindowViewModel Instance { get; private set; }

        public MapleButtons Click { get; set; } = new MapleButtons();

        public ViewModelConfig Config { get; set; }

        private void Init()
        {
            SetTitle($"MapleSeed {Settings.Version}");

            SetDefaults();

            RegisterEvents();

            new DispatcherTimer(TimeSpan.Zero, DispatcherPriority.ApplicationIdle, OnLoadComplete,
                Application.Current.Dispatcher);
        }

        private void SetTitle(string title)
        {
            Config.Name = title;
            Config.RaisePropertyChangedEvent("Name");
        }

        private void SetDefaults()
        {
            Config.LogBox = string.Empty;
            Config.ProgressMin = 0;
            Config.ProgressValue = 0;
            Config.ProgressMax = 100;
        }

        private void RegisterEvents()
        {
            TextLog.MesgLog.NewLogEntryEventHandler += MesgLogOnNewLogEntryEventHandler;
            TextLog.StatusLog.NewLogEntryEventHandler += StatusLogOnNewLogEntryEventHandler;

            //Web.DownloadProgressChangedEvent += WebClientOnDownloadProgressChangedEvent;

            Database.RegisterEvent(Database_ProgressReport);
            Database.DatabaseLoaded += Database_DatabasesLoaded;
        }

        private static void CheckUpdate()
        {
            var update = new Update(UpdateType.MapleSeed2);
            if (!update.IsAvailable) return;

            TextLog.MesgLog.WriteLog($"MapleSeed Latest Version: {update.CurrentVersion}", Color.Green);

            MessageBox.Show(@"Please visit https://github.com/Tsume/Maple-Tree/releases for the latest releases.",
                $@"Version Mis-Match - Latest: {update.LatestVersion}");
        }

        public async void SetBackgroundImg(Title title)
        {
            if (!Config.DynamicTheme)
                return;

            if (string.IsNullOrEmpty(title.ImageLocation))
                await Task.Run(() => title.Image());

            Config.BackgroundImage = title.ImageLocation;
            Config.RaisePropertyChangedEvent("BackgroundImage");
        }

        public void titleIdTextChanged(string tid)
        {
            if (tid.Length != 16)
                return;

            var title = Database.FindTitle(tid);
            if (title == null) return;

            Config.SelectedItem = title;
            RaisePropertyChangedEvent("TitleID");

            if (string.IsNullOrEmpty(title.ID) ||
                string.IsNullOrEmpty(title.Region) ||
                string.IsNullOrEmpty(title.Name))
                return;

            TextLog.MesgLog.WriteLog($"[{title.Lower8Digits()}][{title.Region}] {title.Name}");
        }

        public void DynamicTheme(bool enabled)
        {
            if (enabled) {
                SetBackgroundImg(Config.SelectedItem);
            }
            else {
                Config.BackgroundImage = string.Empty;
                Config.RaisePropertyChangedEvent("BackgroundImage");
            }
        }

        private void OnLoadComplete(object sender, EventArgs e)
        {
            (sender as DispatcherTimer)?.Stop();

            CheckUpdate();

            if (Config.TitleList.Any())
                Config.SelectedItem = Config.TitleList.First();

            Config.LaunchCemuText = "Launch Cemu";
            TextLog.MesgLog.WriteLog($"Game Directory [{Settings.LibraryDirectory}]");
        }

        private void Database_DatabasesLoaded(object sender, EventArgs e)
        {
            Config.CacheDatabase = false;
        }

        private void Database_ProgressReport(object sender, ProgressReport e)
        {
            Config.ProgressMin = e.Min;
            Config.ProgressMax = e.Max;
            Config.ProgressValue = e.Value;
            Config.RaisePropertyChangedEvent("ProgressMin");
            Config.RaisePropertyChangedEvent("ProgressMax");
            Config.RaisePropertyChangedEvent("ProgressValue");
        }

        private void StatusLogOnNewLogEntryEventHandler(object sender, NewLogEntryEvent newLogEntryEvent)
        {
            Config.Status = newLogEntryEvent.Entry;
            Config.RaisePropertyChangedEvent("Status");
        }

        private void MesgLogOnNewLogEntryEventHandler(object sender, NewLogEntryEvent newLogEntryEvent)
        {
            Config.LogBox += Config.Status = newLogEntryEvent.Entry;
            Config.RaisePropertyChangedEvent("LogBox");
            Config.RaisePropertyChangedEvent("Status");
        }
    }
}