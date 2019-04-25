#include "mapleseed.h"
#include "ui_mainwindow.h"
#include "versioninfo.h"

MapleSeed::MapleSeed(QWidget* parent)
  : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  this->setWindowTitle("MapleSeed++ " + QString(GEN_VERSION_STRING));
  initialize();
}

MapleSeed::~MapleSeed() {
  delete decrypt;
  delete config;
  delete ui;
  delete downloadManager;
  delete gameLibrary;
}

void MapleSeed::initialize() {
  this->messageLog("Setting up enviornment variables");
  config = new Configuration;
  decrypt = new Decrypt;
  downloadManager = new DownloadManager;
  gameLibrary = new GameLibrary;

  defineActions();
  if (!config->load())
    config->save();

  defaultConfiguration();
  gameLibrary->init(config->getBaseDirectory());
  this->messageLog("Environment setup complete");
}

void MapleSeed::defineActions() {
  connect(decrypt, &Decrypt::log, this, &MapleSeed::messageLog);
  connect(decrypt, &Decrypt::decryptStarted, this, &MapleSeed::disableMenubar);
  connect(decrypt, &Decrypt::decryptFinished, this, &MapleSeed::enableMenubar);
  connect(decrypt, &Decrypt::progressReport, this, &MapleSeed::updateDecryptProgress);

  connect(gameLibrary, &GameLibrary::changed, this, &MapleSeed::updateListview);

  connect(downloadManager, &DownloadManager::log, this, &MapleSeed::messageLog, Qt::ConnectionType::DirectConnection);
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
  connect(ui->listWidget, &QListWidget::itemSelectionChanged, this, &MapleSeed::itemSelectionChanged);
  connect(ui->listWidget, &QListWidget::itemDoubleClicked, this, &MapleSeed::itemDoubleClicked);
  connect(ui->actionConfigTemporary, &QAction::triggered, this, &MapleSeed::actionConfigTemporary);
  connect(ui->actionConfigPersistent, &QAction::triggered, this, &MapleSeed::actionConfigPersistent);
  connect(ui->actionVerbose, &QAction::triggered, this, &MapleSeed::actionVerboseChecked);
  connect(ui->actionIntegrateCemu, &QAction::triggered, this, &MapleSeed::actionIntegrateCemu);
}

void MapleSeed::defaultConfiguration() {
  ui->actionVerbose->setChecked(config->getKeyBool("VerboseLog"));
  ui->actionIntegrateCemu->setChecked(config->getKeyBool("IntegrateCemu"));
  ui->actionConfigTemporary->setChecked(!config->getKeyString("configtype").compare("Temporary"));
  ui->actionConfigPersistent->setChecked(!config->getKeyString("configtype").compare("Persistent"));
}

void MapleSeed::menuQuit() { QApplication::quit(); }

void MapleSeed::actionChange_Library() {
  QDir* dir = this->selectDirectory();
  if (dir == nullptr)
    return;

  config->setBaseDirectory(dir->path());
  ui->listWidget->clear();
  gameLibrary->init(dir->path());

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

  TitleInfo* ti = TitleInfo::DownloadCreate(value, gameLibrary->baseDirectory);
  if (ti == nullptr)
    return;
  auto dir = QString(ti->getDirectory());
  QtConcurrent::run([ = ] { ti->decryptContent(decrypt); });
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

  value.replace(7, 1, 'e');
  TitleInfo* ti = TitleInfo::DownloadCreate(value, gameLibrary->baseDirectory);
  if (ti == nullptr)
    return;
  auto dir = QString(ti->getDirectory());
  QtConcurrent::run([ = ] { ti->decryptContent(decrypt); });
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

  value.replace(7, 1, 'c');
  TitleInfo* ti = TitleInfo::DownloadCreate(value, gameLibrary->baseDirectory);
  if (ti == nullptr)
    return;
  auto dir = QString(ti->getDirectory());
  QtConcurrent::run([ = ] { ti->decryptContent(decrypt); });
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
  QtConcurrent::run([ = ] { decrypt->start(path); });
  delete dir;
}

QDir* MapleSeed::selectDirectory() {
  QFileDialog dialog;
  dialog.setFileMode(QFileDialog::DirectoryOnly);
  dialog.setOption(QFileDialog::ShowDirsOnly);
  if (dialog.exec()) {
    QStringList directories(dialog.selectedFiles());
    return new QDir(directories[0]);
  }
  return nullptr;
}

void MapleSeed::messageLog(QString msg, bool verbose) {
  ui->statusbar->showMessage(msg);
  if (ui->actionVerbose->isChecked() || verbose) {
    ui->textEdit->append(msg);
  }
}

void MapleSeed::disableMenubar() { this->ui->menubar->setEnabled(false); }

void MapleSeed::enableMenubar() { this->ui->menubar->setEnabled(true); }

void MapleSeed::updateListview(TitleInfo* tb) {
  if (ui->listWidget->count() == 1) {
    ui->listWidget->setCurrentRow(0);
  }

  TitleInfoItem* tii = new TitleInfoItem(tb);
  tii->setText(tii->getItem()->getFormatName());
  this->ui->listWidget->addItem(tii);
  this->messageLog("Added to library: " + tb->getFormatName(), true);
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

  this->ui->progressBar->setFormat("%p%   /   " + QString::fromLatin1("%1 %2").arg(speed, 3, 'f', 1).arg(unit));
}

void MapleSeed::updateDecryptProgress(qint64 min, qint64 max) {
  this->ui->progressBar->setValue(static_cast<int>(min));
  this->ui->progressBar->setRange(0, static_cast<int>(max));
  this->ui->progressBar->setFormat("%p%");
}

void MapleSeed::itemSelectionChanged() {
  auto items = ui->listWidget->selectedItems();
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
  auto list = QStringList() << "-g \"" + rpx + "\"";
  cemuProcess = new QProcess(this);
  cemuProcess->setArguments(list);
  cemuProcess->setWorkingDirectory(workingdir);
  cemuProcess->start(file);
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

  if (checked && !QFile(config->getKeyString("IntegrateCemu")).exists()) {
    QFileDialog dialog;
    dialog.setNameFilter("cemu.exe");
    dialog.setFileMode(QFileDialog::ExistingFile);
    if (dialog.exec()) {
      QStringList files(dialog.selectedFiles());
      config->setKey("CemuPath", QFileInfo(files[0]).absoluteFilePath());
    }
  }
}
