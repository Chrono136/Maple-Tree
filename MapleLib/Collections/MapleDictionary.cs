// Project: MapleLib
// File: MapleDictionary.cs
// Updated By: Jared
// 

using System.IO;
using System.Threading.Tasks;
using MapleLib.Structs;

namespace MapleLib.Collections
{
    public class MapleDictionary : MapleList<Title>
    {
        public MapleDictionary(string baseDir)
        {
            BaseDir = baseDir;
        }

        private string BaseDir { get; }

        public async void OrganizeTitles()
        {
            await Task.Run(() => {
                foreach (var value in this) {
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