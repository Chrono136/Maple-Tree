// Created: 2017/08/02 8:21 AM
// Updated: 2017/10/02 1:13 PM
// 
// Project: MapleLib
// Filename: Downloader.cs
// Created By: Jared T

using System;
using System.Diagnostics;
using System.Threading.Tasks;
using MapleLib.Common;

namespace MapleLib.Network
{
    public class Downloader
    {
        public Downloader()
        {
            DownloadQueue.AddDownload += DownloadQueue_AddDownload;
            DownloadQueueWorkerTask = Task.Run(() => DownloadQueueTask());
        }

        private Task DownloadQueueWorkerTask { get; }

        private DownloadQueue DownloadQueue { get; } = new DownloadQueue();

        public void AddToQueue(string titleId, string titleFolderLocation, string contentType, string version)
        {
            var itemInfo = new ItemInfo
            {
                TitleID = titleId,
                Location = titleFolderLocation,
                ContentType = contentType,
                Version = version
            };
            DownloadQueue.Add(itemInfo);
        }

        private async void DownloadQueueTask()
        {
            while (!Process.GetCurrentProcess().HasExited)
            {
                if (DownloadQueue.Count <= 0)
                {
                    await Task.Delay(250);
                    continue;
                }

                var itemInfo = DownloadQueue[0];

                try
                {
                    TextLog.Write($"[DLQ] '{itemInfo.Name}' starting download.");
                    await DownloadProcess(itemInfo);
                    DownloadQueue.Remove(itemInfo);
                }
                catch (Exception e)
                {
                    TextLog.Write(e.Message);
                    TextLog.Write(e.StackTrace);
                    TextLog.Write($"[DLQ] '{itemInfo.Name}' failed download.");
                }
            }
        }

        private Task DownloadProcess(ItemInfo itemInfo)
        {
            return WiiuClient.DownloadTitle(itemInfo.TitleID, itemInfo.Location, itemInfo.ContentType,
                itemInfo.Version);
        }

        private void DownloadQueue_AddDownload(object sender, ItemInfo e)
        {
            TextLog.Write($"[DLQ] '{e.Name}' added to queue.");
        }
    }

    public class ItemInfo
    {
        public string Name => Database.FindTitle(TitleID)?.ToString();

        public string TitleID { get; set; }

        public string Location { get; set; }

        public string ContentType { get; set; }

        public string Version { get; set; }
    }
}