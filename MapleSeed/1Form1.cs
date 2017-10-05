// Created: 2017/03/27 11:29 AM
// Updated: 2017/09/29 2:03 AM
// 
// Project: MapleSeed
// Filename: 1Form1.cs
// Created By: Jared T

#region usings

using System;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net;
using System.Reflection;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using MapleLib;
using MapleLib.Collections;
using MapleLib.Common;
using MapleLib.Enums;
using MapleLib.Network;
using MapleLib.Properties;
using MapleLib.Structs;

#endregion

namespace MapleSeed
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            Text = $@"MapleSeed {Assembly.GetEntryAssembly().GetName().Version}";
            Text = $@"MapleSeed 1{Settings.Version.TrimStart('2')}";
            MinimumSize = MaximumSize = Size;

            Controls.Cast<Control>().ToList().ForEach(x => x.Enabled = false);

            InitSettings();

            RegisterEvents();

            RegisterDefaults();

            new Update(UpdateType.MapleSeed).CheckForUpdate();

            AppendLog($"Game Directory [{Settings.LibraryDirectory}]");
            AppendLog(@"Welcome to Maple Tree.");
            AppendLog(@"Enter /help for a list of possible commands.");

            Controls.Cast<Control>().ToList().ForEach(x => x.Enabled = true);
        }

        private void RegisterEvents()
        {
            Database.RegisterEvent(OnAddItemEvent);

            TextLog.MesgLog.NewLogEntryEventHandler += MesgLog_NewLogEntryEventHandler;
            TextLog.StatusLog.NewLogEntryEventHandler += StatusLog_NewLogEntryEventHandler;
            Web.DownloadProgressChangedEvent += Network_DownloadProgressChangedEvent;
        }

        private void RegisterDefaults()
        {
            fullScreen.Checked = Settings.FullScreenMode;
            cemu173Patch.Checked = Settings.Cemu173Patch;
            storeEncCont.Checked = Settings.StoreEncryptedContent;

            titleDir.Text = Settings.LibraryDirectory;
            cemuDir.Text = Settings.CemuDirectory;
            serverHub.Text = Settings.Hub;
        }

        private static void InitSettings()
        {
            if (string.IsNullOrEmpty(Settings.CemuDirectory) ||
                !File.Exists(Path.Combine(Settings.CemuDirectory, "cemu.exe")))
            {
                var ofd = new OpenFileDialog
                {
                    CheckFileExists = true,
                    Filter = @"Cemu Executable |cemu.exe"
                };

                var result = ofd.ShowDialog();
                if (string.IsNullOrWhiteSpace(ofd.FileName) || result != DialogResult.OK)
                {
                    MessageBox.Show(@"Cemu Directory is required to launch titles.");
                    Settings.CemuDirectory = string.Empty;
                }

                Settings.CemuDirectory = Path.GetDirectoryName(ofd.FileName);
            }

            if (string.IsNullOrEmpty(Settings.LibraryDirectory) || !Directory.Exists(Settings.LibraryDirectory))
            {
                var fbd = new FolderBrowserDialog
                {
                    Description = @"Cemu Title Directory" + Environment.NewLine + @"(Where you store games)"
                };

                var result = fbd.ShowDialog();
                if (string.IsNullOrWhiteSpace(fbd.SelectedPath) || result == DialogResult.Cancel)
                {
                    MessageBox.Show(@"Title Directory is required. Shutting down.");
                    Application.Exit();
                }

                Settings.LibraryDirectory = fbd.SelectedPath;
            }
        }

        private async void SetCurrentImage(eShopTitle title)
        {
            //TODO: FIX ME
            //pictureBox1.ImageLocation = await title.Image();
        }

        private async void OnAddItemEvent(object sender, AddItemEventArgs<Title> e)
        {
            var title = e.Item;
            if (title == null) return;

            await title.GetImage();

            if (pictureBox1.ImageLocation.IsNullOrEmpty())
                SetCurrentImage(title);

            titleList.BeginInvoke(new Action(() => { titleList.Items.Add(title); }));
        }

        private void UpdateProgressBar(int percent, long toReceive, long received)
        {
            if (percent <= 0 || toReceive <= 0 || received <= 0)
                return;

            try
            {
                Invoke(new Action(() => progressBar.Value = percent));

                var stoReceive = Toolbelt.SizeSuffix(toReceive);
                var sreceived = Toolbelt.SizeSuffix(received);

                progressOverlay.Invoke(new Action(() => { progressOverlay.Text = $@"{sreceived} / {stoReceive}"; }));
            }
            catch (Exception ex)
            {
                TextLog.MesgLog.WriteError($"{ex.Message}\n{ex.StackTrace}");
            }
        }

        private void AppendLog(string msg, Color color = default(Color))
        {
            TextLog.MesgLog.WriteLog(msg, color);
        }

        private async Task DownloadContentClick(Control btn, string contentType, string version = "0")
        {
            btn.Enabled = false;

            foreach (var item in titleList.SelectedItems)
            {
                var title = item as Title;
                if (title == null) continue;

                if (title.ID.IsNullOrEmpty())
                    throw new NullReferenceException("Title ID is null or empty, can't proceed!");

                if (title.Name.IsNullOrEmpty())
                    throw new NullReferenceException($"[{title}] Title Name is null or empty, can't proceed!");

                string id;

                if (contentType == "DLC" && title.HasDLC)
                {
                    id = $"0005000C{title.Lower8Digits()}";
                    title = await Database.FindTitleAsync(id);
                    if (title == null) continue;
                    await title.DownloadDLC();
                }

                if (contentType == "Patch")
                {
                    if (!title.Versions.Any())
                    {
                        MessageBox.Show($@"Update for {title.Name} is not available");
                        return;
                    }

                    id = $"0005000E{title.Lower8Digits()}";
                    title = await Database.FindTitleAsync(id);
                    if (title == null) continue;
                    await title.DownloadUpdate(version);
                }

                if (contentType == "eShop/Application")
                    await title.DownloadContent(version);
            }

            btn.Enabled = true;
        }

        private void Network_DownloadProgressChangedEvent(object sender, DownloadProgressChangedEventArgs e)
        {
            if (e == null) return;
            UpdateProgressBar(e.ProgressPercentage, e.TotalBytesToReceive, e.BytesReceived);
        }

        private void MesgLog_NewLogEntryEventHandler(object sender, NewLogEntryEvent e)
        {
            outputTextbox.AppendText(e.Entry, e.Color);
        }

        private void StatusLog_NewLogEntryEventHandler(object sender, NewLogEntryEvent e)
        {
            if (status.InvokeRequired)
            {
                status.Invoke(new Action(() =>
                {
                    status.Text = e.Entry;
                    status.ForeColor = e.Color;
                }));
            }
            else
            {
                status.Text = e.Entry;
                status.ForeColor = e.Color;
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            foreach (var proc in Process.GetProcessesByName("CDecrypt"))
                try
                {
                    proc.Kill();
                }
                catch
                {
                    // ignored
                }

            Application.Exit();
        }

        private void uninstallBtn_Click(object sender, EventArgs e)
        {
            Helper.Uninstall();
        }

        private void playBtn_Click(object sender, EventArgs e)
        {
            var title = titleList.SelectedItem as Title;
            if (title == null) return;

            new Thread(() => { Toolbelt.LaunchCemu(title.MetaLocation, null); }).Start();
        }

        private async void dlcBtn_Click(object sender, EventArgs e)
        {
            await DownloadContentClick(dlcBtn, "DLC");
        }

        private async void updateBtn_Click(object sender, EventArgs e)
        {
            int ver;
            var version = int.TryParse(titleVersion.Text, out ver) ? ver.ToString() : "0";
            await DownloadContentClick(updateBtn, "Patch", version);
        }

        private async void cleanTitleBtn_Click(object sender, EventArgs e)
        {
            cleanTitleBtn.Enabled = false;
            var result = DialogResult.Cancel;

            foreach (var item in titleList.SelectedItems)
            {
                if (result != DialogResult.OK)
                {
                    var msg = $"This task will delete your '{item}' directory and re-download the base title content!";
                    result = MessageBox.Show(msg, $@"Reinstall {item}", MessageBoxButtons.OKCancel);

                    if (result != DialogResult.OK)
                        continue;
                }

                var title = item as Title;
                if (title == null) continue;

                if (title.FolderLocation.IsNullOrEmpty())
                    throw new Exception($"[{title}] title.FolderLocation can not be empty or null");

                var fullPath = Path.GetFullPath(title.FolderLocation);
                if (string.IsNullOrEmpty(fullPath))
                    return;

                if (Directory.Exists(Path.Combine(fullPath, "code")))
                    Directory.Delete(Path.Combine(fullPath, "code"), true);

                if (Directory.Exists(Path.Combine(fullPath, "content")))
                    Directory.Delete(Path.Combine(fullPath, "content"), true);

                await title.DownloadContent();
            }

            cleanTitleBtn.Enabled = true;
        }

        private async void newdlbtn_Click(object sender, EventArgs e)
        {
            var titleId = titleIdTextBox.Text;
            if (string.IsNullOrEmpty(titleId)) return;

            Title title;
            if ((title = await Database.FindTitleAsync(titleId)) == null) return;
            title.FolderLocation = Path.Combine(Settings.LibraryDirectory, $"{title}");

            await title.DownloadContent(titleVersion.Text);

            Database.AddTitle(title);
        }

        private void organizeBtn_Click(object sender, EventArgs e)
        {
            foreach (var value in Database.GetLibrary())
                AppendLog(Path.Combine(Settings.LibraryDirectory, value.ToString()));

            var result = MessageBox.Show(Resources.OrganizeBtn_Click_, Resources.PleaseConfirmAction,
                MessageBoxButtons.OKCancel);

            if (result == DialogResult.OK)
                Database.GetLibrary().OrganizeTitles();
        }

        private void pictureBox1_Click(object sender, EventArgs e)
        {
            playBtn_Click(null, null);
        }

        private void clearCache_Click(object sender, EventArgs e)
        {
            var deleteDir = Path.Combine(Settings.ConfigDirectory, "cache");
            var result = MessageBox.Show(string.Format(Resources.WillDeleteContents, deleteDir),
                Resources.PleaseConfirmAction, MessageBoxButtons.OKCancel);

            if (result == DialogResult.OK)
                Directory.Delete(deleteDir, true);
        }

        private void fullScreen_CheckedChanged(object sender, EventArgs e)
        {
            Settings.FullScreenMode = fullScreen.Checked;
        }

        private void titleDir_TextChanged(object sender, EventArgs e)
        {
            Settings.LibraryDirectory = titleDir.Text;
        }

        private void cemuDir_TextChanged(object sender, EventArgs e)
        {
            Settings.CemuDirectory = cemuDir.Text;
        }

        private void serverHub_TextChanged(object sender, EventArgs e)
        {
            Settings.Hub = serverHub.Text;
        }

        private void cemu173Patch_CheckedChanged(object sender, EventArgs e)
        {
            Settings.Cemu173Patch = cemu173Patch.Checked;
        }

        private void storeEncCont_CheckedChanged(object sender, EventArgs e)
        {
            Settings.StoreEncryptedContent = storeEncCont.Checked;
        }

        private async void titleIdTextBox_TextChanged(object sender, EventArgs e)
        {
            if (titleIdTextBox.Text.Length != 16)
                return;

            var title = await Database.FindTitleAsync(titleIdTextBox.Text);
            if (title == null) return;

            titleName.Text = Toolbelt.Ric(title.Name);

            SetCurrentImage(title);
        }

        private void titleList_SelectedValueChanged(object sender, EventArgs e)
        {
            var title = titleList.SelectedItem as Title;
            if (string.IsNullOrEmpty(title?.ID)) return;

            titleName.Text = title.Name;

            //dlcBtn.Enabled = title.DLC.Count > 0;
            //updateBtn.Enabled = title.Versions.Count > 0;

            var updatesStr = title.Versions.Aggregate(string.Empty,
                (current, update) => current + $"| v{update} ");

            SetCurrentImage(title);

            var tvs = title.GetUpdateVersion();
            var l8D = title.Lower8Digits();
            TextLog.StatusLog.WriteLog($"{l8D} | Current Update: v{tvs} | Available Updates: {updatesStr}");
        }

        private void titleList_MouseUp(object sender, MouseEventArgs e)
        {
            var location = titleList.IndexFromPoint(e.Location);
            if (e.Button != MouseButtons.Right)
                return;

            titleList.SelectedIndex = location;
            if (titleList.SelectedItems.Count <= 0)
                return;

            var title = titleList.SelectedItems[0] as Title;
            if (title == null)
                return;

            nameToolStripTextBox1.Text = title.ID;

            var titlePath = Path.Combine(Settings.BasePatchDir, title.Lower8Digits());

            installDLCToolStripMenuItem.Enabled = title.HasDLC;
            uninstallDLCToolStripMenuItem.Enabled = File.Exists(Path.Combine(titlePath, "aoc", "meta", "meta.xml"));

            installUpdateToolStripMenuItem.Enabled = title.Versions.Any();
            uninstallUpdateToolStripMenuItem.Enabled = File.Exists(Path.Combine(titlePath, "meta", "meta.xml"));

            titeListMenuStrip1.Show(MousePosition);
        }

        private async void installDLCToolStripMenuItem_Click(object sender, EventArgs e)
        {
            await DownloadContentClick(dlcBtn, "DLC");
        }

        private void uninstallDLCToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (titleList.SelectedItems.Count <= 0) return;

            foreach (var titleListSelectedItem in titleList.SelectedItems)
            {
                var title = titleListSelectedItem as Title;
                if (title == null) continue;

                var updatePath = Path.Combine(Settings.BasePatchDir, title.Lower8Digits());

                var result = MessageBox.Show(string.Format(Resources.ActionWillDeleteAllContent, updatePath),
                    Resources.PleaseConfirmAction, MessageBoxButtons.OKCancel);

                if (result != DialogResult.OK)
                    return;

                title.DeleteAddOnContent();
            }
        }

        private async void installUpdateToolStripMenuItem_Click(object sender, EventArgs e)
        {
            int ver;
            var version = int.TryParse(titleVersion.Text, out ver) ? ver.ToString() : "0";
            await DownloadContentClick(updateBtn, "Patch", version);
        }

        private void uninstallToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (titleList.SelectedItems.Count <= 0) return;

            foreach (var titleListSelectedItem in titleList.SelectedItems)
            {
                var title = titleListSelectedItem as Title;
                if (title == null) continue;

                var updatePath = Path.Combine(Settings.BasePatchDir, title.Lower8Digits());

                var result = MessageBox.Show(string.Format(Resources.ActionWillDeleteAllContent, updatePath),
                    Resources.PleaseConfirmAction, MessageBoxButtons.OKCancel);

                if (result != DialogResult.OK)
                    return;

                title.DeleteUpdateContent();
            }
        }

        private void deleteTitleToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (titleList.SelectedItems.Count <= 0) return;
            var title = titleList.SelectedItems[0] as Title;
            if (title == null) return;

            if (title.DeleteContent())
                titleList.Items.Remove(title);
        }
    }
}