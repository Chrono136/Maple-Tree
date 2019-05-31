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
    if (downloadManager) {
        delete downloadManager;
    }
    if (gameLibrary) {
        delete gameLibrary;
    }
    if (config) {
        if (config->decrypt) {
            delete config->decrypt;
        }
        delete config;
    }
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

  gameLibrary->init(config->getBaseDirectory());
  this->messageLog("Environment setup complete");
}

void MapleSeed::defineActions() {
    connect(QtCompressor::self, &QtCompressor::updateProgress, this, &MapleSeed::updateBaiscProgress);

    connect(config->decrypt, &Decrypt::log, this, &MapleSeed::messageLog);
    connect(config->decrypt, &Decrypt::decryptStarted, this, &MapleSeed::disableMenubar);
    connect(config->decrypt, &Decrypt::decryptFinished, this, &MapleSeed::enableMenubar);
    connect(config->decrypt, &Decrypt::progressReport, this, &MapleSeed::updateBaiscProgress);
    connect(config->decrypt, &Decrypt::progressReport2, this, &MapleSeed::updateProgress);

    connect(gameLibrary, &GameLibrary::log, this, &MapleSeed::messageLog);
    connect(gameLibrary, &GameLibrary::progress, this, &MapleSeed::updateBaiscProgress);
    connect(gameLibrary, &GameLibrary::changed, this, &MapleSeed::updateListview);
    connect(gameLibrary, &GameLibrary::addTitle, this, &MapleSeed::updateTitleList);

    connect(downloadManager, &DownloadManager::log, this, &MapleSeed::messageLog);
    connect(downloadManager, &DownloadManager::downloadStarted, this, &MapleSeed::downloadStarted);
    connect(downloadManager, &DownloadManager::downloadStarted, this, &MapleSeed::disableMenubar);
    connect(downloadManager, &DownloadManager::downloadSuccessful, this, &MapleSeed::downloadSuccessful);
    connect(downloadManager, &DownloadManager::downloadSuccessful, this, &MapleSeed::enableMenubar);
    connect(downloadManager, &DownloadManager::downloadError, this, &MapleSeed::downloadError);
    connect(downloadManager, &DownloadManager::downloadError, this, &MapleSeed::enableMenubar);
    connect(downloadManager, &DownloadManager::downloadProgress, this, &MapleSeed::updateDownloadProgress);
}

void MapleSeed::defaultConfiguration() {
  ui->actionVerbose->setChecked(config->getKeyBool("VerboseLog"));
  ui->actionIntegrateCemu->setChecked(config->getKeyBool("IntegrateCemu"));
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
    return QFileInfo();
}

void MapleSeed::CopyToClipboard(QString text)
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(text);
    this->messageLog(text + " copied to clipboard", true);
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

void MapleSeed::SelectionChanged(QListWidget* listWidget) {
    auto items = listWidget->selectedItems();
    if (items.count() <= 0)
        return;

    TitleInfoItem* tii = reinterpret_cast<TitleInfoItem*>(items[0]);
    ui->label->setPixmap(QPixmap(tii->getItem()->titleInfo->getCoverArtPath()));
}

void MapleSeed::showContextMenu(QListWidget* list, const QPoint& pos) {
  QPoint globalPos = list->mapToGlobal(pos);
  if (list->selectedItems().count() == 0) {
    return;
  }
  auto itm = list->selectedItems().first();
  auto tii = reinterpret_cast<TitleInfoItem*>(itm);
  LibraryEntry* entry = tii->getItem();
  TitleInfo* item = entry->titleInfo;
  if (!entry) {
      return;
  }
  QString name(QFileInfo(entry->directory).baseName());
  if (name.isEmpty()) {
      name = item->getName();
  }

  QMenu menu;
  menu.addAction(name, this, [] {})->setEnabled(false);

  menu.addSeparator();
  if (!QFileInfo(entry->metaxml).exists()) {
      menu.addAction("Add Entry", this, [=] { })->setEnabled(false);
      menu.addAction("Delete Entry", this, [=] {
          gameLibrary->database.remove(entry->titleInfo->getID());
          if (gameLibrary->saveDatabase("titlekeys.json")){
              delete ui->titlelistWidget->takeItem(ui->titlelistWidget->row(itm));
          }
      });
      menu.addAction("Modify Entry", this, [=] { })->setEnabled(false);
  }

  menu.addSeparator();
  if (TitleInfo::ValidId(item->getID().replace(7, 1, '0'))) {
      menu.addAction("Download Game", this, [=] { item->download(); });
  }
  if (TitleInfo::ValidId(item->getID().replace(7, 1, 'c'))) {
      menu.addAction("Download DLC", this, [=] { item->downloadDlc(); });
  }
  if (TitleInfo::ValidId(item->getID().replace(7, 1, 'e'))) {
      menu.addAction("Download Patch", this, [=] { item->downloadPatch(); });
  }

  menu.addSeparator();
  if (QFile(QDir(item->getDirectory()).filePath("tmd")).exists() && QFile(QDir(item->getDirectory()).filePath("cetk")).exists())
      menu.addAction("Decrypt Content", this, [=] { QtConcurrent::run([=] {item->decryptContent(); }); });
  menu.addAction("Copy ID to Clipboard", this, [=] { CopyToClipboard(item->getID()); });

  menu.setEnabled(ui->menubar->isEnabled());
  menu.exec(globalPos);
}

void MapleSeed::disableMenubar() {
    ui->menubar->setEnabled(false);
}

void MapleSeed::enableMenubar() {
    ui->menubar->setEnabled(true);
}

void MapleSeed::updateListview(LibraryEntry* entry) {
  TitleInfoItem* tii = new TitleInfoItem(entry);
  tii->setText(tii->getItem()->titleInfo->getFormatName());
  this->ui->listWidget->addItem(tii);
}

void MapleSeed::updateTitleList(LibraryEntry* entry) {
    TitleInfoItem* tii = new TitleInfoItem(entry);
    tii->setText(tii->getItem()->titleInfo->getFormatName());
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

void MapleSeed::on_actionQuit_triggered()
{
    QApplication::quit();
}

void MapleSeed::on_actionChangeLibrary_triggered()
{
    QDir* dir = this->selectDirectory();
    if (dir == nullptr)
      return;
    QString directory(dir->path());
    delete dir;

    ui->listWidget->clear();
    config->setBaseDirectory(directory);
    QtConcurrent::run([=] { gameLibrary->setupLibrary(directory, true); });
}

void MapleSeed::on_actionDecryptContent_triggered()
{
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

    QString path = dir->path();
    delete dir;
    this->messageLog("Decrypt: " + path);
    QtConcurrent::run([ = ] { config->decrypt->start(path); });
}

void MapleSeed::on_actionVerbose_triggered(bool checked)
{
    config->setKeyBool("VerboseLog", checked);
}

void MapleSeed::on_actionIntegrateCemu_triggered(bool checked)
{
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

void MapleSeed::on_actionRefreshLibrary_triggered()
{
    QFile(Configuration::self->getLibPath()).remove();
    ui->listWidget->clear();
    QtConcurrent::run([=] { gameLibrary->setupLibrary(true); });
}

void MapleSeed::on_actionClearSettings_triggered()
{
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

void MapleSeed::on_actionCovertArt_triggered()
{
    QDir directory("covers");
    QString fileName("covers.qta");

    if (!QFile(fileName).exists()) {
        downloadManager->downloadSingle(QUrl("http://pixxy.in/mapleseed/covers.qta"), fileName);
    }

    if (!directory.exists()) {
        QtConcurrent::run([=] { QtCompressor::decompress(fileName, directory.absolutePath()); });
    }
}

void MapleSeed::on_actionCompress_triggered()
{
    QDir* directory(this->selectDirectory());
    if (directory) {
        QString path(directory->absolutePath());
        QtConcurrent::run([=] { QtCompressor::compress(path, path + ".qta"); });
        delete directory;
    }
}

void MapleSeed::on_actionDecompress_triggered()
{
    QFileInfo info(this->selectFile());
    if (info.exists()) {
        QString filename = info.absoluteFilePath();
        QString dir = info.absoluteDir().filePath(info.baseName());
        QtConcurrent::run([=] { QtCompressor::decompress(filename, dir); });
    }
}

void MapleSeed::on_actionDownload_triggered()
{
    bool ok;
    QString value = QInputDialog::getText(this, "Download Content", "Insert title id of desired content below.", QLineEdit::Normal, nullptr, &ok);
    if (!ok){
        return;
    }
    if (value.contains('-')){
        value.remove('-');
    }
    if (value.isEmpty() || value.length() != 16) {
        QMessageBox::information(this, "Download Title Error", "Invalid title id");
        return;
    }
    TitleInfo* titleinfo = TitleInfo::Create(value, gameLibrary->baseDirectory);
    titleinfo->download();
}

void MapleSeed::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    if (item == nullptr || !ui->actionIntegrateCemu->isChecked())
      return;

    auto titleInfoItem = reinterpret_cast<TitleInfoItem*>(item);
    TitleInfo* titleInfo = titleInfoItem->getItem()->titleInfo;
    QString file(config->getKeyString("cemupath"));
    QString workingdir(QFileInfo(file).dir().path());
    QString rpx(titleInfo->getExecutable());
    process = new QProcess(this);
    process->setWorkingDirectory(workingdir);
    process->start(file + " -g \"" + rpx + "\"", QStringList() << " -g \"" + rpx + "\"");
}

void MapleSeed::on_listWidget_itemSelectionChanged()
{
    return SelectionChanged(ui->listWidget);
}

void MapleSeed::on_listWidget_customContextMenuRequested(const QPoint &pos)
{
    return showContextMenu(ui->listWidget, pos);
}

void MapleSeed::on_titlelistWidget_itemSelectionChanged()
{
    return SelectionChanged(ui->titlelistWidget);
}

void MapleSeed::on_titlelistWidget_customContextMenuRequested(const QPoint &pos)
{
    return showContextMenu(ui->titlelistWidget, pos);
}

void MapleSeed::on_searchInput_textEdited(const QString &arg1)
{
    return filter(arg1);
}

void MapleSeed::on_regionBox_currentTextChanged(const QString &arg1)
{
    return filter(arg1);
}
