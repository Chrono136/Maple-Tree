// Created: 2017/04/01 12:46 PM
// Updated: 2017/11/11 1:29 PM
// 
// Project: MapleLib
// Filename: GraphicPack.cs
// Created By: Jared T

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using MapleLib.Collections;
using MapleLib.Common;

namespace MapleLib.WiiU
{
    [Serializable]
    public class GraphicPack
    {
        public int Id { get; set; }

        private string GPDirectory => Path.Combine(Settings.CemuDirectory, "graphicPacks", Toolbelt.Ric(Name));

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
            if (Directory.GetFileSystemEntries(path).Any())
            {
                var moveTo = $"graphicPacks.{Path.GetRandomFileName()}";
                Directory.Move(path, Path.Combine(Settings.CemuDirectory, moveTo));
            }

            if (!string.IsNullOrEmpty(Name) && !Directory.Exists(GPDirectory))
                Directory.CreateDirectory(GPDirectory);

            File.WriteAllText(Path.Combine(GPDirectory, "rules.txt"), Rules);
            Sources.ToList().ForEach(x => File.WriteAllBytes(Path.Combine(GPDirectory, x.FileName), Encoding.UTF8.GetBytes(x.Data)));
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
            public string Data { get; set; }
        }

        #endregion
    }
}