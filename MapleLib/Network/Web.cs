// Created: 2017/04/01 10:40 AM
// Updated: 2017/10/05 3:09 PM
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
        private const string WII_USER_AGENT = "wii libnup/1.1";

        public static event EventHandler<DownloadProgressChangedEventArgs> DownloadProgressChangedEvent;

        public static async Task DownloadContent(this Title title, string version = null, string contentType = null,
            bool libraryContent = false)
        {
            if (string.IsNullOrEmpty(title.ID))
                throw new Exception("Can't download content without a valid Title ID.");

            if (contentType == null)
                contentType = title.ContentType;

            if (version == null)
                version = "0";

            title.FolderLocation = Path.Combine(Settings.LibraryDirectory, title.ToString());

            #region Output Directory

            var workingId = title.ID.ToUpper();

            if (contentType == "Patch")
            {
                workingId = $"0005000E{workingId.Substring(8)}";

                if (Settings.Cemu173Patch && !libraryContent)
                    title.FolderLocation = Path.Combine(Settings.BasePatchDir, workingId.Substring(8));
            }

            if (contentType == "DLC")
            {
                workingId = $"0005000C{workingId.Substring(8)}";

                if (Settings.Cemu173Patch && !libraryContent)
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
                wc.Headers[HttpRequestHeader.UserAgent] = WII_USER_AGENT;
                wc.Headers[HttpRequestHeader.CacheControl] = "max-age=0, no-cache, no-store";
                wc.DownloadProgressChanged += DownloadProgressChanged;
                await wc.DownloadFileTaskAsync(new Uri(url), saveTo);
                while (wc.IsBusy) await Task.Delay(100);
            }
        }

        public static string DownloadString(string url)
        {
            if (!Helper.InternetActive())
                return null;

            using (var wc = new WebClient())
            {
                wc.Headers[HttpRequestHeader.UserAgent] = WII_USER_AGENT;
                wc.Headers[HttpRequestHeader.CacheControl] = "max-age=0, no-cache, no-store";
                wc.DownloadProgressChanged += DownloadProgressChanged;
                return wc.DownloadString(url);
            }
        }

        public static async Task<string> DownloadStringAsync(string url)
        {
            if (!Helper.InternetActive())
                return null;

            using (var wc = new WebClient())
            {
                wc.Headers[HttpRequestHeader.UserAgent] = WII_USER_AGENT;
                wc.Headers[HttpRequestHeader.CacheControl] = "max-age=0, no-cache, no-store";
                wc.DownloadProgressChanged += DownloadProgressChanged;
                return await wc.DownloadStringTaskAsync(url);
            }
        }

        public static async Task<byte[]> DownloadDataAsync(string url)
        {
            if (!Helper.InternetActive())
                return new byte[0];

            using (var wc = new WebClient())
            {
                wc.Headers[HttpRequestHeader.UserAgent] = WII_USER_AGENT;
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
            WiiuClient.ReportProgress(0, (int) e.TotalBytesToReceive, (int) e.BytesReceived);
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
            catch (Exception)
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