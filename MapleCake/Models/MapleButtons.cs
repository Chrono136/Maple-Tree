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
    [Serializable]
    public class MapleButtons
    {
        private static Title SelectedItem => MainWindowViewModel.Instance.Config.SelectedItem;
        private static string TitleID => MainWindowViewModel.Instance.Config.TitleID;

        public ICommand Uninstall => new CommandHandler(UninstallButton);
        public ICommand LaunchCemu => new CommandHandler(LaunchCemuButton);
        public ICommand Download => new CommandHandler(DownloadButton);
        public ICommand AddUpdate => new CommandHandler(AddUpdateButton);
        public ICommand RemoveUpdate => new CommandHandler(RemoveUpdateButton);
        public ICommand AddDLC => new CommandHandler(AddDLCButton);
        public ICommand RemoveDLC => new CommandHandler(RemoveDLCButton);
        public ICommand RemoveTitle => new CommandHandler(RemoveTitleButton);
        public ICommand TitleIdToClipboard => new CommandHandler(TitleIdToClipboardButton);

        private void UninstallButton()
        {
            Helper.Uninstall();
        }

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

            int result;
            var vers = MainWindowViewModel.Instance.Config.TitleVersion;
            var version = int.TryParse(vers, out result) ? result.ToString() : "0";

            await title.DownloadContent(version);

            MainWindowViewModel.Instance.Config.TitleList.Add(title);

            MainWindowViewModel.Instance.Config.DownloadCommandEnabled = true;
            RaisePropertyChangedEvent("DownloadCommandEnabled");
        }

        private async void AddUpdateButton()
        {
            int result;
            var vers = MainWindowViewModel.Instance.Config.TitleVersion;
            var version = int.TryParse(vers, out result) ? result.ToString() : "0";
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
            if (SelectedItem != null && SelectedItem.DeleteContent())
                MainWindowViewModel.Instance.Config.TitleList.Remove(SelectedItem);
        }

        private static void TitleIdToClipboardButton()
        {
            Clipboard.SetText(SelectedItem.ID);
        }

        private static void RaisePropertyChangedEvent(string propertyName)
        {
            MainWindowViewModel.Instance.RaisePropertyChangedEvent(propertyName);
        }

        private static async Task DownloadContentClick(string contentType, string version = "0")
        {
            if (SelectedItem == null)
                throw new NullReferenceException("Title is null or empty, can't proceed!");

            if (SelectedItem.ID.IsNullOrEmpty())
                throw new NullReferenceException("Title ID is null or empty, can't proceed!");

            if (SelectedItem.Name.IsNullOrEmpty())
                throw new NullReferenceException($"[{SelectedItem}] Title Name is null or empty, can't proceed!");

            if (contentType == "DLC" && SelectedItem.HasDLC) {
                var id = $"0005000C{SelectedItem.Lower8Digits()}";

                var title = Database.SearchById(id);
                if (title == null)
                    throw new NullReferenceException($"Could not locate content for title ID {id}");

                await title.DownloadDLC();
            }

            if (contentType == "Patch") {
                if (!SelectedItem.Versions.Any()) {
                    MessageBox.Show($@"Update for '{SelectedItem.Name}' is not available");
                    return;
                }

                var id = $"0005000E{SelectedItem.Lower8Digits()}";

                var title = Database.SearchById(id);
                if (title == null)
                    throw new NullReferenceException($"Could not locate content for title ID {id}");

                await title.DownloadUpdate(version);
            }

            if (contentType == "eShop/Application") {
                await SelectedItem.DownloadContent(version);
            }
        }
    }
}