// Project: MapleLib
// File: DownloadQueue.cs
// Updated By: Jared
// 

using System;
using System.Collections.Generic;

namespace MapleLib.Network
{
    public class DownloadQueue : List<ItemInfo>
    {
        public event EventHandler<ItemInfo> AddDownload;

        public new void Add(ItemInfo itemInfo)
        {
            AddDownload?.Invoke(this, itemInfo);
            base.Add(itemInfo);
        }
    }
}