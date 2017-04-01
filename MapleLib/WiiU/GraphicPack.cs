// Project: MapleCake
// File: GraphicsPack.cs
// Updated By: Jared
// 

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using IniParser;
using LibGit2Sharp;
using MapleLib.Collections;
using MapleLib.Common;
using Newtonsoft.Json;

namespace MapleLib.WiiU
{
    public class GraphicPack
    {
        static GraphicPack()
        {
            NewGraphicPack += GraphicPack_NewGraphicPack;
        }

        private static int RetryCount { get; set; }

        public static MapleList<GraphicPack> GraphicPacks { get; set; }

        private string GPDirectory => Path.Combine(Settings.CemuDirectory, "graphicPacks", Toolbelt.RIC(Name));

        private string SaveToLocation => Path.Combine(GPDirectory, "rules.txt");

        public string Name { get; set; }

        public string FileLocation { get; set; }

        public List<string> TitleIds { get; } = new List<string>();

        public static event EventHandler<GraphicPack> NewGraphicPack;

        public void Apply()
        {
            if (!File.Exists(FileLocation) || !Settings.GraphicPacksEnabled)
                return;

            var path = Path.Combine(Settings.CemuDirectory, "graphicPacks");
            if (Directory.GetFileSystemEntries(path).Any()) {
                var moveTo = $"graphicPacks.{Path.GetRandomFileName()}";
                Directory.Move(path, Path.Combine(Settings.CemuDirectory, moveTo));
            }

            if (!string.IsNullOrEmpty(Name) && !Directory.Exists(GPDirectory))
                Directory.CreateDirectory(GPDirectory);

            var contents = File.ReadAllText(FileLocation);
            File.WriteAllText(SaveToLocation, contents);
        }

        public void Remove()
        {
            if (Directory.Exists(GPDirectory) && !string.IsNullOrEmpty(Name))
                Directory.Delete(GPDirectory, true);
        }

        private static void GraphicPack_NewGraphicPack(object sender, GraphicPack pack)
        {
            if (!GraphicPacks.Contains(pack))
                GraphicPacks.Add(pack);
        }

        public static GraphicPack FindPack(string title_id)
        {
            return GraphicPacks.FirstOrDefault(x => x.TitleIds.ToList().Contains(title_id.ToUpper()));
        }

        public static async Task<MapleList<GraphicPack>> Init()
        {
            var gpFile = Path.Combine(Settings.ConfigDirectory, "graphicPacks.db");
            GraphicPacks = new MapleList<GraphicPack>();

            if (!File.Exists(gpFile) || !Settings.CacheDatabase) {
                TextLog.MesgLog.WriteLog(@"Building graphic pack database...");

                var path = Path.Combine(Settings.ConfigDirectory, "graphicPacks");
                if (Directory.Exists(path) && Repository.IsValid(path)) {
                    var logMessage = string.Empty;
                    using (var repo = new Repository(path)) {
                        foreach (var remote in repo.Network.Remotes) {
                            var refSpecs = remote.FetchRefSpecs.Select(x => x.Specification);
                            Commands.Fetch(repo, remote.Name, refSpecs, null, logMessage);
                        }
                    }
                }
                else {
                    Repository.Clone("https://github.com/slashiee/cemu_graphic_packs.git", path);
                }

                var files = Directory.GetFiles(path, "*.txt", SearchOption.AllDirectories).ToList();
                await Task.Run(() => Parallel.ForEach(files, Process));

                Save();
            }

            try {
                if (File.Exists(gpFile)) {
                    TextLog.MesgLog.WriteLog(@"Loading graphic pack database...");
                    var json = File.ReadAllText(gpFile);
                    GraphicPacks = JsonConvert.DeserializeObject<MapleList<GraphicPack>>(json);
                }
            }
            catch {
                if (RetryCount >= 3) {
                    TextLog.MesgLog.WriteLog(@"GraphicPacks Init() failed too many times, cancelling...");
                    return GraphicPacks;
                }

                RetryCount++;
                File.Delete(gpFile);
                await Init();
            }

            return GraphicPacks;
        }

        private static void Process(string file)
        {
            try {
                if (!File.Exists(file))
                    return;

                var parser = new FileIniDataParser();
                parser.Parser.Configuration.SkipInvalidLines = true;
                parser.Parser.Configuration.AllowDuplicateKeys = true;
                parser.Parser.Configuration.AllowDuplicateSections = true;
                var data = parser.ReadData(new StreamReader(file));

                if (data["Definition"]?["titleIds"] == null || data["Definition"]?["name"] == null)
                    return;

                var value = data["Definition"]?["titleIds"]?.ToUpper();
                if (string.IsNullOrEmpty(value)) return;

                var titleIds = value.Split(',');
                var name = data["Definition"]?["name"].Trim();

                if (name.Contains("Mario Kart 8")) {
                    Console.Write(0);
                }

                var pack = new GraphicPack {Name = name, FileLocation = file};
                pack.TitleIds.AddRange(titleIds);

                NewGraphicPack?.Invoke(null, pack);
            }
            catch (Exception e) {
                //MessageBox.Show($"{e.Message}\n{e.StackTrace}");
            }
        }

        private static void Save()
        {
            var saveTo = Path.Combine(Settings.ConfigDirectory, "graphicPacks.db");

            if (!Directory.Exists(Settings.ConfigDirectory))
                Directory.CreateDirectory(Settings.ConfigDirectory);

            var json = JsonConvert.SerializeObject(GraphicPacks);
            File.WriteAllText(saveTo, json);
        }

        /// <inheritdoc />
        public override string ToString()
        {
            return Name.Replace("\"", "");
        }
    }
}