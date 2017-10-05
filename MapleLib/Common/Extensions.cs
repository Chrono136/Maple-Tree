// Created: 2017/03/27 11:20 AM
// Updated: 2017/10/05 5:47 PM
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
using System.Windows.Media.Imaging;
using System.Xml;
using MapleLib.Collections;
using MapleLib.Network;
using MapleLib.Properties;
using MapleLib.Structs;
using Application = System.Windows.Application;

// ReSharper disable UnusedMember.Global

namespace MapleLib.Common
{
    public static class Extensions
    {
        public static async Task GetImage(this eShopTitle title)
        {
            if (string.IsNullOrEmpty(title.ProductCode))
                return;

            if (Settings.ImageCache.ContainsKey(title.ID))
                return;

            var code = title.ProductCode.Substring(6);

            var wiiutb = Resources.wiiutdb.Split('\n').ToList();
            var line = wiiutb.FirstOrDefault(x => x.Contains(code))?.Trim();

            if (string.IsNullOrEmpty(line)) return;
            var entries = line.Split('=');
            var id = entries[0]?.Trim();

            foreach (var region in "EN,US,JA,NL,DE,FR".Split(','))
                try
                {
                    var url = $"http://art.gametdb.com/wiiu/coverHQ/{region}/{id}.jpg";
                    if (!Web.UrlExists(url)) continue;

                    var data = await Web.DownloadDataAsync(url);
                    Settings.ImageCache[title.ID] = data;

                    using (var ms = new MemoryStream(data))
                    {
                        title.ImageLocation = BitmapFrame.Create(ms);
                    }
                }
                catch
                {
                    TextLog.MesgLog.WriteLog($"Could not locate cover image for {title}");
                }
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