// Created: 2017/03/27 11:20 AM
// Updated: 2017/10/02 11:07 AM
// 
// Project: MapleLib
// Filename: Extensions.cs
// Created By: Jared T

using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml;
using MapleLib.Collections;
using MapleLib.Network;
using MapleLib.Structs;
using Application = System.Windows.Application;

// ReSharper disable UnusedMember.Global

namespace MapleLib.Common
{
    public static class Extensions
    {
        public static async Task<string> Image(this eShopTitle title, bool save = true)
        {
            return string.Empty;

            if (string.IsNullOrEmpty(title.ProductCode) || title.ProductCode.Length <= 6)
                return string.Empty;

            var code = title.ProductCode.Substring(6);

            var compCode = "01";
            if (!string.IsNullOrEmpty(title.CompanyCode) && title.CompanyCode.Length > 3 && title.CompanyCode != "None")
                compCode = title.CompanyCode.Substring(2);

            /*var doc = new XmlDocument();
            doc.Load(new MemoryStream(Encoding.UTF8.GetBytes(Properties.Resources.wiiutdb)));

            var values = doc.GetElementsByTagName("id").Cast<XmlNode>().ToList();
            var value = values.Find(x => x.InnerText.Contains(code));
            var imageCode = value?.InnerText;*/

            var imageCode = code + compCode;

            if (imageCode.IsNullOrEmpty())
                return string.Empty;

            var cacheDir = Path.Combine(Settings.ConfigDirectory, "cache");
            if (!Directory.Exists(cacheDir)) Directory.CreateDirectory(cacheDir);

            string cachedFile;
            if (File.Exists(cachedFile = Path.Combine(cacheDir, $"{imageCode}.jpg")))
                return title.ImageLocation = cachedFile;

            foreach (var langCode in "US,EN,DE,FR,JA".Split(','))
                try
                {
                    var url = $"http://art.gametdb.com/wiiu/coverHQ/{langCode}/{imageCode}.jpg";

                    if (!Web.UrlExists(url)) continue;
                    title.ImageLocation = cachedFile;

                    if (!save) return string.Empty;
                    var data = await Web.DownloadDataAsync(url);
                    File.WriteAllBytes(title.ImageLocation, data);
                }
                catch
                {
                    TextLog.MesgLog.WriteLog($"Could not locate cover image for {title}");
                }

            return title.ImageLocation;
        }

        public static bool FilePathHasInvalidChars(this string path)
        {
            if (string.IsNullOrEmpty(path))
                return false;

            try
            {
                // ReSharper disable once UnusedVariable
                var fileName = Path.GetFileName(path);

                // ReSharper disable once UnusedVariable
                var fileDirectory = Path.GetFullPath(path);
            }
            catch (ArgumentException)
            {
                return true;
            }
            return false;
        }

        public static string GetString(this ZipArchiveEntry entry)
        {
            return ToString(entry.Open());
        }

        public static byte[] GetBytes(this ZipArchiveEntry entry)
        {
            using (var ms = new MemoryStream())
            {
                entry.Open().CopyTo(ms);
                return ms.ToArray();
            }
        }

        public static byte[] GetBytes(this Stream stream)
        {
            using (var ms = new MemoryStream())
            {
                stream.CopyTo(ms);
                return ms.ToArray();
            }
        }

        private static string ToString(this Stream stream)
        {
            using (var ms = new MemoryStream())
            {
                stream.CopyTo(ms);
                return Encoding.Default.GetString(ms.ToArray());
            }
        }

        public static T[] Slice<T>(this T[] source, int start, int end)
        {
            // Handles negative ends.
            if (end < 0)
                end = source.Length + end;
            var len = end - start;

            // Return new array.
            var res = new T[len];
            for (var i = 0; i < len; i++)
                res[i] = source[i + start];
            return res;
        }

        public static T Random<T>(this IList<T> value)
        {
            if (value != null && value.Any())
                return value[new Random().Next(value.Count - 1)];

            return default(T);
        }

        public static byte[] HexToBytes(this string hexEncodedBytes)
        {
            var start = 0;
            var end = hexEncodedBytes.Length;

            var length = end - start;
            const string tagName = "hex";
            var fakeXmlDocument = string.Format("<{1}>{0}</{1}>", hexEncodedBytes.Substring(start, length), tagName);
            var stream = new MemoryStream(Encoding.ASCII.GetBytes(fakeXmlDocument));
            var reader = XmlReader.Create(stream, new XmlReaderSettings());
            var hexLength = length / 2;
            var result = new byte[hexLength];
            reader.ReadStartElement(tagName);
            reader.ReadContentAsBinHex(result, 0, hexLength);
            return result;
        }

        public static byte[] ToBytes(this int i)
        {
            return BitConverter.GetBytes(i);
        }

        public static int[] ToIntList(this string value, char delimiter)
        {
            var list = value.Split(delimiter);
            var vers = new int[list.Length];

            for (var i = 0; i < list.Length; i++)
                int.TryParse(list[i].Replace("v", "").Trim(), out vers[i]);

            return vers;
        }

        public static void AppendText(this RichTextBox box, string text, Color color)
        {
            try
            {
                if (box.InvokeRequired)
                {
                    box.Invoke(new Action(() =>
                    {
                        box.SelectionStart = box.TextLength;
                        box.SelectionLength = 0;

                        box.SelectionColor = color;
                        box.AppendText(text);
                        box.SelectionColor = box.ForeColor;
                        box.ScrollToCaret();
                    }));
                }
                else
                {
                    box.SelectionStart = box.TextLength;
                    box.SelectionLength = 0;

                    box.SelectionColor = color;
                    box.AppendText(text);
                    box.SelectionColor = box.ForeColor;
                    box.ScrollToCaret();
                }
            }
            catch (Exception e)
            {
                TextLog.Write(e.Message);
                TextLog.Write(e.StackTrace);
            }
        }

        public static string TimeStamp(this DateTime dateTime)
        {
            return dateTime.ToString("T");
        }

        public static bool IsNullOrEmpty(this string str)
        {
            return string.IsNullOrEmpty(str);
        }

        public static async void AddOnUi(this MapleDictionary collection, Title item)
        {
            var action = new Action(() => collection.Add(item));

            if (Application.Current == null) action.DynamicInvoke();
            else await Application.Current.Dispatcher.BeginInvoke(action);
        }

        public static DialogResult StaShowDialog(this CommonDialog dialog)
        {
            var state = new DialogState {Dialog = dialog};
            var t = new Thread(state.ThreadProcShowDialog);
            t.SetApartmentState(ApartmentState.STA);
            t.Start();
            t.Join();
            return state.Result;
        }

        #region Nested type: DialogState

        private class DialogState
        {
            public CommonDialog Dialog;
            public DialogResult Result;


            public void ThreadProcShowDialog()
            {
                Result = Dialog.ShowDialog();
            }
        }

        #endregion
    }
}