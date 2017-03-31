// Project: MapleCake
// File: ViewModelConfig.cs
// Updated By: Jared
// 

using System.Collections.Generic;
using MapleCake.Models.Interfaces;
using MapleCake.ViewModels;
using MapleLib;
using MapleLib.Collections;
using MapleLib.Structs;

namespace MapleCake.Models
{
    public class ViewModelConfig : ViewModelBase
    {
        private readonly MainWindowViewModel _self;

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

        public string SelectedItemText => SelectedItem != null ? $"Download '{SelectedItem.Name}'" : "Download";

        public int ProgressValue { get; set; }

        public bool DownloadCommandEnabled { get; set; } = true;

        public bool FullScreen {
            get { return Settings.FullScreenMode; }
            set {
                Settings.FullScreenMode = value;
                RaisePropertyChangedEvent("FullScreen");
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
                _self.WriteVersions(value);
                _self.SetBackgroundImg(_selectedItem = value);
                RaisePropertyChangedEvent("SelectedItem");
                RaisePropertyChangedEvent("ContextItems");
                RaisePropertyChangedEvent("SelectedItemText");
            }
        }

        public MapleDictionary TitleList => Database.TitleDb;

        public List<ICommandItem> ContextItems => MapleContext.CreateMenu();
    }
}