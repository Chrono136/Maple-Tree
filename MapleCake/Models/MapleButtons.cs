// Created: 2017/03/27 11:20 AM
// Updated: 2017/10/14 3:24 PM
// 
// Project: MapleCake
// Filename: MapleButtons.cs
// Created By: Jared T

using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Input;
using MapleCake.ViewModels;
using MapleLib;
using MapleLib.Common;
using MapleLib.Databases.Managers;
using MapleLib.Network;
using MapleLib.Properties;
using MapleLib.Structs;
using MapleLib.UserInterface;

namespace MapleCake.Models
{
    [Serializable]
    public class MapleButtons
    {
        private static Title SelectedItem => MainWindowViewModel.Instance.Config.SelectedItem;
        private static string TitleId => MainWindowViewModel.Instance.Config.TitleId;

        public ICommand Uninstall => new CommandHandler(UninstallButton);
        public ICommand LaunchCemu => new CommandHandler(LaunchCemuButton);
        public ICommand Download => new CommandHandler(DownloadButton);
        public ICommand AddUpdate => new CommandHandler(AddUpdateButton);
        public ICommand RemoveUpdate => new CommandHandler(RemoveUpdateButton);
        public ICommand AddDLC => new CommandHandler(AddDlcButton);
        public ICommand RemoveDLC => new CommandHandler(RemoveDlcButton);
        public ICommand RemoveTitle => new CommandHandler(RemoveTitleButton);
        public ICommand TitleIdToClipboard => new CommandHandler(TitleIdToClipboardButton);
        public ICommand EditorUI => new CommandHandler(WiiUManager.Instance.ToggleUi);

        private void UninstallButton()
        {
            Helper.Uninstall();
        }

        private static void LaunchCemuButton()
        {
            new Thread(() =>
            {
                if (SelectedItem == null)
                {
                    Toolbelt.LaunchCemu(string.Empty, null);
                    return;
                }
                SelectedItem.PlayTitle();
            }).Start();
        }

        private static async void DownloadButton()
        {
            if (string.IsNullOrEmpty(TitleId))
                return;

            var title = await Database.FindTitleAsync($"00050000{TitleId.Substring(8)}");
            title.ID = TitleId;

            MainWindowViewModel.Instance.Config.DownloadCommandEnabled = false;
            RaisePropertyChangedEvent("DownloadCommandEnabled");

            int result;
            var vers = MainWindowViewModel.Instance.Config.TitleVersion;
            var version = int.TryParse(vers, out result) ? result.ToString() : "0";

            switch (title.ContentType)
            {
                case "Patch":
                    await title.DownloadUpdateTask(version, true);
                    break;

                case "DLC":
                    await title.DownloadDLCTask(true);
                    break;

                default:
                    await title.DownloadContentTask(version);
                    break;
            }

            if (title.ContentType.Contains("App"))
                MainWindowViewModel.Instance.Config.TitleList.Add(title);

            MainWindowViewModel.Instance.Config.DownloadCommandEnabled = true;
            RaisePropertyChangedEvent("DownloadCommandEnabled");
        }

        private static async void AddUpdateButton()
        {
            int version;
            if (int.TryParse(MainWindowViewModel.Instance.Config.TitleVersion, out version))
                await DownloadContentClickTask("Patch", version);
        }

        private static async void AddDlcButton()
        {
            await DownloadContentClickTask("DLC");
        }

        private static void RemoveUpdateButton()
        {
            SelectedItem?.DeleteUpdateContent();
        }

        private static void RemoveDlcButton()
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

        public static async Task DownloadContentClickTask(string contentType, int version = 0)
        {
            await Task.Run(() => DownloadContentClick(contentType, version));
        }

        public static void DownloadContentClick(string contentType, int version = 0)
        {
            if (SelectedItem == null)
                throw new NullReferenceException("Title is null or empty, can't proceed!");

            if (SelectedItem.ID.IsNullOrEmpty())
                throw new NullReferenceException("Title ID is null or empty, can't proceed!");

            if (SelectedItem.Name.IsNullOrEmpty())
                throw new NullReferenceException($"[{SelectedItem}] Title Name is null or empty, can't proceed!");

            var title = Database.FindTitle(SelectedItem.ID);

            //download dlc if applicable
            if (contentType == "DLC" && SelectedItem.HasDLC)
                title?.DownloadDLC();

            //download patch if applicable
            if (contentType == "Patch" && SelectedItem.HasPatch)
                title?.DownloadUpdate(version.ToString());

            if (contentType == "eShop/Application")
                SelectedItem.DownloadContent(version.ToString());
        }
    }
}