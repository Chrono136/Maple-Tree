// Created: 2017/03/27 11:20 AM
// Updated: 2017/10/05 6:12 PM
// 
// Project: MapleLib
// Filename: MapleDictionary.cs
// Created By: Jared T

using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;
using MapleLib.Structs;

namespace MapleLib.Collections
{
    [Serializable]
    public class MapleDictionary : MapleList<Title>
    {
        public MapleDictionary() { }

        public MapleDictionary(IList<Title> list) : base(list) { }

        public async void OrganizeTitles()
        {
            await Task.Run(() =>
            {
                foreach (var value in this)
                {
                    var fromLocation = value.FolderLocation;
                    var toLocation = Path.Combine(Settings.LibraryDirectory, value.ToString());

                    if (!Directory.Exists(fromLocation) || Directory.Exists(toLocation))
                        continue;

                    Directory.Move(fromLocation, toLocation);
                    value.FolderLocation = toLocation;
                    value.MetaLocation = value.MetaLocation.Replace(fromLocation, toLocation);
                }
            });
        }
    }
}