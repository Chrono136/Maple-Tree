// Project: MapleUI
// File: MainWindowViewModel.cs
// Updated By: Jared
// 

using System;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Threading;
using MapleCake.Models;
using MapleLib;
using MapleLib.Common;
using MapleLib.Enums;
using MapleLib.Structs;

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
#if DEBUG
            SetTitle($"MapleSeed {Settings.Version} - DEBUG VERSION");
#else
            SetTitle($"MapleSeed {Settings.Version}");
#endif

            SetDefaults();

            RegisterEvents();
        }

        private void SetTitle(string title)
        {
            Config.Name = title;
            Config.RaisePropertyChangedEvent("Name");
        }

        private void SetDefaults()
        {
            Config.LaunchCemuText = "Loading...";
            Config.LogBox = string.Empty;
            Config.ProgressMin = 0;
            Config.ProgressValue = 0;
            Config.ProgressMax = 100;
        }

        private void RegisterEvents()
        {
            TextLog.MesgLog.NewLogEntryEventHandler += MesgLogOnNewLogEntryEventHandler;
            TextLog.StatusLog.NewLogEntryEventHandler += StatusLogOnNewLogEntryEventHandler;

            Database.RegisterEvent(Database_ProgressReport);
            Database.DatabaseLoaded += Database_DatabasesLoaded;
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

        private async void OnLoadComplete(object sender, EventArgs e)
        {
            (sender as DispatcherTimer)?.Stop();

            Config.LaunchCemuText = "Launch Cemu";

            await Task.Delay(250);

            if (Config.TitleList.Any())
                Config.SelectedItem = Config.TitleList.First();

            TextLog.MesgLog.WriteLog($"Game Directory [{Settings.LibraryDirectory}]");

            new Update(UpdateType.MapleSeed2).CheckForUpdate();
        }

        private void Database_DatabasesLoaded(object sender, EventArgs e)
        {
            Config.CacheDatabase = false;

            new DispatcherTimer(TimeSpan.Zero, DispatcherPriority.ApplicationIdle, OnLoadComplete,
                Application.Current.Dispatcher);
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