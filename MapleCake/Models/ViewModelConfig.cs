// Project: MapleCake
// File: ViewModelConfig.cs
// Updated By: Jared
// 

using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using MapleCake.Models.Interfaces;
using MapleCake.ViewModels;
using MapleLib;
using MapleLib.Collections;
using MapleLib.Structs;
using MapleLib.WiiU;

namespace MapleCake.Models
{
    public class ViewModelConfig : ViewModelBase
    {
        private readonly MainWindowViewModel _self;
        private string _launchCemuText = "Loading Please wait...";
        private Title _selectedItem;
        private string _titleId;

        public ViewModelConfig(ViewModelBase self)
        {
            _self = self as MainWindowViewModel;
        }

        public string Name { get; set; }

        public string Status { get; set; }

        public string BackgroundImage { get; set; }

        public string LogBox { get; set; }

        public int ProgressMin { get; set; }

        public int ProgressMax { get; set; }

        public int ProgressValue { get; set; }

        public bool DownloadCommandEnabled { get; set; } = true;

        public string LibraryDirectory
        {
            get { return Settings.LibraryDirectory; }
            set {
                Settings.LibraryDirectory = value;
                RaisePropertyChangedEvent("TitleDirectory");
            }
        }

        public string CemuDirectory
        {
            get { return Settings.CemuDirectory; }
            set
            {
                Settings.CemuDirectory = value;
                RaisePropertyChangedEvent("CemuDirectory");
            }
        }

        public string LaunchCemuText
        {
            get { return _launchCemuText; }
            set
            {
                _launchCemuText = value;
                RaisePropertyChangedEvent("LaunchCemuText");
            }
        }

        public bool FullScreen {
            get { return Settings.FullScreenMode; }
            set {
                Settings.FullScreenMode = value;
                RaisePropertyChangedEvent("FullScreen");
            }
        }

        public bool GraphicPacksEnabled {
            get { return Settings.GraphicPacksEnabled; }
            set {
                Settings.GraphicPacksEnabled = value;
                RaisePropertyChangedEvent("GraphicPacksEnabled");
            }
        }

        public bool StoreEncryptedContent {
            get { return Settings.StoreEncryptedContent; }
            set {
                Settings.StoreEncryptedContent = value;
                RaisePropertyChangedEvent("DynamicTheme");
            }
        }

        public bool CacheDatabase {
            get { return Settings.CacheDatabase; }
            set {
                Settings.CacheDatabase = value;
                RaisePropertyChangedEvent("CacheDatabase");
            }
        }

        public bool DynamicTheme {
            get { return Settings.DynamicTheme; }
            set {
                _self.DynamicTheme(Settings.DynamicTheme = value);
                RaisePropertyChangedEvent("DynamicTheme");
            }
        }

        public string TitleID {
            get { return _titleId; }
            set { _self.titleIdTextChanged(_titleId = value); }
        }

        public Title SelectedItem {
            get { return _selectedItem; }
            set {
                if (value == null) return;
                _self.SetBackgroundImg(_selectedItem = value);
                RaisePropertyChangedEvent("SelectedItem");
                RaisePropertyChangedEvent("ContextItems");
                RaisePropertyChangedEvent("SelectedItemGraphicPacks");
                SelectedItemGraphicPack = SelectedItemGraphicPacks.FirstOrDefault();
                RaisePropertyChangedEvent("SelectedItemGraphicPack");
            }
        }

        public GraphicPack SelectedItemGraphicPack { get; set; }

        public BindingList<GraphicPack> SelectedItemGraphicPacks => SelectedItem?.GetGraphicPacks();

        public MapleDictionary TitleList => Database.TitleDb;

        public List<ICommandItem> ContextItems => MapleContext.CreateMenu();
    }
}