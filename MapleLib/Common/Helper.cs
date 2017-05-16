// Project: MapleSeed-UI
// File: Helper.cs
// Updated By: Jared
// 

using System;
using System.Diagnostics;
using System.IO;
using System.Management;
using System.Reflection;
using System.Windows.Forms;
using System.Xml;

namespace MapleLib.Common
{
    public static class Helper
    {
        public static uint GenerateHashFromRawRpxData(byte[] rpxData, int size)
        {
            uint h = 0x3416DCBF;
            for (var i = 0; i < size; i++) {
                uint c = rpxData[i];
                h = (h << 3) | (h >> 29);
                h += c;
            }
            return h;
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
            var results = MessageBox.Show(@"This will remove all extra files related to MapleSeed", "",
                MessageBoxButtons.OKCancel);

            if (results != DialogResult.OK) return;

            Database.Dispose();

            if (!string.IsNullOrEmpty(Settings.ConfigDirectory))
                Directory.Delete(Settings.ConfigDirectory, true);

            MessageBox.Show(@"You may now delete this exe");
            Process.GetCurrentProcess().Kill();
        }

        public static string UniqueID()
        {
            var drive = DriveInfo.GetDrives()[0].ToString().Replace(":", "").Replace("\\", "");
            var dsk = new ManagementObject(@"win32_logicaldisk.deviceid=""" + drive + @":""");
            dsk.Get();
            var volumeSerial = dsk["VolumeSerialNumber"].ToString();
            return volumeSerial;
        }

        public static void ResolveAssembly()
        {
            AppDomain.CurrentDomain.AssemblyResolve += (sender, args) => {
                var resourceName = new AssemblyName(args.Name).Name + ".dll";
                using (var stream = Assembly.GetExecutingAssembly().GetManifestResourceStream(resourceName)) {
                    if (stream == null) return null;
                    var assemblyData = new byte[stream.Length];
                    stream.Read(assemblyData, 0, assemblyData.Length);
                    return Assembly.Load(assemblyData);
                }
            };
        }

        public static string XmlGetStringByTag(string file, string tag)
        {
            try {
                var xml = new XmlDocument();
                xml.Load(file);

                using (var strTag = xml.GetElementsByTagName(tag)) {
                    return strTag.Count <= 0 ? null : strTag[0].InnerText.ToUpper();
                }
            }
            catch (Exception e) {
                System.Windows.MessageBox.Show(e.Message);
            }

            return null;
        }
    }
}