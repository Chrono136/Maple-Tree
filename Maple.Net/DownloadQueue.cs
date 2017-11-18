// Created: 2017/11/18 3:07 PM
// Updated: 2017/11/18 3:08 PM
// 
// Project: Maple.Net
// Filename: DownloadQueue.cs
// Created By: Jared T

using System;
using System.Collections.Generic;

namespace Maple.Net
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