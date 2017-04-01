// Project: MapleCake
// File: MapleButtons.cs
// Updated By: Jared
// 

using System;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Input;
using MapleCake.ViewModels;
using MapleLib;
using MapleLib.Common;
using MapleLib.Properties;
using MapleLib.Structs;

namespace MapleCake.Models
{
    public class MapleButtons
    {
        private static Title SelectedItem => MainWindowViewModel.Instance.Config.SelectedItem;

        private static string TitleID => MainWindowViewModel.Instance.Config.TitleID;

        public ICommand LaunchCemu => new CommandHandler(LaunchCemuButton);
        public ICommand Download => new CommandHandler(DownloadButton);
        public ICommand AddUpdate => new CommandHandler(AddUpdateButton);
        public ICommand RemoveUpdate => new CommandHandler(RemoveUpdateButton);
        public ICommand AddDLC => new CommandHandler(AddDLCButton);
        public ICommand RemoveDLC => new CommandHandler(RemoveDLCButton);
        public ICommand RemoveTitle => new CommandHandler(RemoveTitleButton);
        public ICommand TitleIdToClipboard => new CommandHandler(TitleIdToClipboardButton);

        private void LaunchCemuButton()
        {
            if (SelectedItem == null) return;

            new Thread(() => {
                var pack = MainWindowViewModel.Instance.Config.SelectedItemGraphicPack;

                if (!Toolbelt.LaunchCemu(SelectedItem.MetaLocation, pack)) return;
                TextLog.MesgLog.WriteLog($"Now Playing: {SelectedItem.Name}");
            }).Start();
        }

        private async void DownloadButton()
        {
            if (string.IsNullOrEmpty(TitleID))
                return;

            var title = Database.SearchById(TitleID);
            if (title == null) return;

            MainWindowViewModel.Instance.Config.DownloadCommandEnabled = false;
            RaisePropertyChangedEvent("DownloadCommandEnabled");

            await title.DownloadContent();

            MainWindowViewModel.Instance.Config.TitleList.Add(title);

            MainWindowViewModel.Instance.Config.DownloadCommandEnabled = true;
            RaisePropertyChangedEvent("DownloadCommandEnabled");
        }

        private async void AddUpdateButton()
        {
            int ver;
            var version = int.TryParse("0", out ver) ? ver.ToString() : "0";
            await DownloadContentClick("Patch", version);
        }

        private async void RemoveUpdateButton()
        {
            if (SelectedItem == null) return;

            await Task.Run(() => {
                var updatePath = Path.Combine(Settings.BasePatchDir, SelectedItem.Lower8Digits());
                var result = MessageBox.Show(string.Format(Resources.ActionWillDeleteAllContent, updatePath),
                    Resources.PleaseConfirmAction, MessageBoxButtons.OKCancel);

                if (result != DialogResult.OK)
                    return;

                SelectedItem.DeleteUpdateContent();
            });
        }

        private async void AddDLCButton()
        {
            await DownloadContentClick("DLC");
        }

        private void RemoveDLCButton()
        {
            if (SelectedItem == null) return;

            var updatePath = Path.Combine(Settings.BasePatchDir, SelectedItem.Lower8Digits());

            var result = MessageBox.Show(string.Format(Resources.ActionWillDeleteAllContent, updatePath),
                Resources.PleaseConfirmAction, MessageBoxButtons.OKCancel);

            if (result == DialogResult.OK)
                SelectedItem.DeleteAddOnContent();
        }

        private void RemoveTitleButton()
        {
            SelectedItem?.DeleteContent();
            MainWindowViewModel.Instance.Config.TitleList.Remove(SelectedItem);
        }

        private static void TitleIdToClipboardButton()
        {
            Clipboard.SetText(SelectedItem.ID);
        }

        private static async Task DownloadContentClick(string contentType, string version = "0")
        {
            try {
                if (SelectedItem == null)
                    return;

                string id;

                switch (contentType) {
                    case "DLC":
                    {
                        if (SelectedItem.HasDLC) {
                            id = $"0005000C{SelectedItem.Lower8Digits()}";
                            var title = Database.SearchById(id);
                            await title.DownloadDLC();
                        }
                        break;
                    }

                    case "Patch":
                    {
                        if (!SelectedItem.Versions.Any()) {
                            MessageBox.Show($@"Update for {SelectedItem.Name} is not available");
                            break;
                        }

                        id = $"0005000E{SelectedItem.Lower8Digits()}";
                        var title = Database.SearchById(id);
                        await title.DownloadUpdate(version);

                        break;
                    }

                    case "eShop/Application":
                    {
                        await SelectedItem.DownloadContent(version);
                        break;
                    }
                }
            }
            catch (Exception ex) {
                MessageBox.Show(ex.Message + Environment.NewLine + ex.StackTrace);
            }
        }

        private static void RaisePropertyChangedEvent(string propertyName)
        {
            MainWindowViewModel.Instance.RaisePropertyChangedEvent(propertyName);
        }
    }
}