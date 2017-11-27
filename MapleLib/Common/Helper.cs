// Created: 2017/03/27 11:20 AM
// Updated: 2017/10/26 4:27 PM
// 
// Project: MapleLib
// Filename: Helper.cs
// Created By: Jared T

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net;
using System.Threading;
using System.Windows;
using System.Windows.Forms;
using System.Xml;
using MapleLib.Properties;
using MessageBox = System.Windows.MessageBox;

namespace MapleLib.Common
{
    public static class Helper
    {
        public static uint GenerateHashFromRawRpxData(byte[] rpxData, int size)
        {
            uint h = 0x3416DCBF;
            for (var i = 0; i < size; i++)
            {
                uint c = rpxData[i];
                h = (h << 3) | (h >> 29);
                h += c;
            }
            return h;
        }

        public static string RandomString(int length)
        {
            const string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
            var random = new Random();
            return new string(Enumerable.Repeat(chars, length).Select(s => s[random.Next(s.Length)]).ToArray());
        }

        public static object InvokeOnCurrentDispatcher(Action action)
        {
            return System.Windows.Application.Current.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, action);
        }

        public static string FullPath(string path1, string path2)
        {
            return Path.GetFullPath(Path.Combine(path1, path2));
        }

        public static void CheckSession()
        {
            var currentProcess = Process.GetCurrentProcess();
            var processes = Process.GetProcessesByName("MapleSeed2");

            if (currentProcess.ProcessName != "MapleSeed2")
            {
                MessageBox.Show(Resources.InvalidExecutableName);
                currentProcess.Kill();
            }

            if (processes.Length <= 1)
                return;

            var result = MessageBox.Show(Resources.Session_Still_Open, "Previous Session Open", MessageBoxButton.OKCancel);
            if (result == MessageBoxResult.OK)
            {
                foreach (var process in processes)
                    if (process.Id != currentProcess.Id)
                        process.Kill();

                Thread.Sleep(1500);
                return;
            }

            currentProcess.Kill();
        }

        public static bool InternetActive()
        {
            try
            {
                using (var client = new WebClient())
                using (client.OpenRead("http://www.google.com"))
                {
                    return true;
                }
            }
            catch
            {
                System.Windows.Forms.MessageBox.Show(@"This tool requires an Internet connection for network features.");
                return false;
            }
        }

        public static IEnumerable<string> GetFiles(string path, string pattern)
        {
            return Directory.EnumerateFiles(path, pattern, SearchOption.AllDirectories);
        }

        public static Stream FileOpenStream(string path)
        {
            var directory = Path.GetDirectoryName(path);
            if (!Directory.Exists(directory) && !string.IsNullOrEmpty(directory))
                Directory.CreateDirectory(directory);

            return File.Open(path, FileMode.OpenOrCreate, FileAccess.ReadWrite, FileShare.Read);
        }

        public static void Uninstall()
        {
            var results = System.Windows.Forms.MessageBox.Show(@"This will remove all extra files related to MapleSeed", "",
                MessageBoxButtons.OKCancel);

            if (results != DialogResult.OK) return;

            Database.Dispose();

            if (!string.IsNullOrEmpty(Settings.ConfigDirectory))
                Directory.Delete(Settings.ConfigDirectory, true);

            System.Windows.Forms.MessageBox.Show(@"You may now delete this exe");
            Process.GetCurrentProcess().Kill();
        }

        public static string XmlGetStringByTag(string file, string tag)
        {
            try
            {
                var xml = new XmlDocument();
                xml.Load(file);

                using (var strTag = xml.GetElementsByTagName(tag))
                {
                    return strTag.Count <= 0 ? null : strTag[0].InnerText.ToUpper();
                }
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
            }

            return null;
        }
    }
}