#include "mapleseed.h"
#include "ui_mainwindow.h"
#include "versioninfo.h"

MapleSeed::MapleSeed(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("MapleSeed++ " + QString(GEN_VERSION_STRING));
    initialize();
}

MapleSeed::~MapleSeed()
{
    delete config;
    delete ui;
    delete decrypt;
    delete downloadManager;
    delete gameLibrary;
}

void MapleSeed::initialize()
{
    ui->statusbar->showMessage("Setting up enviornment variables");
    config = new Configuration;
    decrypt = new Decrypt;
    downloadManager = new DownloadManager;
    gameLibrary = new GameLibrary;

    defineActions();
    if (!config->load())
        config->save();

    gameLibrary->init(config->getBaseDirectory());
    ui->statusbar->showMessage("Environment setup complete");
}

void MapleSeed::defineActions()
{
    connect(decrypt, &Decrypt::decryptStarted, this, &MapleSeed::disableMenubar);
    connect(decrypt, &Decrypt::decryptFinished, this, &MapleSeed::enableMenubar);
    connect(decrypt, &Decrypt::progressReport, this, &MapleSeed::updateDecryptProgress);

    connect(gameLibrary, &GameLibrary::changed, this, &MapleSeed::updateListview);

    connect(downloadManager, &DownloadManager::downloadStarted, this, &MapleSeed::downloadStarted);
    connect(downloadManager, &DownloadManager::downloadStarted, this, &MapleSeed::disableMenubar);
    connect(downloadManager, &DownloadManager::downloadSuccessful, this, &MapleSeed::downloadSuccessful);
    connect(downloadManager, &DownloadManager::downloadSuccessful, this, &MapleSeed::enableMenubar);
    connect(downloadManager, &DownloadManager::downloadError, this, &MapleSeed::downloadError);
    connect(downloadManager, &DownloadManager::downloadError, this, &MapleSeed::enableMenubar);
    connect(downloadManager, &DownloadManager::downloadProgress, this, &MapleSeed::updateDownloadProgress);

    connect(ui->actionQuit, &QAction::triggered, this, &MapleSeed::menuQuit);
    connect(ui->actionChange_Library, &QAction::triggered, this, &MapleSeed::menuChangeLibrary);
    connect(ui->actionDownload_Title, &QAction::triggered, this, &MapleSeed::startDownload);
    connect(ui->actionDecrypt_Content, &QAction::triggered, this, &MapleSeed::decryptContent);
    connect(ui->listWidget, &QListWidget::itemSelectionChanged, this, &MapleSeed::itemSelectionChanged);
    connect(ui->actionConfigTemporary, &QAction::triggered, this, &MapleSeed::actionConfigTemporary);
    connect(ui->actionConfigPersistent, &QAction::triggered, this, &MapleSeed::actionConfigPersistent);
}

void MapleSeed::menuQuit()
{
    QApplication::quit();
}

void MapleSeed::menuChangeLibrary()
{
    QDir *dir = this->selectDirectory();
    if (dir == nullptr) return;
    config->setBaseDirectory(dir->path());

    ui->listWidget->clear();
    gameLibrary->init(dir->path());

    if (ui->listWidget->count() > 0){
        auto item = ui->listWidget->item(0);
        TitleInfoItem *itm = reinterpret_cast<TitleInfoItem*>(item);
        ui->label->setPixmap(QPixmap(itm->getItem()->getCoverArtPath()));
    }

    ui->statusbar->showMessage("Game library has been updated to: "+dir->path());
    delete dir;
}

void MapleSeed::decryptContent()
{
    QDir *dir = this->selectDirectory();
    if (dir == nullptr) return;
    QFileInfo tmd(dir->filePath("tmd"));
    QFileInfo cetk(dir->filePath("cetk"));

    if (!tmd.exists()){
        QMessageBox::critical(this, "Missing file", +"Missing: "+dir->filePath("/tmd"));
        return;
    }
    if (!cetk.exists()){
        QMessageBox::critical(this, "Missing file", +"Missing: "+dir->filePath("/cetk"));
        return;
    }

    auto path = dir->path();
    ui->statusbar->showMessage("Decrypt: "+path);
    QtConcurrent::run([=]{decrypt->start(path);});
    delete dir;
}

void MapleSeed::startDownload()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Download Title"), tr("Title ID:"), QLineEdit::Normal, nullptr, &ok);
    if (!ok) return;
    if (text.isEmpty() || text.count() != 16)
    {
        QMessageBox::information(this, "Download Title Error", "Invalid title id. Please verify your title id is 16 characters");
        return;
    }
    TitleInfo *ti = TitleInfo::DownloadCreate(text, gameLibrary->baseDirectory);
    if (ti == nullptr) return;
    auto dir = QString(ti->getDirectory());
    QtConcurrent::run([=]{ti->decryptContent(decrypt);});
}

QDir *MapleSeed::selectDirectory()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    dialog.setOption(QFileDialog::ShowDirsOnly);
    if (dialog.exec())
    {
        QStringList directories(dialog.selectedFiles());
        return new QDir(directories[0]);
    }
    return nullptr;
}

void MapleSeed::disableMenubar()
{
    this->ui->menubar->setEnabled(false);
}

void MapleSeed::enableMenubar()
{
    this->ui->menubar->setEnabled(true);
}

void MapleSeed::updateListview(TitleInfo *tb)
{
    if (ui->listWidget->count() == 1){
        ui->listWidget->setCurrentRow(0);
    }

    TitleInfoItem *tii = new TitleInfoItem(tb);
    tii->setText(tii->getItem()->getFormatName());
    this->ui->listWidget->addItem(tii);
    this->ui->statusbar->showMessage("Added to library: "+tb->getFormatName());
}

void MapleSeed::downloadStarted(QString filename)
{
    this->ui->statusbar->showMessage("Downloading: "+filename);
}

void MapleSeed::downloadSuccessful(QString fileName)
{
    this->ui->progressBar->setValue(0);
    this->ui->progressBar->setFormat("%p%");
    this->statusBar()->showMessage("Download successful: "+fileName);
}

void MapleSeed::downloadError(QString errorString)
{
    this->statusBar()->showMessage(errorString);
}

void MapleSeed::updateDownloadProgress(qint64 bytesReceived, qint64 bytesTotal, QTime qtime)
{
    this->ui->progressBar->setRange(0, static_cast<int>(bytesTotal));
    this->ui->progressBar->setValue(static_cast<int>(bytesReceived));

    double speed = bytesReceived * 1000.0 / qtime.elapsed();
    QString unit;
    if (speed < 1024) {
        unit = "bytes/sec";
    } else if (speed < 1024*1024) {
        speed /= 1024;
        unit = "kB/s";
    } else {
        speed /= 1024*1024;
        unit = "MB/s";
    }

    this->ui->progressBar->setFormat("%p%   /   "+QString::fromLatin1("%1 %2").arg(speed, 3, 'f', 1).arg(unit));
}

void MapleSeed::updateDecryptProgress(qint64 min, qint64 max)
{
    this->ui->progressBar->setValue(static_cast<int>(min));
    this->ui->progressBar->setRange(0, static_cast<int>(max));
    this->ui->progressBar->setFormat("%p%");
}

void MapleSeed::itemSelectionChanged()
{
    auto items = ui->listWidget->selectedItems();
    if (items.count() <= 0)
        return;

    TitleInfoItem *tii = reinterpret_cast<TitleInfoItem*>(items[0]);
    ui->label->setPixmap(QPixmap(tii->getItem()->getCoverArtPath()));
}

void MapleSeed::actionConfigTemporary(bool checked)
{
    config->setKey("ConfigType", "Temporary");
    ui->actionConfigPersistent->setChecked(!checked);
}

void MapleSeed::actionConfigPersistent(bool checked)
{
    config->setKey("ConfigType", "Persistent");
    ui->actionConfigTemporary->setChecked(!checked);
}
