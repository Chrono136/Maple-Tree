// Project: MapleRoot
// File: Extensions.cs
// Updated By: Jared
// 

using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.IO.Compression;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using MapleLib.Collections;
using MapleLib.Structs;
using Application = System.Windows.Application;

namespace MapleLib.Common
{
    public static class Extensions
    {
        public static string GetString(this ZipArchiveEntry entry)
        {
            return entry.Open().toString();
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

        public static string toString(this Stream stream)
        {
            using (var ms = new MemoryStream())
            {
                stream.CopyTo(ms);
                return Encoding.UTF8.GetString(ms.ToArray());
            }
        }

        public static T[] Slice<T>(this T[] source, int start, int end)
        {
            // Handles negative ends.
            if (end < 0)
            {
                end = source.Length + end;
            }
            int len = end - start;

            // Return new array.
            T[] res = new T[len];
            for (int i = 0; i < len; i++)
            {
                res[i] = source[i + start];
            }
            return res;
        }

        public static T Random<T>(this IList<T> value)
        {
            return value.Count > 0 ? value[new Random().Next(value.Count - 1)] : default(T);
        }

        public static byte[] HexToBytes(this string hexEncodedBytes)
        {
            int start = 0;
            int end = hexEncodedBytes.Length;

            int length = end - start;
            const string tagName = "hex";
            string fakeXmlDocument = String.Format("<{1}>{0}</{1}>", hexEncodedBytes.Substring(start, length), tagName);
            var stream = new MemoryStream(Encoding.ASCII.GetBytes(fakeXmlDocument));
            XmlReader reader = XmlReader.Create(stream, new XmlReaderSettings());
            int hexLength = length / 2;
            byte[] result = new byte[hexLength];
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

            for (int i = 0; i < list.Length; i++)
            {
                int.TryParse(list[i].Replace("v", "").Trim(), out vers[i]);
            }

            return vers;
        }

        public static void AppendText(this RichTextBox box, string text, Color color)
        {
            try {
                if (box.InvokeRequired) {
                    box.Invoke(new Action(() => {
                        box.SelectionStart = box.TextLength;
                        box.SelectionLength = 0;

                        box.SelectionColor = color;
                        box.AppendText(text);
                        box.SelectionColor = box.ForeColor;
                        box.ScrollToCaret();
                    }));
                }
                else {
                    box.SelectionStart = box.TextLength;
                    box.SelectionLength = 0;

                    box.SelectionColor = color;
                    box.AppendText(text);
                    box.SelectionColor = box.ForeColor;
                    box.ScrollToCaret();
                }
            }
            catch (Exception) {
                
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
        
        public static void AddOnUI(this MapleDictionary collection, Title item)
        {
            var add = new Action(()=> collection.Add(item));
            Application.Current?.Dispatcher.BeginInvoke(add);
        }
    }
}