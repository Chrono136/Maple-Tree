// Created: 2017/03/27 11:20 AM
// Updated: 2017/10/02 1:55 PM
// 
// Project: MapleCake
// Filename: MapleContext.cs
// Created By: Jared T

using System.Collections.Generic;
using System.IO;
using System.Linq;
using MapleCake.Models.ContextMenu;
using MapleCake.Models.Interfaces;
using MapleCake.ViewModels;
using MapleLib;
using MapleLib.Abstract;
using MapleLib.Structs;

namespace MapleCake.Models
{
    public static class MapleContext
    {
        private static MapleButtons Click => MainWindowViewModel.Instance.Click;

        private static Title SelectedItem => MainWindowViewModel.Instance?.Config?.SelectedItem;

        public static List<ICommandItem> CreateMenu()
        {
            if (SelectedItem == null)
                return null;

            var items = new List<ICommandItem>
            {
                new CommandItem
                {
                    Text = $"{SelectedItem.Name} v{SelectedItem.GetUpdateVersion()}",
                    ToolTip = "Copy title ID to clipboard",
                    Command = Click.TitleIdToClipboard
                }
            };

            CreateUpdateItems(items);

            if (items.Any(x => x.Text.Contains("Update")))
                items.Add(new SeparatorCommandItem());

            CreateDlcItems(items);

            if (items.Any(x => x.Text.Contains("DLC")))
                items.Add(new SeparatorCommandItem());

            items.Add(new CommandItem
            {
                Text = "Delete Title",
                ToolTip = "Removes the title from your disk",
                Command = Click.RemoveTitle
            });

            return items;
        }

        private static void CreateUpdateItems(ICollection<ICommandItem> items)
        {
            var title = Database.FindTitle($"00050000{SelectedItem.Lower8Digits()}");

            var titleVersion = int.Parse(MainWindowViewModel.Instance.Config.TitleVersion);

            if (SelectedItem.HasPatch || SelectedItem.HasDLC)
                items.Add(new SeparatorCommandItem());

            if (SelectedItem.HasPatch)
            {
                foreach (var version in title.Versions)
                    items.Add(new CommandItem
                    {
                        Text = $"[+] Update -> v{version}",
                        Command = new CommandHandler(() => { DownloadContent("Patch", version); })
                    });

                items.Add(new CommandItem
                {
                    Text = $"[+] Update -> v{titleVersion} (Custom)",
                    Command = new CommandHandler(() => { DownloadContent("Patch", titleVersion); })
                });
            }

            var dir = Path.Combine(Settings.BasePatchDir, SelectedItem.Lower8Digits());
            if (File.Exists(Path.Combine(dir, "meta", "meta.xml")))
                items.Add(new CommandItem
                {
                    Text = "[-] Remove Update",
                    ToolTip = "Remove Update",
                    Command = Click.RemoveUpdate
                });
        }

        private static void CreateDlcItems(ICollection<ICommandItem> items)
        {
            if (SelectedItem.HasDLC)
                items.Add(new CommandItem {Text = "[+] DLC", ToolTip = "Add DLC", Command = Click.AddDLC});

            var dir = Path.Combine(Settings.BasePatchDir, SelectedItem.Lower8Digits(), "aoc");
            var meta = Path.Combine(dir, "meta", "meta.xml");

            if (File.Exists(meta))
                items.Add(new CommandItem {Text = "[-] DLC", ToolTip = "Remove DLC", Command = Click.RemoveDLC});
        }

        private static async void DownloadContent(string type, int version)
        {
            await MapleButtons.DownloadContentClick(type, version);
        }
    }
}