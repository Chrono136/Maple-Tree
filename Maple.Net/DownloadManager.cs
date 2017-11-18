// Created: 2017/11/18 3:07 PM
// Updated: 2017/11/18 3:45 PM
// 
// Project: Maple.Net
// Filename: DownloadManager.cs
// Created By: Jared T

using System;
using System.Diagnostics;
using System.Threading.Tasks;

namespace Maple.Net
{
    public class DownloadManager
    {
        private readonly Func<string, string, string, string, Task> _downloadTitleTask;

        public DownloadManager(Func<string, string, string, string, Task> downloadTitleTask)
        {
            DownloadQueueWorkerTask = DownloadQueueTask();

            _downloadTitleTask = downloadTitleTask;
        }

        private Task DownloadQueueWorkerTask { get; }

        private DownloadQueue DownloadQueue { get; } = new DownloadQueue();

        public void AddToQueue(string titleId, string titleFolderLocation, string contentType, string version)
        {
            var itemInfo = new ItemInfo
            {
                TitleId = titleId,
                Location = titleFolderLocation,
                ContentType = contentType,
                Version = version
            };
            DownloadQueue.Add(itemInfo);
        }

        private async Task DownloadQueueTask()
        {
            while (!Process.GetCurrentProcess().HasExited)
            {
                if (DownloadQueue.Count <= 0)
                {
                    await Task.Delay(250);
                    continue;
                }

                var itemInfo = DownloadQueue[0];
                await DownloadProcess(itemInfo);
                DownloadQueue.Remove(itemInfo);
            }
        }

        private async Task DownloadProcess(ItemInfo itemInfo)
        {
            await _downloadTitleTask(itemInfo.TitleId, itemInfo.Location, itemInfo.ContentType, itemInfo.Version);
        }
    }

    public class ItemInfo
    {
        public string TitleId { get; set; }

        public string Location { get; set; }

        public string ContentType { get; set; }

        public string Version { get; set; }
    }
}