// Project: MapleLib
// File: WiiuClient.cs
// Updated By: Jared
// 

using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Net;
using System.Threading.Tasks;
using System.Windows.Forms;
using libWiiSharp;
using MapleLib.Common;
using MapleLib.Structs;
using MapleLib.WiiU;

namespace MapleLib.Network
{
    public static class WiiuClient
    {
        public static event EventHandler<ProgressReport> ProgressReport;

        private static async Task<byte[]> DownloadData(string url)
        {
            byte[] data = {};

            try {
                if (Helper.InternetActive())
                    data = await Web.DownloadDataAsync(url);
            }
            catch (WebException e) {
                TextLog.MesgLog.AddHistory($"{e.Message}\n{e.StackTrace}");
            }

            return data;
        }

        private static async Task<TMD> DownloadTmd(string url, string saveTo)
        {
            var data = await DownloadData(url);
            if (data.Length <= 0) return null;
            var tmd = TMD.Load(data);

            if (tmd.TitleVersion > 999) return null;
            Toolbelt.AppendLog("  - Parsing TMD...");
            Toolbelt.AppendLog($"    + Title Version: {tmd.TitleVersion}");
            Toolbelt.AppendLog($"    + {tmd.NumOfContents} Contents");

            File.WriteAllBytes(saveTo, data);
            return tmd;
        }

        private static async Task<TMD> LoadTmd(string id, string key, string outputDir, string titleUrl, string version)
        {
            var tmdFile = Path.Combine(outputDir, "tmd");

            if (string.IsNullOrEmpty(id) || string.IsNullOrEmpty(key))
                return null;

            version = int.Parse(version) == 0 ? "" : $".{version}";
            if (await DownloadTmd(titleUrl + $"tmd{version}", tmdFile) == null) {
                var url = $"http://ccs.cdn.wup.shop.nintendo.net/ccs/download/{id.ToLower()}/tmd";

                await DownloadTmd(url, tmdFile);
            }

            var file = new FileInfo(tmdFile);
            if (!file.Exists || file.Length <= 0)
                return null;

            return TMD.Load(tmdFile);
        }

        public static async Task DownloadTitle(string id, string outputDir, string contentType, string version)
        {
            #region Setup

            var workingId = id.ToUpper();

            Title title;
            if ((title = Database.FindTitle(workingId)) == null)
                throw new Exception("Could not locate the title key");

            var key = title.Key;
            var name = title.Name;
            if (string.IsNullOrEmpty(key) || string.IsNullOrEmpty(name)) return;

            if (!Directory.Exists(outputDir))
                Directory.CreateDirectory(outputDir);

            var str = $"Download {contentType} content to the following location?\n\"{outputDir}\"";
            var result = MessageBox.Show(str, name, MessageBoxButtons.YesNo);

            if (result != DialogResult.Yes)
                return;

            Toolbelt.AppendLog($"Output Directory '{outputDir}'");

            #endregion

            #region TMD

            Toolbelt.AppendLog("  - Loading TMD...");
            TMD tmd = null;

            var nusUrls = new List<string>
            {
                "http://ccs.cdn.wup.shop.nintendo.net/ccs/download/",
                "http://nus.cdn.shop.wii.com/ccs/download/",
                "http://ccs.cdn.c.shop.nintendowifi.net/ccs/download/"
            };

            foreach (var nusUrl in nusUrls) {
                string titleUrl = $"{nusUrl}{workingId}/";
                tmd = await LoadTmd(id, key, outputDir, titleUrl, version);

                if (tmd != null)
                    break;
            }

            if (tmd == null) {
                TextLog.MesgLog.WriteError("Could not locate TMD. Is this content request valid?");
                return;
            }

            #endregion

            #region Ticket

            Toolbelt.AppendLog("Generating Ticket...");

            var tikData = MapleTicket.Create(Database.FindTitle(id));
            if (tikData == null)
                throw new Exception("Invalid ticket data. Verify Title ID.");

            var ticket = Ticket.Load(tikData);
            ticket.Save(Path.Combine(outputDir, "cetk"));

            #endregion

            #region Content

            Toolbelt.AppendLog($"[+] [{contentType}] {name} v{tmd.TitleVersion}");
            Toolbelt.SetStatus($"Output Directory: {outputDir}");

            foreach (var nusUrl in nusUrls) {
                var url = nusUrl + workingId;
                if (await DownloadContent(tmd, outputDir, url) != 1)
                    continue;

                Toolbelt.AppendLog(string.Empty);
                Toolbelt.AppendLog("  - Decrypting Content");
                Toolbelt.AppendLog("  + This may take a minute. Please wait...");
                Toolbelt.SetStatus("Decrypting Content. This may take a minute. Please wait...", Color.OrangeRed);

                if (await Toolbelt.CDecrypt(outputDir) != 0) {
                    CleanUp(outputDir, tmd);
                    Toolbelt.AppendLog($"Error while decrypting {name}");
                    return;
                }

                CleanUp(outputDir, tmd);
                break;
            }

            #endregion

            Web.ResetDownloadProgressChanged();
            Toolbelt.AppendLog($"[+] [{contentType}] {name} v{tmd.TitleVersion} Finished.");
            Toolbelt.SetStatus($"[+] [{contentType}] {name} v{tmd.TitleVersion} Finished.");
        }

        private static async Task<int> DownloadContent(TMD tmd, string outputDir, string titleUrl)
        {
            var result = 0;
            for (var i = 0; i < tmd.NumOfContents; i++) {
                var i1 = i;
                result = await Task.Run(async () => {
                    var numc = tmd.NumOfContents;
                    var size = Toolbelt.SizeSuffix((long) tmd.Contents[i1].Size);
                    Toolbelt.AppendLog($"Downloading Content #{i1 + 1} of {numc}... ({size})");
                    var contentPath = Path.Combine(outputDir, tmd.Contents[i1].ContentID.ToString("x8"));

                    if (!Toolbelt.IsValid(tmd.Contents[i1], contentPath))
                        try {
                            var downloadUrl = $"{titleUrl}/{tmd.Contents[i1].ContentID:x8}";
                            await Web.DownloadFileAsync(downloadUrl, contentPath);
                        }
                        catch (Exception ex) {
                            Toolbelt.AppendLog($"Downloading Content #{i1 + 1} of {numc} failed...\n{ex.Message}");
                            return 0;
                        }
                    ReportProgress(0, tmd.NumOfContents - 1, i1);
                    return 1;
                });
                if (result == 0)
                    break;
            }
            ReportProgress(0, 100, 0);
            return result;
        }

        private static void ReportProgress(int min, int max, int value)
        {
            ProgressReport?.Invoke(null, new ProgressReport {Min = min, Max = max, Value = value});
        }

        private static void CleanUp(string outputDir, TMD tmd)
        {
            try {
                if (!Settings.StoreEncryptedContent) {
                    Toolbelt.AppendLog("  - Deleting Encrypted Contents...");
                    foreach (var t in tmd.Contents) {
                        if (!File.Exists(Path.Combine(outputDir, t.ContentID.ToString("x8")))) continue;
                        File.Delete(Path.Combine(outputDir, t.ContentID.ToString("x8")));
                    }
                }

                Toolbelt.AppendLog("  - Deleting CDecrypt, libeay32, and msvcr120d...");
                File.Delete(Path.Combine(outputDir, "CDecrypt.exe"));
                File.Delete(Path.Combine(outputDir, "libeay32.dll"));
                File.Delete(Path.Combine(outputDir, "msvcr120d.dll"));
            }
            catch {
                // ignored
            }
        }
    }
}