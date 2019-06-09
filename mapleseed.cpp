#include "mapleseed.h"
#include "ui_mainwindow.h"
#include "versioninfo.h"

MapleSeed* MapleSeed::self;

MapleSeed::MapleSeed(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(self = this);
    this->setWindowTitle("MapleSeed++ " + QString(GEN_VERSION_STRING));
    initialize();
}

MapleSeed::~MapleSeed()
{
    Gamepad::terminate();
    if (downloadManager)
    {
        delete downloadManager;
    }
    if (gameLibrary)
    {
        delete gameLibrary;
    }
    if (config)
    {
        delete config;
    }
    delete process;
    delete ui;
}

void MapleSeed::initialize()
{
    qInfo() << "Setting up enviornment variables";
    QtCompressor::self = new QtCompressor;

    defineActions();
    if (!config->load()) {
      config->save();
    }
    defaultConfiguration();

    gameLibrary->init(config->getBaseDirectory());
    on_actionGamepad_triggered(config->getKeyBool("Gamepad"));
    qInfo() << "Environment setup complete";
}

void MapleSeed::defineActions()
{
    connect(QtCompressor::self, &QtCompressor::updateProgress, this, &MapleSeed::updateBaiscProgress);

    connect(config->decrypt, &Decrypt::decryptStarted, this, &MapleSeed::disableMenubar);
    connect(config->decrypt, &Decrypt::decryptFinished, this, &MapleSeed::enableMenubar);
    connect(config->decrypt, &Decrypt::progressReport, this, &MapleSeed::updateBaiscProgress);
    connect(config->decrypt, &Decrypt::progressReport2, this, &MapleSeed::updateProgress);

    connect(gameLibrary, &GameLibrary::progress, this, &MapleSeed::updateBaiscProgress);
    connect(gameLibrary, &GameLibrary::changed, this, &MapleSeed::updateListview);
    connect(gameLibrary, &GameLibrary::addTitle, this, &MapleSeed::updateTitleList);
    connect(gameLibrary, &GameLibrary::loadComplete, this, &MapleSeed::gameLibraryLoadComplete);

    connect(downloadManager, &DownloadManager::downloadStarted, this, &MapleSeed::downloadStarted);
    connect(downloadManager, &DownloadManager::downloadStarted, this, &MapleSeed::disableMenubar);
    connect(downloadManager, &DownloadManager::downloadSuccessful, this, &MapleSeed::downloadSuccessful);
    connect(downloadManager, &DownloadManager::downloadSuccessful, this, &MapleSeed::enableMenubar);
    connect(downloadManager, &DownloadManager::downloadError, this, &MapleSeed::downloadError);
    connect(downloadManager, &DownloadManager::downloadError, this, &MapleSeed::enableMenubar);
    connect(downloadManager, &DownloadManager::downloadProgress, this, &MapleSeed::updateDownloadProgress);
}

void MapleSeed::defaultConfiguration()
{
    ui->actionVerbose->setChecked(config->getKeyBool("VerboseLog"));
    ui->actionIntegrateCemu->setChecked(config->getKeyBool("IntegrateCemu"));
    ui->checkBoxEShopTitles->setChecked(config->getKeyBool("eShopTitles"));
    ui->actionGamepad->setChecked(Gamepad::isEnabled = config->getKeyBool("Gamepad"));
    ui->actionDebug->setChecked(Debug::debugEnabled = config->getKeyBool("DebugLogging"));
}

QDir* MapleSeed::selectDirectory()
{
  QFileDialog dialog;
  dialog.setFileMode(QFileDialog::DirectoryOnly);
  dialog.setOption(QFileDialog::ShowDirsOnly);
  if (dialog.exec()) {
    QStringList entries(dialog.selectedFiles());
    return new QDir(entries.first());
  }
  return nullptr;
}

QFileInfo MapleSeed::selectFile(QString defaultDir)
{
    QFileDialog dialog;
    if (QDir(defaultDir).exists()){
        dialog.setDirectory(defaultDir);
    }
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
    qInfo() << text << "copied to clipboard";
}

void MapleSeed::executeCemu(QString rpxPath)
{
    QFileInfo rpx(rpxPath);
    if (rpx.exists()){
        QString file(config->getKeyString("cemupath"));
        process->setWorkingDirectory(QFileInfo(file).dir().path());
        process->setNativeArguments("-g \"" + rpx.filePath() + "\"");
        process->setProgram(file);
        process->start();
    }
}

bool MapleSeed::processActive()
{
    if (process->state() == process->NotRunning)
    {
        return false;
    }
    return true;
}

void MapleSeed::gameUp(bool pressed)
{
    if (!pressed || processActive()) return;
    qDebug() << "row up";

    auto row = ui->listWidget->currentRow();
    if (ui->listWidget->currentRow() == 0)
    {
        row = ui->listWidget->count()-1;
    }
    else {
        row -= 1;
    }
    ui->listWidget->setCurrentRow(row);
}

void MapleSeed::gameDown(bool pressed)
{
    if (!pressed || processActive()) return;
    qDebug() << "row down";

    auto row = ui->listWidget->currentRow();
    if (ui->listWidget->currentRow() == ui->listWidget->count()-1)
    {
        row = 0;
    }
    else {
        row += 1;
    }
    ui->listWidget->setCurrentRow(row);
}

void MapleSeed::gameStart(bool pressed)
{
    if (!pressed || processActive()) return;
    qDebug() << "game start";

    auto item = ui->listWidget->selectedItems().first();
    auto titleInfoItem = reinterpret_cast<TitleInfoItem*>(item);
    if (titleInfoItem->getItem())
    {
        executeCemu(titleInfoItem->getItem()->rpx);
    }
}

void MapleSeed::gameClose(bool pressed)
{
    if (!pressed || !processActive()) return;

    process->terminate();
}

void MapleSeed::messageLog(QString msg)
{
    if (config && config->getKeyBool("VerboseLog"))
    {
        if (mutex.tryLock(100))
        {
            if (ui && ui->statusbar)
            {
                ui->statusbar->showMessage(msg);
            }
            QFile file(QCoreApplication::applicationName() + ".log");
            if (!file.open(QIODevice::Append))
            {
              qWarning("Couldn't open file.");
              return;
            }
            QString log(QDateTime::currentDateTime().toString("[MMM dd, yyyy HH:mm:ss ap] ") + msg + "\n");
            file.write(log.toLatin1());
            file.close();
            mutex.unlock();
        }
    }
}

void MapleSeed::gameLibraryLoadComplete()
{
    on_checkBoxEShopTitles_stateChanged(config->getKeyBool("eShopTitles"));
}

void MapleSeed::SelectionChanged(QListWidget* listWidget)
{
    auto items = listWidget->selectedItems();
    if (items.isEmpty())
        return;

    TitleInfoItem* tii = reinterpret_cast<TitleInfoItem*>(items.first());
    ui->label->setPixmap(QPixmap(tii->getItem()->titleInfo->getCoverArtPath()));
}

void MapleSeed::showContextMenu(QListWidget* list, const QPoint& pos)
{
  QPoint globalPos = list->mapToGlobal(pos);
  if (list->selectedItems().isEmpty()) {
    return;
  }
  auto itm = list->selectedItems().first();
  auto tii = reinterpret_cast<TitleInfoItem*>(itm);
  auto entry = tii->getItem();
  TitleInfo* titleInfo = entry->titleInfo;
  if (!tii->getItem()) {
      return;
  }
  QString name(QFileInfo(tii->getItem()->directory).baseName());
  if (name.isEmpty()) {
      name = titleInfo->getName();
  }

  QMenu menu;

  if (QFileInfo(entry->rpx).exists())
  {
      menu.addAction(+"[Play] " + name, this, [=]
      { executeCemu(entry->rpx); })->setEnabled(true);
  }

  if (QFileInfo(entry->rpx).exists() && config->getIntegrateCemu())
  {
      menu.addSeparator();
      menu.addAction("Export Save Data", this, [&]
      {
          QDir dir = config->getBaseDirectory();
          if (dir.exists())
          {
              entry->backupSave(dir.filePath("Backup"));
          }
          else
          {
              qWarning() << "Save data export failed, base directory not valid" << config->getBaseDirectory();
          }
      })->setEnabled(true);
      menu.addAction("Import Save Data", this, [&]
      {
          QString dir = QDir(config->getBaseDirectory()+"/Backup/"+titleInfo->getFormatName()).absolutePath();
          if (QDir().mkpath(dir))
          {
              QFileInfo fileInfo = selectFile(dir);
              if (fileInfo.exists())
              {
                  entry->ImportSave(fileInfo.absoluteFilePath());
              }
          }
      })->setEnabled(true);
      menu.addAction("Purge Save Data", this, [&]
      {
          QMessageBox::StandardButton reply;
          reply = QMessageBox::question(this, titleInfo->getFormatName(), "Purge Save Data?", QMessageBox::Yes|QMessageBox::No);
          if (reply == QMessageBox::Yes)
          {
              QString saveDir = LibraryEntry::initSave(titleInfo->getID());
              QDir meta = QDir(saveDir).filePath("meta");
              QDir user = QDir(saveDir).filePath("user");
              if (!meta.removeRecursively() || !user.removeRecursively())
              {
                  qWarning() << "Purge Save Data: failed";
              }
          }
      })->setEnabled(true);
  }

  if (!QFileInfo(entry->metaxml).exists())
  {
      menu.addSeparator();
      TitleItem* ti_ui = new TitleItem(this);
      menu.addAction("Add Entry", this, [&]
      {
          if (ti_ui->add(titleInfo->getID()) == QDialog::Accepted){
              auto le = new LibraryEntry(std::move(ti_ui->getInfo()));
              this->updateTitleList(std::move(le));
          }
          delete ti_ui;
      });
      menu.addAction("Delete Entry", this, [=]
      {
          QMessageBox::StandardButton reply;
          reply = QMessageBox::question(this, titleInfo->getFormatName(), "Delete Entry?", QMessageBox::Yes|QMessageBox::No);
          if (reply == QMessageBox::Yes)
          {
              gameLibrary->database.remove(titleInfo->getID());
              if (gameLibrary->saveDatabase())
              {
                  delete ui->titlelistWidget->takeItem(ui->titlelistWidget->row(itm));
              }
          }
      });
      menu.addAction("Modify Entry", this, [&]
      {
          if (ti_ui->modify(tii->getItem()->titleInfo->getID()) == QDialog::Accepted){
              tii->setText(ti_ui->getInfo()->getFormatName());
              tii->getItem()->titleInfo->info = ti_ui->getInfo()->info;
              gameLibrary->database[ti_ui->getInfo()->getID()] = std::move(ti_ui->getInfo());
              gameLibrary->saveDatabase();
              list->editItem(tii);
          }
          delete ti_ui;
      });
  }

  menu.addSeparator();
  if (TitleInfo::ValidId(titleInfo->getID().replace(7, 1, '0'))) {
      menu.addAction("Download Game", this, [=] { titleInfo->download(); });
  }
  if (TitleInfo::ValidId(titleInfo->getID().replace(7, 1, 'c'))) {
      menu.addAction("Download DLC", this, [=] { titleInfo->downloadDlc(); });
  }
  if (TitleInfo::ValidId(titleInfo->getID().replace(7, 1, 'e'))) {
      menu.addAction("Download Patch", this, [=] { titleInfo->downloadPatch(); });
  }

  menu.addSeparator();
  if (QFile(QDir(titleInfo->getDirectory()).filePath("tmd")).exists() && QFile(QDir(titleInfo->getDirectory()).filePath("cetk")).exists())
      menu.addAction("Decrypt Content", this, [=] { QtConcurrent::run([=] {titleInfo->decryptContent(); }); });
  menu.addAction("Copy ID to Clipboard", this, [=] { CopyToClipboard(titleInfo->getID()); });

  menu.setEnabled(ui->menubar->isEnabled());
  menu.exec(globalPos);
}

void MapleSeed::disableMenubar()
{
    ui->menubar->setEnabled(false);
}

void MapleSeed::enableMenubar()
{
    ui->menubar->setEnabled(true);
}

void MapleSeed::updateListview(LibraryEntry* entry)
{
    TitleInfoItem* tii = new TitleInfoItem(entry);
    tii->setText(tii->getItem()->titleInfo->getFormatName());
    this->ui->listWidget->addItem(tii);
}

void MapleSeed::updateTitleList(LibraryEntry* entry)
{
    TitleInfoItem* tii = new TitleInfoItem(entry);
    tii->setText(tii->getItem()->titleInfo->getFormatName());
    this->ui->titlelistWidget->addItem(tii);
}

void MapleSeed::downloadStarted(QString filename)
{
  qInfo() << "Downloading" << filename;
}

void MapleSeed::downloadSuccessful(QString fileName)
{
    this->ui->progressBar->setValue(0);
    this->ui->progressBar->setFormat("%p%");
    qInfo() << "Download successful" << fileName;
}

void MapleSeed::downloadError(QString errorString)
{
    this->ui->menubar->setEnabled(true);
    qCritical() << errorString;
}

void MapleSeed::updateDownloadProgress(qint64 bytesReceived, qint64 bytesTotal, QTime qtime)
{
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

void MapleSeed::updateProgress(qint64 min, qint64 max, int curfile, int maxfiles)
{
    float per = (static_cast<float>(min) / static_cast<float>(max)) * 100;
    this->ui->progressBar->setRange(0, static_cast<int>(100));
    this->ui->progressBar->setValue(static_cast<int>(per));
    this->ui->progressBar->setFormat(QString::number(per, 'G', 3) + "% " +
        config->size_human(min) + " / " + config->size_human(max) + " | " +
        QString::number(curfile) + " / " + QString::number(maxfiles) + " files");
}

void MapleSeed::updateBaiscProgress(qint64 min, qint64 max)
{
  this->ui->progressBar->setRange(0, static_cast<int>(max));
  this->ui->progressBar->setValue(static_cast<int>(min));
  this->ui->progressBar->setFormat("%p% / %v of %m");
}

void MapleSeed::filter(QString region, QString filter_string)
{
    if (!filter_string.isEmpty())
        qInfo() << "filter:" << filter_string;

    ui->titlelistWidget->setItemSelected(nullptr, true);
    for (int row(0); row < ui->titlelistWidget->count(); row++)
        ui->titlelistWidget->item(row)->setHidden(true);

    QString searchString;
    auto matches = QList<QListWidgetItem*>();

    if (filter_string.isEmpty()) {
        searchString.append("*" + region + "* *");
        matches.append(ui->titlelistWidget->findItems(searchString, Qt::MatchFlag::MatchWildcard | Qt::MatchFlag::MatchCaseSensitive));
    }
    else {
        searchString.append("*" + region + "*" + filter_string + "*");
        matches.append(ui->titlelistWidget->findItems(searchString, Qt::MatchFlag::MatchWildcard));
    }

    QtConcurrent::blockingMapped(matches, &MapleSeed::processItemFilter);
}

QListWidgetItem* MapleSeed::processItemFilter(QListWidgetItem *item)
{
    if (self->mutex.tryLock(100))
    {
        if (Configuration::self->getKeyBool("eShopTitles")){
            item->setHidden(false);
        }else{
            auto tii = reinterpret_cast<TitleInfoItem*>(item);
            if (tii->getItem()->titleInfo->coverExists()){
                item->setHidden(false);
            }
        }
        self->mutex.unlock();
    }
    return item;
}

void MapleSeed::on_actionQuit_triggered()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Exit", "Exit Program?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
      QApplication::quit();
    }
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
    qInfo() << "Decrypting" << path;
    QtConcurrent::run([ = ] { config->decrypt->start(path); });
}

void MapleSeed::on_actionVerbose_triggered(bool checked)
{
    config->setKeyBool("VerboseLog", checked);
}

void MapleSeed::on_actionIntegrateCemu_triggered(bool checked)
{
    config->setKeyBool("IntegrateCemu", checked);
    QString cemulocation(config->getCemuPath());
    if (checked && !QFile(cemulocation).exists()) {
      QFileDialog dialog;
      dialog.setNameFilter("cemu.exe");
      dialog.setFileMode(QFileDialog::ExistingFile);
      if (dialog.exec()) {
        QStringList files(dialog.selectedFiles());
        config->setKey("CemuPath", QFileInfo(files[0]).absoluteFilePath());
      }
    }
    if (checked)
    {
        QMessageBox::information(this, "Warning!!!!!!",  "Save Data exports WILL NOT work with any other save data tool/program. DO NOT change the default export file name.");
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
    executeCemu(titleInfoItem->getItem()->rpx);
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
    return filter(ui->regionBox->currentText(), arg1);
}

void MapleSeed::on_regionBox_currentTextChanged(const QString &arg1)
{
    return filter(arg1, ui->searchInput->text());
}

void MapleSeed::on_checkBoxEShopTitles_stateChanged(int arg1)
{
    config->setKeyBool("eShopTitles", arg1);
    filter(ui->regionBox->currentText(), ui->searchInput->text());
}

void MapleSeed::on_actionGamepad_triggered(bool checked)
{
    config->setKeyBool("Gamepad", checked);

    if (Gamepad::instance == nullptr)
    {
        Gamepad::instance = new Gamepad;
        QtConcurrent::run([=] { Gamepad::instance->init(); });
        connect(Gamepad::instance, &Gamepad::gameUp, this, &MapleSeed::gameUp);
        connect(Gamepad::instance, &Gamepad::gameDown, this, &MapleSeed::gameDown);
        connect(Gamepad::instance, &Gamepad::gameStart, this, &MapleSeed::gameStart);
        connect(Gamepad::instance, &Gamepad::gameClose, this, &MapleSeed::gameClose);
    }

    if (checked){
        Gamepad::enable();
    }else {
        Gamepad::disable();
    }
}

void MapleSeed::on_actionDebug_triggered(bool checked)
{
    config->setKeyBool("DebugLogging", Debug::debugEnabled = checked);
}
