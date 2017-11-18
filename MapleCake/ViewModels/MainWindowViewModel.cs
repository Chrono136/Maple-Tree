// Created: 2017/03/27 11:20 AM
// Updated: 2017/10/06 2:08 PM
// 
// Project: MapleCake
// Filename: MainWindowViewModel.cs
// Created By: Jared T

using System;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Threading;
using MapleCake.Models;
using MapleLib;
using MapleLib.Common;
using MapleLib.Common.XInput;
using MapleLib.Enums;
using MapleLib.Network;
using MapleLib.Structs;
using MapleLib.UserInterface;

namespace MapleCake.ViewModels
{
    public class MainWindowViewModel : ViewModelBase
    {
        public MainWindowViewModel()
        {
            Helper.CheckSession();

            if (Config == null)
                Config = new ViewModelConfig(this);

            if (Instance == null)
                Instance = this;

            Init();
        }

        private static XInputController XInputController { get; set; }

        public static MainWindowViewModel Instance { get; private set; }

        public MapleButtons Click { get; set; } = new MapleButtons();

        public ViewModelConfig Config { get; set; }

        private void Init()
        {
#if DEBUG
            SetTitle($"MapleSeed {Settings.Version} - DEBUG VERSION");
            SetTitle($"MapleSeed {Settings.Version}");
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

            WiiuClient.ProgressReport += Database_ProgressReport;
            Database.DatabaseLoaded += Database_DatabasesLoaded;
        }

        public async Task SetBackgroundImg(Title title)
        {
            if (title == null || !Config.DynamicTheme || string.IsNullOrEmpty(title.ID))
                return;

            //Config.BackgroundImage = Resources.CEMU;

            if (title.ImageLocation == null)
                await title.GetImage();

            Config.BackgroundImage = title.ImageLocation;
        }

        public async void TitleIdTextChanged(string tid)
        {
            if (tid.Length != 16)
                return;

            var title = await Database.FindTitleAsync(tid);
            if (title == null) return;

            Config.SelectedItem = title;
            RaisePropertyChangedEvent("TitleID");

            if (string.IsNullOrEmpty(title.ID) ||
                string.IsNullOrEmpty(title.Region) ||
                string.IsNullOrEmpty(title.Name))
                return;

            TextLog.MesgLog.WriteLog($"[{title.Lower8Digits()}][{title.Region}] {title.Name}");
        }

        public async void DynamicTheme(bool enabled)
        {
            if (enabled)
                await SetBackgroundImg(Config.SelectedItem);
            else
                Config.BackgroundImage = null;
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

            XInputController = new XInputController();
            XInputController.Start(Config);
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