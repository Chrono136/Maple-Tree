// Project: MapleCake
// File: GraphicsPack.cs
// Updated By: Jared
// 

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using MapleLib.Collections;
using MapleLib.Common;

namespace MapleLib.WiiU
{
    [Serializable]
    public class GraphicPack
    {
        private string GPDirectory => Path.Combine(Settings.CemuDirectory, "graphicPacks", Toolbelt.RIC(Name));

        public MapleList<GraphicPackSource> Sources { get; } = new MapleList<GraphicPackSource>();

        public List<string> TitleIds { get; } = new List<string>();

        public string TitleIdString => string.Join(", ", TitleIds.ToArray());

        public string Name { get; set; }

        public string Rules { private get; set; }

        public void Apply()
        {
            if (!Settings.GraphicPacksEnabled || Rules.IsNullOrEmpty())
                return;

            var path = Path.Combine(Settings.CemuDirectory, "graphicPacks");
            if (Directory.GetFileSystemEntries(path).Any()) {
                var moveTo = $"graphicPacks.{Path.GetRandomFileName()}";
                Directory.Move(path, Path.Combine(Settings.CemuDirectory, moveTo));
            }

            if (!string.IsNullOrEmpty(Name) && !Directory.Exists(GPDirectory))
                Directory.CreateDirectory(GPDirectory);

            File.WriteAllText(Path.Combine(GPDirectory, "rules.txt"), Rules);
            Sources.ToList().ForEach(x => File.WriteAllBytes(Path.Combine(GPDirectory, x.FileName), x.Data));
        }

        public void Remove()
        {
            if (Directory.Exists(GPDirectory) && !string.IsNullOrEmpty(Name))
                Directory.Delete(GPDirectory, true);
        }

        public override string ToString()
        {
            return Name.Replace("\"", "");
        }

        #region Nested type: GraphicPackSource

        [Serializable]
        public struct GraphicPackSource
        {
            public string FileName { get; set; }
            public byte[] Data { get; set; }
        }

        #endregion
    }
}