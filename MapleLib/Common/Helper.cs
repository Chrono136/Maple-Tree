// Project: MapleSeed-UI
// File: Helper.cs
// Updated By: Jared
// 

using System;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Windows.Forms;
using System.Xml;
using MessageBox = System.Windows.MessageBox;

namespace MapleLib.Common
{
    public static class Helper
    {
        public static void Uninstall()
        {
            var results = System.Windows.Forms.MessageBox.Show(@"This will remove all extra files related to MapleSeed", "", MessageBoxButtons.OKCancel);

            if (results != DialogResult.OK) return;

            if (!string.IsNullOrEmpty(Settings.ConfigDirectory))
                Directory.Delete(Settings.ConfigDirectory, true);

            System.Windows.Forms.MessageBox.Show(@"You may now delete this exe");
            Process.GetCurrentProcess().Kill();
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
                MessageBox.Show(e.Message);
            }

            return null;
        }

        public static bool IsFolder(string file)
        {
            return Directory.Exists(file);
        }

        public static string EncryptStr(string text)
        {
            return Crypto.Encrypt(text);
        }

        public static string DecryptStr(string text)
        {
            return Crypto.Decrypt(text);
        }
    }
}