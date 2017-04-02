// Project: MapleUI
// File: MainWindowViewModel.cs
// Updated By: Jared
// 

using System;
using System.Drawing;
using System.IO;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Threading;
using MapleCake.Models;
using MapleLib;
using MapleLib.Common;
using MapleLib.Structs;
using MapleLib.WiiU;
using Application = System.Windows.Application;

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

        public ViewModelConfig Config { get; }

        private void Init()
        {
            SetTitle($"MapleSeed {Settings.Version}");

            SetDefaults();

            InitSettings();

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

        private void InitSettings()
        {
            if (string.IsNullOrEmpty(Settings.CemuDirectory) ||
                !File.Exists(Path.Combine(Settings.CemuDirectory, "cemu.exe"))) {
                var ofd = new OpenFileDialog
                {
                    CheckFileExists = true,
                    Filter = @"Cemu Excutable |cemu.exe"
                };

                var result = ofd.ShowDialog();
                if (string.IsNullOrWhiteSpace(ofd.FileName) || result != DialogResult.OK) {
                    MessageBox.Show(@"Cemu Directory is required to launch titles.");
                    Settings.CemuDirectory = string.Empty;
                }

                Settings.CemuDirectory = Path.GetDirectoryName(ofd.FileName);
            }

            if (string.IsNullOrEmpty(Settings.TitleDirectory) || !Directory.Exists(Settings.TitleDirectory)) {
                var fbd = new FolderBrowserDialog
                {
                    Description = @"Cemu Title Directory" + Environment.NewLine + @"(Where you store games)"
                };

                var result = fbd.ShowDialog();
                if (string.IsNullOrWhiteSpace(fbd.SelectedPath) || result == DialogResult.Cancel) {
                    MessageBox.Show(@"Title Directory is required. Shutting down.");
                    System.Windows.Forms.Application.Exit();
                }

                Settings.TitleDirectory = fbd.SelectedPath;
            }
        }

        private void RegisterEvents()
        {
            TextLog.MesgLog.NewLogEntryEventHandler += MesgLogOnNewLogEntryEventHandler;
            TextLog.StatusLog.NewLogEntryEventHandler += StatusLogOnNewLogEntryEventHandler;
            //Web.DownloadProgressChangedEvent += WebClientOnDownloadProgressChangedEvent;
            Database.ProgressReport += Database_ProgressReport;
        }

        private static void CheckUpdate()
        {
            AutoUpdaterDotNET.AutoUpdater.Start("https://s3.amazonaws.com/mapletree/mapleseed.xml", "MapleSeed");
            TextLog.MesgLog.WriteLog($"Current Version: {Settings.Version}", Color.Green);
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

            var title = Database.SearchById(tid);
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

            CheckUpdate();

            await Task.Run(() => {
                GraphicPack.Init().Wait();

                Database.Load();

                Config.SelectedItem = Config.TitleList.Random();

                Config.LaunchCemuText = "Launch Cemu";
                TextLog.MesgLog.WriteLog($"Game Directory [{Settings.TitleDirectory}]");
            });
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