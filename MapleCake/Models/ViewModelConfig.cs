// Created: 2017/03/27 11:20 AM
// Updated: 2017/10/02 11:42 AM
// 
// Project: MapleCake
// Filename: ViewModelConfig.cs
// Created By: Jared T

using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.IO.Packaging;
using System.Linq;
using System.Runtime.Serialization.Formatters.Binary;
using MapleCake.Models.Interfaces;
using MapleCake.ViewModels;
using MapleLib;
using MapleLib.Abstract;
using MapleLib.Collections;
using MapleLib.Structs;
using MapleLib.WiiU;

namespace MapleCake.Models
{
    public class ViewModelConfig : ViewModelBase
    {
        private readonly MainWindowViewModel _self;

        private List<ICommandItem> _contextItems;

        private Dictionary<string, GraphicPack> _graphicPackCache =
            new Dictionary<string, GraphicPack>();

        private Dictionary<string, BindingList<GraphicPack>> _graphicPackCollection =
            new Dictionary<string, BindingList<GraphicPack>>();

        private string _launchCemuText = "Loading Please wait...";

        private Title _selectedItem;

        private string _titleId;

        private string _titleVersion = "0";

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
            set {
                Settings.CemuDirectory = value;
                RaisePropertyChangedEvent("CemuDirectory");
            }
        }

        public string LaunchCemuText
        {
            get { return _launchCemuText; }
            set {
                _launchCemuText = value;
                RaisePropertyChangedEvent("LaunchCemuText");
            }
        }

        public bool FullScreen
        {
            get { return Settings.FullScreenMode; }
            set {
                Settings.FullScreenMode = value;
                RaisePropertyChangedEvent("FullScreen");
            }
        }

        public bool GraphicPacksEnabled
        {
            get { return Settings.GraphicPacksEnabled; }
            set {
                Settings.GraphicPacksEnabled = value;
                RaisePropertyChangedEvent("GraphicPacksEnabled");
            }
        }

        public bool StoreEncryptedContent
        {
            get { return Settings.StoreEncryptedContent; }
            set {
                Settings.StoreEncryptedContent = value;
                RaisePropertyChangedEvent("DynamicTheme");
            }
        }

        public bool CacheDatabase
        {
            get { return Settings.CacheDatabase; }
            set {
                Settings.CacheDatabase = value;
                RaisePropertyChangedEvent("CacheDatabase");
            }
        }

        public bool DynamicTheme
        {
            get { return Settings.DynamicTheme; }
            set {
                _self.DynamicTheme(Settings.DynamicTheme = value);
                RaisePropertyChangedEvent("DynamicTheme");
            }
        }

        public string TitleVersion
        {
            get { return _titleVersion; }
            set {
                _titleVersion = value;
                RaisePropertyChangedEvent("TitleVersion");
            }
        }

        public string TitleID
        {
            get { return _titleId; }
            set { _self.titleIdTextChanged(_titleId = value); }
        }

        public Title SelectedItem
        {
            get { return _selectedItem; }
            set {
                _self.SetBackgroundImg(_selectedItem = value);
                ContextItems = MapleContext.CreateMenu();
                RaisePropertyChangedEvent("ContextItems");
                RaisePropertyChangedEvent("SelectedItem");
                RaisePropertyChangedEvent("SelectedItemGraphicPack");
                RaisePropertyChangedEvent("SelectedItemGraphicPacks");
            }
        }

        public GraphicPack SelectedItemGraphicPack
        {
            get {
                if (string.IsNullOrEmpty(SelectedItem?.ID))
                    return null;

                if (_graphicPackCache.ContainsKey(SelectedItem.ID))
                    return _graphicPackCache[SelectedItem.ID];

                return (_graphicPackCollection[SelectedItem.ID] = SelectedItem?.GetGraphicPacks()).First();
            }
            set {
                if (SelectedItem == null || value == null) return;

                SelectedItem.SelectedGraphicPack = value;
                _graphicPackCache[SelectedItem.ID] = value;
                RaisePropertyChangedEvent("SelectedItemGraphicPack");
            }
        }

        public BindingList<GraphicPack> SelectedItemGraphicPacks
        {
            get {
                var col = new BindingList<GraphicPack>();

                if (!string.IsNullOrEmpty(SelectedItem?.ID))
                    col = _graphicPackCollection[SelectedItem.ID];

                return col;
            }
            set {
                if (value == null || !value.Any())
                    return;

                _graphicPackCollection[value.Last().TitleIds.First()] = value;
                RaisePropertyChangedEvent("SelectedItemGraphicPacks");
            }
        }

        public MapleDictionary TitleList => Database.GetLibrary();

        public List<ICommandItem> ContextItems
        {
            get {
                if (SelectedItem == null || _contextItems == null)
                    return _contextItems = MapleContext.CreateMenu();

                if (_contextItems[0].Text.Contains(SelectedItem.Name))
                    return _contextItems;

                return _contextItems = MapleContext.CreateMenu();
            }
            set {
                _contextItems = value;
                RaisePropertyChangedEvent("SelectedItem");
                RaisePropertyChangedEvent("ContextItems");
            }
        }

        public void SaveState()
        {
            var stateFile = Path.Combine(Settings.ConfigDirectory, "state");

            using (var stream = File.Open(stateFile, FileMode.Create))
            {
                var bformatter = new BinaryFormatter();

                bformatter.Serialize(stream, new List<object> {_graphicPackCache, _graphicPackCollection});
            }
        }

        public void LoadState()
        {
            var stateFile = Path.Combine(Settings.ConfigDirectory, "state");

            if (Settings.CacheDatabase)
                File.Delete(stateFile);

            if (File.Exists("cstate"))
                File.Move("cstate", stateFile);

            if (!File.Exists(stateFile))
                return;

            try
            {
                List<object> state;

                using (var stream = File.Open(stateFile, FileMode.Open))
                {
                    var bformatter = new BinaryFormatter();

                    state = bformatter.Deserialize(stream) as List<object>;
                }

                if (MainWindowViewModel.Instance == null || state == null || state.Count != 2) return;

                _graphicPackCache = state[0] as Dictionary<string, GraphicPack>;
                RaisePropertyChangedEvent("SelectedItemGraphicPack");

                _graphicPackCollection = state[1] as Dictionary<string, BindingList<GraphicPack>>;
                RaisePropertyChangedEvent("SelectedItemGraphicPacks");
            }
            catch
            {
                // ignored
            }
        }
    }
}