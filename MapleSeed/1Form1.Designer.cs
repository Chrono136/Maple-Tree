namespace MapleSeed
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.progressBar = new System.Windows.Forms.ProgressBar();
            this.status = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.folderBrowserDialog1 = new System.Windows.Forms.FolderBrowserDialog();
            this.outputTextbox = new System.Windows.Forms.RichTextBox();
            this.storeEncCont = new System.Windows.Forms.CheckBox();
            this.updateBtn = new System.Windows.Forms.Button();
            this.fullScreen = new System.Windows.Forms.CheckBox();
            this.playBtn = new System.Windows.Forms.Button();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.titleList = new System.Windows.Forms.ListBox();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.clearCache = new System.Windows.Forms.Button();
            this.uninstallBtn = new System.Windows.Forms.Button();
            this.serverHub = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.cemuDir = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.titleDir = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.titeListMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.nameToolStripTextBox1 = new System.Windows.Forms.ToolStripTextBox();
            this.installDLCToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.uninstallDLCToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.installUpdateToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.uninstallUpdateToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.deleteTitleToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.cemu173Patch = new System.Windows.Forms.CheckBox();
            this.progressOverlay = new System.Windows.Forms.Label();
            this.dlcBtn = new System.Windows.Forms.Button();
            this.cleanTitleBtn = new System.Windows.Forms.Button();
            this.titleVersion = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.titleIdTextBox = new System.Windows.Forms.TextBox();
            this.newdlbtn = new System.Windows.Forms.Button();
            this.organizeBtn = new System.Windows.Forms.Button();
            this.titleName = new System.Windows.Forms.TextBox();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.tabPage2.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.titeListMenuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // progressBar
            // 
            this.progressBar.Location = new System.Drawing.Point(12, 658);
            this.progressBar.Name = "progressBar";
            this.progressBar.Size = new System.Drawing.Size(1230, 23);
            this.progressBar.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
            this.progressBar.TabIndex = 0;
            // 
            // status
            // 
            this.status.AutoSize = true;
            this.status.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.status.ForeColor = System.Drawing.SystemColors.ControlDarkDark;
            this.status.Location = new System.Drawing.Point(9, 684);
            this.status.Name = "status";
            this.status.Size = new System.Drawing.Size(112, 13);
            this.status.TabIndex = 2;
            this.status.Text = "GitHub.com/Tsumes";
            this.status.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // groupBox2
            // 
            this.groupBox2.Location = new System.Drawing.Point(12, 642);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(1230, 10);
            this.groupBox2.TabIndex = 4;
            this.groupBox2.TabStop = false;
            // 
            // outputTextbox
            // 
            this.outputTextbox.BackColor = System.Drawing.SystemColors.HighlightText;
            this.outputTextbox.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.outputTextbox.ForeColor = System.Drawing.SystemColors.WindowFrame;
            this.outputTextbox.Location = new System.Drawing.Point(370, 6);
            this.outputTextbox.Name = "outputTextbox";
            this.outputTextbox.ReadOnly = true;
            this.outputTextbox.Size = new System.Drawing.Size(504, 480);
            this.outputTextbox.TabIndex = 0;
            this.outputTextbox.Text = "";
            // 
            // storeEncCont
            // 
            this.storeEncCont.AutoSize = true;
            this.storeEncCont.Location = new System.Drawing.Point(133, 619);
            this.storeEncCont.Name = "storeEncCont";
            this.storeEncCont.Size = new System.Drawing.Size(152, 17);
            this.storeEncCont.TabIndex = 11;
            this.storeEncCont.Text = "Store Encrypted Content";
            this.storeEncCont.UseVisualStyleBackColor = true;
            this.storeEncCont.CheckedChanged += new System.EventHandler(this.storeEncCont_CheckedChanged);
            // 
            // updateBtn
            // 
            this.updateBtn.Location = new System.Drawing.Point(91, 32);
            this.updateBtn.Name = "updateBtn";
            this.updateBtn.Size = new System.Drawing.Size(74, 23);
            this.updateBtn.TabIndex = 12;
            this.updateBtn.Text = "[+] Update";
            this.updateBtn.UseVisualStyleBackColor = true;
            this.updateBtn.Click += new System.EventHandler(this.updateBtn_Click);
            // 
            // fullScreen
            // 
            this.fullScreen.AutoSize = true;
            this.fullScreen.Location = new System.Drawing.Point(12, 619);
            this.fullScreen.Name = "fullScreen";
            this.fullScreen.Size = new System.Drawing.Size(115, 17);
            this.fullScreen.TabIndex = 13;
            this.fullScreen.Text = "Full Screen Mode";
            this.fullScreen.UseVisualStyleBackColor = true;
            this.fullScreen.CheckedChanged += new System.EventHandler(this.fullScreen_CheckedChanged);
            // 
            // playBtn
            // 
            this.playBtn.Location = new System.Drawing.Point(12, 7);
            this.playBtn.Name = "playBtn";
            this.playBtn.Size = new System.Drawing.Size(75, 23);
            this.playBtn.TabIndex = 17;
            this.playBtn.Text = "Cemu!";
            this.playBtn.UseVisualStyleBackColor = true;
            this.playBtn.Click += new System.EventHandler(this.playBtn_Click);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Location = new System.Drawing.Point(12, 63);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(1240, 522);
            this.tabControl1.TabIndex = 18;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.titleList);
            this.tabPage1.Controls.Add(this.pictureBox1);
            this.tabPage1.Controls.Add(this.outputTextbox);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(1232, 496);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "Dashboard";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // titleList
            // 
            this.titleList.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.titleList.Font = new System.Drawing.Font("Segoe UI", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.titleList.FormattingEnabled = true;
            this.titleList.ItemHeight = 15;
            this.titleList.Location = new System.Drawing.Point(6, 6);
            this.titleList.Name = "titleList";
            this.titleList.Size = new System.Drawing.Size(358, 482);
            this.titleList.Sorted = true;
            this.titleList.TabIndex = 31;
            this.titleList.SelectedValueChanged += new System.EventHandler(this.titleList_SelectedValueChanged);
            this.titleList.MouseUp += new System.Windows.Forms.MouseEventHandler(this.titleList_MouseUp);
            // 
            // pictureBox1
            // 
            this.pictureBox1.Location = new System.Drawing.Point(880, 6);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(346, 484);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox1.TabIndex = 30;
            this.pictureBox1.TabStop = false;
            this.pictureBox1.Click += new System.EventHandler(this.pictureBox1_Click);
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.groupBox1);
            this.tabPage2.Controls.Add(this.serverHub);
            this.tabPage2.Controls.Add(this.label4);
            this.tabPage2.Controls.Add(this.cemuDir);
            this.tabPage2.Controls.Add(this.label3);
            this.tabPage2.Controls.Add(this.titleDir);
            this.tabPage2.Controls.Add(this.label2);
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(1232, 496);
            this.tabPage2.TabIndex = 3;
            this.tabPage2.Text = "Settings";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.clearCache);
            this.groupBox1.Controls.Add(this.uninstallBtn);
            this.groupBox1.Location = new System.Drawing.Point(9, 6);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(281, 239);
            this.groupBox1.TabIndex = 35;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Actions";
            // 
            // clearCache
            // 
            this.clearCache.Location = new System.Drawing.Point(6, 21);
            this.clearCache.Name = "clearCache";
            this.clearCache.Size = new System.Drawing.Size(80, 25);
            this.clearCache.TabIndex = 34;
            this.clearCache.Text = "Clear Cache";
            this.clearCache.UseVisualStyleBackColor = true;
            this.clearCache.Click += new System.EventHandler(this.clearCache_Click);
            // 
            // uninstallBtn
            // 
            this.uninstallBtn.Location = new System.Drawing.Point(92, 21);
            this.uninstallBtn.Name = "uninstallBtn";
            this.uninstallBtn.Size = new System.Drawing.Size(80, 25);
            this.uninstallBtn.TabIndex = 6;
            this.uninstallBtn.Text = "Uninstall";
            this.uninstallBtn.UseVisualStyleBackColor = true;
            this.uninstallBtn.Click += new System.EventHandler(this.uninstallBtn_Click);
            // 
            // serverHub
            // 
            this.serverHub.Location = new System.Drawing.Point(315, 144);
            this.serverHub.Name = "serverHub";
            this.serverHub.Size = new System.Drawing.Size(174, 22);
            this.serverHub.TabIndex = 5;
            this.serverHub.TextChanged += new System.EventHandler(this.serverHub_TextChanged);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(312, 127);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(63, 13);
            this.label4.TabIndex = 4;
            this.label4.Text = "Server Hub";
            // 
            // cemuDir
            // 
            this.cemuDir.Location = new System.Drawing.Point(315, 88);
            this.cemuDir.Name = "cemuDir";
            this.cemuDir.Size = new System.Drawing.Size(174, 22);
            this.cemuDir.TabIndex = 3;
            this.cemuDir.TextChanged += new System.EventHandler(this.cemuDir_TextChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(312, 71);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(85, 13);
            this.label3.TabIndex = 2;
            this.label3.Text = "Cemu Directory";
            // 
            // titleDir
            // 
            this.titleDir.Location = new System.Drawing.Point(315, 31);
            this.titleDir.Name = "titleDir";
            this.titleDir.Size = new System.Drawing.Size(174, 22);
            this.titleDir.TabIndex = 1;
            this.titleDir.TextChanged += new System.EventHandler(this.titleDir_TextChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(312, 14);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(77, 13);
            this.label2.TabIndex = 0;
            this.label2.Text = "Title Directory";
            // 
            // titeListMenuStrip1
            // 
            this.titeListMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.nameToolStripTextBox1,
            this.installDLCToolStripMenuItem,
            this.uninstallDLCToolStripMenuItem,
            this.installUpdateToolStripMenuItem,
            this.uninstallUpdateToolStripMenuItem,
            this.toolStripSeparator1,
            this.deleteTitleToolStripMenuItem});
            this.titeListMenuStrip1.Name = "titeListMenuStrip1";
            this.titeListMenuStrip1.ShowImageMargin = false;
            this.titeListMenuStrip1.Size = new System.Drawing.Size(137, 142);
            // 
            // nameToolStripTextBox1
            // 
            this.nameToolStripTextBox1.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.nameToolStripTextBox1.Font = new System.Drawing.Font("Segoe UI", 7F, System.Drawing.FontStyle.Bold);
            this.nameToolStripTextBox1.MaxLength = 16;
            this.nameToolStripTextBox1.Name = "nameToolStripTextBox1";
            this.nameToolStripTextBox1.ReadOnly = true;
            this.nameToolStripTextBox1.Size = new System.Drawing.Size(100, 20);
            this.nameToolStripTextBox1.Text = "Title ID";
            this.nameToolStripTextBox1.TextBoxTextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // installDLCToolStripMenuItem
            // 
            this.installDLCToolStripMenuItem.Enabled = false;
            this.installDLCToolStripMenuItem.Name = "installDLCToolStripMenuItem";
            this.installDLCToolStripMenuItem.Size = new System.Drawing.Size(136, 22);
            this.installDLCToolStripMenuItem.Text = "Add DLC";
            this.installDLCToolStripMenuItem.Click += new System.EventHandler(this.installDLCToolStripMenuItem_Click);
            // 
            // uninstallDLCToolStripMenuItem
            // 
            this.uninstallDLCToolStripMenuItem.Enabled = false;
            this.uninstallDLCToolStripMenuItem.Name = "uninstallDLCToolStripMenuItem";
            this.uninstallDLCToolStripMenuItem.Size = new System.Drawing.Size(136, 22);
            this.uninstallDLCToolStripMenuItem.Text = "Remove DLC";
            this.uninstallDLCToolStripMenuItem.Click += new System.EventHandler(this.uninstallDLCToolStripMenuItem_Click);
            // 
            // installUpdateToolStripMenuItem
            // 
            this.installUpdateToolStripMenuItem.Enabled = false;
            this.installUpdateToolStripMenuItem.Name = "installUpdateToolStripMenuItem";
            this.installUpdateToolStripMenuItem.Size = new System.Drawing.Size(136, 22);
            this.installUpdateToolStripMenuItem.Text = "Install Update";
            this.installUpdateToolStripMenuItem.Click += new System.EventHandler(this.installUpdateToolStripMenuItem_Click);
            // 
            // uninstallUpdateToolStripMenuItem
            // 
            this.uninstallUpdateToolStripMenuItem.Name = "uninstallUpdateToolStripMenuItem";
            this.uninstallUpdateToolStripMenuItem.Size = new System.Drawing.Size(136, 22);
            this.uninstallUpdateToolStripMenuItem.Text = "Uninstall Update";
            this.uninstallUpdateToolStripMenuItem.Click += new System.EventHandler(this.uninstallToolStripMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(133, 6);
            // 
            // deleteTitleToolStripMenuItem
            // 
            this.deleteTitleToolStripMenuItem.Name = "deleteTitleToolStripMenuItem";
            this.deleteTitleToolStripMenuItem.Size = new System.Drawing.Size(136, 22);
            this.deleteTitleToolStripMenuItem.Text = "Delete Title";
            this.deleteTitleToolStripMenuItem.Click += new System.EventHandler(this.deleteTitleToolStripMenuItem_Click);
            // 
            // cemu173Patch
            // 
            this.cemu173Patch.AutoSize = true;
            this.cemu173Patch.Checked = true;
            this.cemu173Patch.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cemu173Patch.Location = new System.Drawing.Point(291, 619);
            this.cemu173Patch.Name = "cemu173Patch";
            this.cemu173Patch.Size = new System.Drawing.Size(158, 17);
            this.cemu173Patch.TabIndex = 7;
            this.cemu173Patch.Text = "Cemu 1.7.3 Patch Support";
            this.cemu173Patch.UseVisualStyleBackColor = true;
            this.cemu173Patch.CheckedChanged += new System.EventHandler(this.cemu173Patch_CheckedChanged);
            // 
            // progressOverlay
            // 
            this.progressOverlay.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            this.progressOverlay.AutoSize = true;
            this.progressOverlay.BackColor = System.Drawing.Color.Transparent;
            this.progressOverlay.Location = new System.Drawing.Point(9, 639);
            this.progressOverlay.Name = "progressOverlay";
            this.progressOverlay.Size = new System.Drawing.Size(89, 13);
            this.progressOverlay.TabIndex = 23;
            this.progressOverlay.Text = "0 bytes / 0 bytes";
            // 
            // dlcBtn
            // 
            this.dlcBtn.Location = new System.Drawing.Point(12, 32);
            this.dlcBtn.Name = "dlcBtn";
            this.dlcBtn.Size = new System.Drawing.Size(75, 23);
            this.dlcBtn.TabIndex = 25;
            this.dlcBtn.Text = "[+] DLC";
            this.dlcBtn.UseVisualStyleBackColor = true;
            this.dlcBtn.Click += new System.EventHandler(this.dlcBtn_Click);
            // 
            // cleanTitleBtn
            // 
            this.cleanTitleBtn.Location = new System.Drawing.Point(171, 7);
            this.cleanTitleBtn.Name = "cleanTitleBtn";
            this.cleanTitleBtn.Size = new System.Drawing.Size(74, 23);
            this.cleanTitleBtn.TabIndex = 26;
            this.cleanTitleBtn.Text = "Clean Title";
            this.cleanTitleBtn.UseVisualStyleBackColor = true;
            this.cleanTitleBtn.Click += new System.EventHandler(this.cleanTitleBtn_Click);
            // 
            // titleVersion
            // 
            this.titleVersion.Location = new System.Drawing.Point(189, 32);
            this.titleVersion.MaxLength = 3;
            this.titleVersion.Name = "titleVersion";
            this.titleVersion.Size = new System.Drawing.Size(39, 22);
            this.titleVersion.TabIndex = 27;
            this.titleVersion.Text = "0";
            this.titleVersion.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Segoe UI", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(171, 35);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(16, 19);
            this.label1.TabIndex = 28;
            this.label1.Text = "v";
            // 
            // titleIdTextBox
            // 
            this.titleIdTextBox.BackColor = System.Drawing.SystemColors.ScrollBar;
            this.titleIdTextBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.titleIdTextBox.Location = new System.Drawing.Point(531, 24);
            this.titleIdTextBox.MaxLength = 16;
            this.titleIdTextBox.Name = "titleIdTextBox";
            this.titleIdTextBox.Size = new System.Drawing.Size(214, 15);
            this.titleIdTextBox.TabIndex = 30;
            this.titleIdTextBox.Text = "0005000000000000";
            this.titleIdTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.titleIdTextBox.TextChanged += new System.EventHandler(this.titleIdTextBox_TextChanged);
            // 
            // newdlbtn
            // 
            this.newdlbtn.Location = new System.Drawing.Point(251, 7);
            this.newdlbtn.Name = "newdlbtn";
            this.newdlbtn.Size = new System.Drawing.Size(74, 23);
            this.newdlbtn.TabIndex = 31;
            this.newdlbtn.Text = "[+] Title";
            this.newdlbtn.UseVisualStyleBackColor = true;
            this.newdlbtn.Click += new System.EventHandler(this.newdlbtn_Click);
            // 
            // organizeBtn
            // 
            this.organizeBtn.Location = new System.Drawing.Point(91, 7);
            this.organizeBtn.Name = "organizeBtn";
            this.organizeBtn.Size = new System.Drawing.Size(74, 23);
            this.organizeBtn.TabIndex = 33;
            this.organizeBtn.Text = "Organize";
            this.organizeBtn.UseVisualStyleBackColor = true;
            this.organizeBtn.Click += new System.EventHandler(this.organizeBtn_Click);
            // 
            // titleName
            // 
            this.titleName.BackColor = System.Drawing.SystemColors.Menu;
            this.titleName.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.titleName.Font = new System.Drawing.Font("Segoe UI", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.titleName.Location = new System.Drawing.Point(386, 47);
            this.titleName.MaxLength = 16;
            this.titleName.Name = "titleName";
            this.titleName.Size = new System.Drawing.Size(504, 16);
            this.titleName.TabIndex = 35;
            this.titleName.Text = "Title ID";
            this.titleName.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1264, 701);
            this.Controls.Add(this.progressOverlay);
            this.Controls.Add(this.titleName);
            this.Controls.Add(this.organizeBtn);
            this.Controls.Add(this.newdlbtn);
            this.Controls.Add(this.titleIdTextBox);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.titleVersion);
            this.Controls.Add(this.cleanTitleBtn);
            this.Controls.Add(this.dlcBtn);
            this.Controls.Add(this.cemu173Patch);
            this.Controls.Add(this.tabControl1);
            this.Controls.Add(this.playBtn);
            this.Controls.Add(this.fullScreen);
            this.Controls.Add(this.updateBtn);
            this.Controls.Add(this.storeEncCont);
            this.Controls.Add(this.status);
            this.Controls.Add(this.progressBar);
            this.Controls.Add(this.groupBox2);
            this.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "Form1";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Maple Seed";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.tabPage2.ResumeLayout(false);
            this.tabPage2.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.titeListMenuStrip1.ResumeLayout(false);
            this.titeListMenuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        internal System.Windows.Forms.ProgressBar progressBar;
        private System.Windows.Forms.Label status;
        private System.Windows.Forms.GroupBox groupBox2;
        internal System.Windows.Forms.FolderBrowserDialog folderBrowserDialog1;
        private System.Windows.Forms.RichTextBox outputTextbox;
        internal System.Windows.Forms.CheckBox storeEncCont;
        private System.Windows.Forms.Button updateBtn;
        private System.Windows.Forms.CheckBox fullScreen;
        private System.Windows.Forms.Button playBtn;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.Label progressOverlay;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.TextBox cemuDir;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox titleDir;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox serverHub;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.CheckBox cemu173Patch;
        private System.Windows.Forms.Button dlcBtn;
        private System.Windows.Forms.Button cleanTitleBtn;
        private System.Windows.Forms.TextBox titleVersion;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.TextBox titleIdTextBox;
        private System.Windows.Forms.Button newdlbtn;
        private System.Windows.Forms.ToolStripMenuItem uninstallUpdateToolStripMenuItem;
        private System.Windows.Forms.Button organizeBtn;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button clearCache;
        private System.Windows.Forms.ToolStripMenuItem installUpdateToolStripMenuItem;
        public System.Windows.Forms.ContextMenuStrip titeListMenuStrip1;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem installDLCToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem uninstallDLCToolStripMenuItem;
        private System.Windows.Forms.ToolStripTextBox nameToolStripTextBox1;
        private System.Windows.Forms.ToolStripMenuItem deleteTitleToolStripMenuItem;
        private System.Windows.Forms.ListBox titleList;
        private System.Windows.Forms.TextBox titleName;
        private System.Windows.Forms.Button uninstallBtn;
    }
}

