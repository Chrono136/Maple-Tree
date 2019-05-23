#include "mapleseed.h"
#include "ui_mainwindow.h"
#include "versioninfo.h"

MapleSeed* MapleSeed::self;

MapleSeed::MapleSeed(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(self = this);
  this->setWindowTitle("MapleSeed++ " + QString(GEN_VERSION_STRING));
  initialize();
}

MapleSeed::~MapleSeed() {
  if (downloadManager)
    delete downloadManager;
  if (config->decrypt)
    delete config->decrypt;
  if (gameLibrary)
    delete gameLibrary;
  if (config)
    delete config;
  delete ui;
}

void MapleSeed::initialize() {
  this->messageLog("Setting up enviornment variables");
  QtCompressor::self = new QtCompressor;
  
  defineActions();
  if (!config->load()) {
    config->save();
  }
  defaultConfiguration();

  gameLibrary->init(config->getBaseDirectory(), ui->actionOffline_Mode->isChecked());
  this->messageLog("Environment setup complete");
}

void MapleSeed::defineActions() {
	connect(QtCompressor::self, &QtCompressor::updateProgress, this, &MapleSeed::updateBaiscProgress);

	connect(config->decrypt, &Decrypt::log, this, &MapleSeed::messageLog);
	connect(config->decrypt, &Decrypt::decryptStarted, this, &MapleSeed::disableMenubar);
	connect(config->decrypt, &Decrypt::decryptFinished, this, &MapleSeed::enableMenubar);
	connect(config->decrypt, &Decrypt::progressReport, this, &MapleSeed::updateBaiscProgress);
	connect(config->decrypt, &Decrypt::progressReport2, this, &MapleSeed::updateProgress);

	connect(gameLibrary, &GameLibrary::progress, this, &MapleSeed::updateBaiscProgress);
	connect(gameLibrary, &GameLibrary::changed, this, &MapleSeed::updateListview);
	connect(gameLibrary, &GameLibrary::addTitle, this, &MapleSeed::updateTitleList);
	connect(gameLibrary, &GameLibrary::log, this, &MapleSeed::messageLog);
	connect(ui->listWidget, &QListWidget::customContextMenuRequested, this, &MapleSeed::showContextMenuLibrary);
	connect(ui->listWidget, &QListWidget::itemSelectionChanged, this, &MapleSeed::itemSelectionChanged);
	connect(ui->listWidget, &QListWidget::itemDoubleClicked, this, &MapleSeed::itemDoubleClicked);
	connect(ui->titlelistWidget, &QListWidget::itemSelectionChanged, this, &MapleSeed::TitleSelectionChanged);
	connect(ui->titlelistWidget, &QListWidget::customContextMenuRequested, this, &MapleSeed::showContextMenuTitles);
	connect(ui->searchInput, &QLineEdit::textEdited, this, &MapleSeed::filter);
	connect(ui->regionBox, &QComboBox::currentTextChanged, this, &MapleSeed::filter);

	connect(downloadManager, &DownloadManager::log, this, &MapleSeed::messageLog);
	connect(downloadManager, &DownloadManager::downloadStarted, this, &MapleSeed::downloadStarted);
	connect(downloadManager, &DownloadManager::downloadStarted, this, &MapleSeed::disableMenubar);
	connect(downloadManager, &DownloadManager::downloadSuccessful, this, &MapleSeed::downloadSuccessful);
	connect(downloadManager, &DownloadManager::downloadSuccessful, this, &MapleSeed::enableMenubar);
	connect(downloadManager, &DownloadManager::downloadError, this, &MapleSeed::downloadError);
	connect(downloadManager, &DownloadManager::downloadError, this, &MapleSeed::enableMenubar);
	connect(downloadManager, &DownloadManager::downloadProgress, this, &MapleSeed::updateDownloadProgress);

	connect(ui->actionQuit, &QAction::triggered, this, &MapleSeed::menuQuit);
	connect(ui->actionChange_Library, &QAction::triggered, this, &MapleSeed::actionChange_Library);
	connect(ui->actionDownload_Title, &QAction::triggered, this, &MapleSeed::actionDownload_Title);
	connect(ui->actionUpdate, &QAction::triggered, this, &MapleSeed::actionUpdate);
	connect(ui->actionDLC, &QAction::triggered, this, &MapleSeed::actionDLC);
	connect(ui->actionDecrypt_Content, &QAction::triggered, this, &MapleSeed::decryptContent);
	connect(ui->actionConfigTemporary, &QAction::triggered, this, &MapleSeed::actionConfigTemporary);
	connect(ui->actionConfigPersistent, &QAction::triggered, this, &MapleSeed::actionConfigPersistent);
	connect(ui->actionVerbose, &QAction::triggered, this, &MapleSeed::actionVerboseChecked);
	connect(ui->actionIntegrateCemu, &QAction::triggered, this, &MapleSeed::actionIntegrateCemu);
	connect(ui->actionRefreshLibrary, &QAction::triggered, this, &MapleSeed::actionRefreshLibrary);
	connect(ui->actionOffline_Mode, &QAction::triggered, this, &MapleSeed::actionOffline_Mode);
	connect(ui->actionClear_Settings, &QAction::triggered, this, &MapleSeed::actionClear_Settings);
	connect(ui->actionCovertArt, &QAction::triggered, this, &MapleSeed::actionCovertArt);
	connect(ui->actionCompress, &QAction::triggered, this, &MapleSeed::actionCompress);
	connect(ui->actionDecompress, &QAction::triggered, this, &MapleSeed::actionDecompress);
}

void MapleSeed::defaultConfiguration() {
  ui->actionVerbose->setChecked(config->getKeyBool("VerboseLog"));
  ui->actionIntegrateCemu->setChecked(config->getKeyBool("IntegrateCemu"));
  ui->actionOffline_Mode->setChecked(config->getKeyBool("Offline"));
  ui->actionConfigTemporary->setChecked(!config->getKeyString("configtype").compare("Temporary"));
  ui->actionConfigPersistent->setChecked(!config->getKeyString("configtype").compare("Persistent"));
}

void MapleSeed::menuQuit() { QApplication::quit(); }

void MapleSeed::actionChange_Library() {
  QDir* dir = this->selectDirectory();
  if (dir == nullptr)
    return;

  ui->listWidget->clear();
  config->setBaseDirectory(dir->path());
  gameLibrary->init(dir->path(), false);

  if (ui->listWidget->count() > 0) {
    auto item = ui->listWidget->item(0);
    TitleInfoItem* itm = reinterpret_cast<TitleInfoItem*>(item);
    ui->label->setPixmap(QPixmap(itm->getItem()->getCoverArtPath()));
  }

  this->messageLog("Game library has been updated to: " + dir->path());
  delete dir;
}

void MapleSeed::actionDownload_Title() {
  bool ok;
  QString value = QInputDialog::getText(this, tr("Download Title"), tr("Title ID:"), QLineEdit::Normal, nullptr, &ok);
  if (!ok)
    return;

  if (value.isEmpty() || value.count() != 16) {
    QMessageBox::information(this, "Download Title Error", "Invalid title id. Please verify your title id is 16 characters");
    return;
  }

  TitleInfo::DownloadCreate(value, gameLibrary->baseDirectory);
}

void MapleSeed::actionUpdate() {
  bool ok;
  QString value = QInputDialog::getText(this, tr("Download Update"), tr("Content ID:"), QLineEdit::Normal, nullptr, &ok);
  if (!ok)
    return;

  if (value.isEmpty() || value.count() != 16) {
    QMessageBox::information(this, "Download Update Error", "Invalid content id. Please verify your content id is 16 characters");
    return;
  }

  TitleInfo::DownloadCreate(value.replace(7, 1, 'e'), gameLibrary->baseDirectory);
}

void MapleSeed::actionDLC() {
  bool ok;
  QString value = QInputDialog::getText(this, tr("Download DLC"), tr("Content ID:"), QLineEdit::Normal, nullptr, &ok);
  if (!ok)
    return;

  if (value.isEmpty() || value.count() != 16) {
    QMessageBox::information(this, "Download DLC Error", "Invalid content id. Please verify your content id is 16 characters");
    return;
  }

  TitleInfo::DownloadCreate(value.replace(7, 1, 'c'), gameLibrary->baseDirectory);
}

void MapleSeed::decryptContent() {
  QDir* dir = this->selectDirectory();
  if (dir == nullptr)
    return;

  if (!QFileInfo(dir->filePath("tmd")).exists()) {
    QMessageBox::critical(this, "Missing file", +"Missing: " + dir->filePath("/tmd"));
    return;
  }
  if (!QFileInfo(dir->filePath("cetk")).exists()) {
    QMessageBox::critical(this, "Missing file", +"Missing: " + dir->filePath("/cetk"));
    return;
  }

  auto path = dir->path();
  this->messageLog("Decrypt: " + path);
  QtConcurrent::run([ = ] { config->decrypt->start(path); });
  delete dir;
}

QDir* MapleSeed::selectDirectory() {
  QFileDialog dialog;
  dialog.setFileMode(QFileDialog::DirectoryOnly);
  dialog.setOption(QFileDialog::ShowDirsOnly);
  if (dialog.exec()) {
    QStringList entries(dialog.selectedFiles());
    return new QDir(entries.first());
  }
  return nullptr;
}

QFileInfo MapleSeed::selectFile()
{
	QFileDialog dialog;
	dialog.setNameFilter("*.qta");
	if (dialog.exec()) {
		QStringList entries(dialog.selectedFiles());
		return entries.first();
	}
	return nullptr;
}

void MapleSeed::messageLog(QString msg, bool verbose) {
  ui->statusbar->showMessage(msg);
  if (ui->actionVerbose->isChecked() || verbose) {
    ui->textEdit->append(msg);
  }

  if (ui->actionVerbose->isChecked()) {
    QFile file(QCoreApplication::applicationName() + ".log");
    if (!file.open(QIODevice::Append)) {
      qWarning("Couldn't open file.");
      return;
    }
    QString log(QDateTime::currentDateTime().toString("[MMM dd, yyyy HH:mm:ss ap] ") + msg + "\n");
    file.write(log.toLatin1());
    file.close();
  }
}

void MapleSeed::showContextMenuLibrary(const QPoint& pos)
{
	showContextMenu(ui->listWidget, pos);
}

void MapleSeed::showContextMenuTitles(const QPoint& pos)
{
	showContextMenu(ui->titlelistWidget, pos);
}

void MapleSeed::showContextMenu(QListWidget* list, const QPoint& pos) {
  QPoint globalPos = list->mapToGlobal(pos);
  if (list->selectedItems().count() == 0) {
    return;
  }
  auto itm = list->selectedItems().first();

  auto tii = reinterpret_cast<TitleInfoItem*>(itm);
  TitleInfo* item = tii->getItem();

  QMenu menu;
  menu.addAction(item->getName(), this, [ = ] {})->setEnabled(false);

  menu.addSeparator();
  menu.addAction("Download Game", this, [=] { item->download(); });
  if (TitleInfo::ValidId(item->getID().replace(7, 1, 'c'))) {
	  menu.addAction("Download DLC", this, [=] { item->downloadDlc(); });
  }
  if (TitleInfo::ValidId(item->getID().replace(7, 1, 'e'))) {
	  menu.addAction("Download Patch", this, [=] { item->downloadPatch(); });
  }

  menu.addSeparator();
  if (QFile(QDir(item->getDirectory()).filePath("tmd")).exists() && QFile(QDir(item->getDirectory()).filePath("cetk")).exists())
	  menu.addAction("Decrypt Content", this, [=] { QtConcurrent::run([=] {item->decryptContent(); }); });
  menu.addAction("Copy ID to Clipboard", this, [=] {
	  QClipboard* clipboard = QApplication::clipboard();
	  QString id(item->getID());
	  clipboard->setText(id);
	  this->messageLog(id + " copied to clipboard");
	  });

  menu.exec(globalPos);
}

void MapleSeed::disableMenubar() { this->ui->menubar->setEnabled(false); }

void MapleSeed::enableMenubar() { this->ui->menubar->setEnabled(true); }

void MapleSeed::updateListview(LibraryEntry* entry) {
  if (ui->listWidget->count() == 1) {
    ui->listWidget->setCurrentRow(0);
  }
  TitleInfoItem* tii = new TitleInfoItem(entry->titleInfo);
  tii->setText(tii->getItem()->getFormatName());
  this->ui->listWidget->addItem(tii);
}

void MapleSeed::updateTitleList(LibraryEntry* entry) {
	if (ui->titlelistWidget->count() == 1) {
		ui->titlelistWidget->setCurrentRow(0);
	}
	TitleInfoItem* tii = new TitleInfoItem(entry->titleInfo);
	tii->setText(tii->getItem()->getFormatName());
	this->ui->titlelistWidget->addItem(tii);
}

void MapleSeed::downloadStarted(QString filename) {
  this->messageLog("Downloading: " + filename);
}

void MapleSeed::downloadSuccessful(QString fileName) {
  this->ui->progressBar->setValue(0);
  this->ui->progressBar->setFormat("%p%");
  this->messageLog("Download successful: " + fileName);
}

void MapleSeed::downloadError(QString errorString) {
  this->ui->menubar->setEnabled(true);
  this->messageLog(errorString, true);
}

void MapleSeed::updateDownloadProgress(qint64 bytesReceived, qint64 bytesTotal, QTime qtime) {
  this->ui->progressBar->setRange(0, static_cast<int>(bytesTotal));
  this->ui->progressBar->setValue(static_cast<int>(bytesReceived));

  double speed = bytesReceived * 1000.0 / qtime.elapsed();
  QString unit;
  if (speed < 1024) {
    unit = "bytes/sec";
  } else if (speed < 1024 * 1024) {
    speed /= 1024;
    unit = "kB/s";
  } else {
    speed /= 1024 * 1024;
    unit = "MB/s";
  }

  this->ui->progressBar->setFormat("%p% " + 
	  config->size_human(bytesReceived) + " / " + config->size_human(bytesTotal) + " | " +
	  QString::fromLatin1("%1 %2").arg(speed, 3, 'f', 1).arg(unit));
}

void MapleSeed::updateProgress(qint64 min, qint64 max, int curfile, int maxfiles) {
	float per = (static_cast<float>(min) / static_cast<float>(max)) * 100;
	this->ui->progressBar->setRange(0, static_cast<int>(100));
	this->ui->progressBar->setValue(static_cast<int>(per));
	this->ui->progressBar->setFormat(QString::number(per, 'G', 3) + "% " + 
		config->size_human(min) + " / " + config->size_human(max) + " | " +
		QString::number(curfile) + " / " + QString::number(maxfiles) + " files");
}

void MapleSeed::updateBaiscProgress(qint64 min, qint64 max) {
  this->ui->progressBar->setRange(0, static_cast<int>(max));
  this->ui->progressBar->setValue(static_cast<int>(min));
  this->ui->progressBar->setFormat("%p% / %v of %m");
}

void MapleSeed::itemSelectionChanged() {
  auto items = ui->listWidget->selectedItems();
  if (items.count() <= 0)
    return;

  TitleInfoItem* tii = reinterpret_cast<TitleInfoItem*>(items[0]);
  ui->label->setPixmap(QPixmap(tii->getItem()->getCoverArtPath()));
}

void MapleSeed::TitleSelectionChanged() {
	auto items = ui->titlelistWidget->selectedItems();
	if (items.count() <= 0)
		return;

	TitleInfoItem* tii = reinterpret_cast<TitleInfoItem*>(items[0]);
	ui->label->setPixmap(QPixmap(tii->getItem()->getCoverArtPath()));
}

void MapleSeed::itemDoubleClicked(QListWidgetItem* itm) {
  if (itm == nullptr || !ui->actionIntegrateCemu->isChecked())
    return;

  auto titleInfoItem = reinterpret_cast<TitleInfoItem*>(itm);
  TitleInfo* item = titleInfoItem->getItem();
  QString file(config->getKeyString("cemupath"));
  QString workingdir(QFileInfo(file).dir().path());
  QString rpx(item->getExecutable());
  process = new QProcess(this);
  process->setWorkingDirectory(workingdir);
  process->start(file + " -g \"" + rpx + "\"", QStringList() << "-g \"" + rpx + "\"");
}

void MapleSeed::actionConfigTemporary(bool checked) {
  config->setKey("ConfigType", QString("Temporary"));
  ui->actionConfigPersistent->setChecked(!checked);
}

void MapleSeed::actionConfigPersistent(bool checked) {
  config->setKey("ConfigType", QString("Persistent"));
  ui->actionConfigTemporary->setChecked(!checked);
}

void MapleSeed::actionVerboseChecked(bool checked) {
  config->setKeyBool("VerboseLog", checked);
}

void MapleSeed::actionIntegrateCemu(bool checked) {
  config->setKeyBool("IntegrateCemu", checked);
  QString cemulocation(config->getKeyString("cemupath"));

  if (checked && !QFile(cemulocation).exists()) {
    QFileDialog dialog;
    dialog.setNameFilter("cemu.exe");
    dialog.setFileMode(QFileDialog::ExistingFile);
    if (dialog.exec()) {
      QStringList files(dialog.selectedFiles());
      config->setKey("CemuPath", QFileInfo(files[0]).absoluteFilePath());
    }
  }
}

void MapleSeed::actionRefreshLibrary() {
  QFile(Configuration::self->getLibPath()).remove();
  ui->listWidget->clear();
  gameLibrary->init(gameLibrary->baseDirectory, false);
}

void MapleSeed::actionOffline_Mode(bool checked) {
  config->setKeyBool("Offline", checked);
  gameLibrary->offline(checked);
}

void MapleSeed::actionClear_Settings() {
  auto reply = QMessageBox::information(this, "Warning!!!", "Do you want to delete all settings and temporary files?\nThis application will close.", QMessageBox::Yes | QMessageBox::No);
  if (reply == QMessageBox::Yes) {
    QDir dir(config->getPersistentDirectory());
    delete gameLibrary;
    gameLibrary = nullptr;
    delete config;
    config = nullptr;
    dir.removeRecursively();
    QApplication::quit();
  }
}

void MapleSeed::actionCovertArt() {
	QDir directory("covers");
	QString fileName("covers.qta");

	if (!QFile(fileName).exists()) {
		downloadManager->downloadSingle(QUrl("http://pixxy.in/mapleseed/covers.qta"), fileName);
	}

	if (!directory.exists()) {
		QtConcurrent::run([=] {
			QtCompressor::decompress(fileName, directory.absolutePath());
			itemSelectionChanged();
			});
	}
}

void MapleSeed::filter(QString filter_string)
{
	for (int row(0); row < ui->titlelistWidget->count(); row++)
		ui->titlelistWidget->item(row)->setHidden(true);

	QString searchString;
	QList<QListWidgetItem*> matches;

	if (filter_string == ui->regionBox->currentText()) {
		searchString.append("*" + ui->regionBox->currentText() + "*");
	}
	else {
		searchString.append("*" + ui->regionBox->currentText() + "*" + filter_string);
	}
	searchString.append("*");
	matches.append(ui->titlelistWidget->findItems(searchString, Qt::MatchFlag::MatchWildcard));

	for (QListWidgetItem* item : matches)
		item->setHidden(false);
}

void MapleSeed::actionCompress()
{
	QDir* directory(this->selectDirectory());
	QString path(directory->absolutePath());
	QtConcurrent::run([=] { QtCompressor::compress(path, path + ".qta"); });
	delete directory;
}

void MapleSeed::actionDecompress()
{
	QFileInfo info(this->selectFile());
	QString filename = info.absoluteFilePath();
	QString dir = info.absoluteDir().filePath(info.baseName());
	QtConcurrent::run([=] { QtCompressor::decompress(filename, dir); });
}
