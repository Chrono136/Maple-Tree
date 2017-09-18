// Project: MapleCake
// File: MapleButtons.cs
// Updated By: Jared
// 

using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Input;
using MapleCake.ViewModels;
using MapleLib;
using MapleLib.Abstract;
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
        public ICommand EditorUI => new CommandHandler(MapleLib.Databases.Managers.DatabaseEditor.Instance.ToggleUI);
        
        private void UninstallButton()
        {
            MainWindowViewModel.Instance.Config.BackgroundImage = string.Empty;
            MainWindowViewModel.Instance.Config.RaisePropertyChangedEvent("BackgroundImage");

            Helper.Uninstall();
        }

        private static void LaunchCemuButton()
        {
            new Thread(() => {
                if (SelectedItem == null) {
                    Toolbelt.LaunchCemu(string.Empty, null);
                    return;
                }
                SelectedItem.PlayTitle();
            }).Start();
        }

        private static async void DownloadButton()
        {
            if (string.IsNullOrEmpty(TitleID))
                return;

            var title = Database.FindTitle(TitleID);
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

        private static async void AddUpdateButton()
        {
            int version = 0;
            if (int.TryParse(MainWindowViewModel.Instance.Config.TitleVersion, out version))
                await DownloadContentClick("Patch", version);
        }

        private static async void RemoveUpdateButton()
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

        private static async void AddDLCButton()
        {
            await DownloadContentClick("DLC");
        }

        private static void RemoveDLCButton()
        {
            if (SelectedItem == null) return;

            var updatePath = Path.Combine(Settings.BasePatchDir, SelectedItem.Lower8Digits());

            var result = MessageBox.Show(string.Format(Resources.ActionWillDeleteAllContent, updatePath),
                Resources.PleaseConfirmAction, MessageBoxButtons.OKCancel);

            if (result == DialogResult.OK)
                SelectedItem.DeleteAddOnContent();
        }

        private static void RemoveTitleButton()
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

        public static async Task DownloadContentClick(string contentType, int version = 0)
        {
            if (SelectedItem == null)
                throw new NullReferenceException("Title is null or empty, can't proceed!");

            if (SelectedItem.ID.IsNullOrEmpty())
                throw new NullReferenceException("Title ID is null or empty, can't proceed!");

            if (SelectedItem.Name.IsNullOrEmpty())
                throw new NullReferenceException($"[{SelectedItem}] Title Name is null or empty, can't proceed!");

            Title title;

            //download dlc if applicable
            if (contentType == "DLC" && SelectedItem.HasDLC)
                if ((title = Database.FindTitle($"0005000C{SelectedItem.Lower8Digits()}")) != null)
                    await title.DownloadDLC();

            //download patch if applicable
            if (contentType == "Patch" && SelectedItem.HasPatch)
                if ((title = Database.FindTitle($"0005000E{SelectedItem.Lower8Digits()}")) != null)
                    await title.DownloadUpdate(version.ToString());

            if (contentType == "eShop/Application")
                await SelectedItem.DownloadContent(version.ToString());
        }
    }
}