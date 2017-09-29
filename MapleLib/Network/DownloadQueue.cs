// Created: 2017/08/02 9:25 AM
// Updated: 2017/09/29 2:05 AM
// 
// Project: MapleLib
// Filename: DownloadQueue.cs
// Created By: Jared T

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