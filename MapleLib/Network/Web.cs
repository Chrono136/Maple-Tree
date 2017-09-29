// Created: 2017/04/01 10:40 AM
// Updated: 2017/09/29 2:05 AM
// 
// Project: MapleLib
// Filename: Web.cs
// Created By: Jared T

#region usings

using System;
using System.IO;
using System.Net;
using System.Threading.Tasks;
using MapleLib.Common;
using MapleLib.Structs;

#endregion

namespace MapleLib.Network
{
    public static class Web
    {
        private const string WiiUserAgent = "wii libnup/1.1";

        public static event EventHandler<DownloadProgressChangedEventArgs> DownloadProgressChangedEvent;

        public static async Task DownloadContent(this Title title, string contentType, string version)
        {
            if (string.IsNullOrEmpty(title.ID))
                throw new Exception("Can't download content without a valid Title ID.");

            if (string.IsNullOrEmpty(title.FolderLocation))
                title.FolderLocation = Path.GetFullPath(Path.Combine(Settings.LibraryDirectory, $"{title}"));

            #region Output Directory

            var workingId = title.ID.ToUpper();

            if (contentType == "Patch")
            {
                workingId = $"0005000E{workingId.Substring(8)}";

                if (Settings.Cemu173Patch)
                    title.FolderLocation = Path.Combine(Settings.BasePatchDir, workingId.Substring(8));
            }

            if (contentType == "DLC")
            {
                workingId = $"0005000C{workingId.Substring(8)}";

                if (Settings.Cemu173Patch)
                    title.FolderLocation = Path.Combine(Settings.BasePatchDir, workingId.Substring(8), "aoc");
            }

            #endregion

            await Database.DownloadTitle(title.ID, title.FolderLocation, contentType, version);
        }

        public static async Task DownloadFileAsync(string url, string saveTo)
        {
            if (!Helper.InternetActive())
                return;

            using (var wc = new WebClient())
            {
                wc.Headers[HttpRequestHeader.UserAgent] = WiiUserAgent;
                wc.Headers[HttpRequestHeader.CacheControl] = "max-age=0, no-cache, no-store";
                wc.DownloadProgressChanged += DownloadProgressChanged;
                await wc.DownloadFileTaskAsync(new Uri(url), saveTo);
                while (wc.IsBusy) await Task.Delay(100);
            }
        }

        public static string DownloadString(string url)
        {
            if (!Helper.InternetActive())
                return string.Empty;

            using (var wc = new WebClient())
            {
                wc.Headers[HttpRequestHeader.UserAgent] = WiiUserAgent;
                wc.Headers[HttpRequestHeader.CacheControl] = "max-age=0, no-cache, no-store";
                wc.DownloadProgressChanged += DownloadProgressChanged;
                return wc.DownloadString(url);
            }
        }

        public static async Task<byte[]> DownloadDataAsync(string url)
        {
            if (!Helper.InternetActive())
                return new byte[0];

            using (var wc = new WebClient())
            {
                wc.Headers[HttpRequestHeader.UserAgent] = WiiUserAgent;
                wc.Headers[HttpRequestHeader.CacheControl] = "max-age=0, no-cache, no-store";
                wc.DownloadProgressChanged += DownloadProgressChanged;
                return await wc.DownloadDataTaskAsync(new Uri(url));
            }
        }

        public static void ResetDownloadProgressChanged()
        {
            DownloadProgressChangedEvent?.Invoke(null, null);
        }

        private static void DownloadProgressChanged(object sender, DownloadProgressChangedEventArgs e)
        {
            DownloadProgressChangedEvent?.Invoke(sender, e);
        }

        public static bool UrlExists(string url)
        {
            var webRequest = WebRequest.Create(url);
            webRequest.Timeout = 1200;
            webRequest.Method = "HEAD";

            HttpWebResponse response = null;

            try
            {
                using (response = (HttpWebResponse) webRequest.GetResponse())
                {
                    if (response.StatusCode == HttpStatusCode.OK)
                        return true;
                }
            }
            catch
            {
                // ignored
            }
            finally
            {
                response?.Close();
            }

            return false;
        }
    }
}