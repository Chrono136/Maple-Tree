#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QtConcurrent>
#include <QInputDialog>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include "gamelibrary.h"
#include "downloadmanager.h"
#include "titleinfoitem.h"
#include "configuration.h"

namespace Ui {
class MainWindow;
}

class MapleSeed : public QMainWindow
{
    Q_OBJECT

public:
    explicit MapleSeed(QWidget *parent = nullptr);
    ~MapleSeed();

    Configuration *config;
    Decrypt *decrypt;
    GameLibrary *gameLibrary;
    DownloadManager *downloadManager;

private:
    Ui::MainWindow *ui;

    void initialize();
    void defineActions();

    void menuQuit();
    void menuChangeLibrary();
    void decryptContent();
    void startDownload();
    QDir *selectDirectory();

public slots:
    void updateListview(TitleInfo * tb);
    void downloadStarted(QString filename);
    void downloadSuccessful(QString fileName);
    void downloadError(QString errorString);
    void updateDownloadProgress(qint64 bytesReceived, qint64 bytesTotal, QTime qtime);
    void updateDecryptProgress(qint64 min, qint64 max);
    void itemSelectionChanged();
    void actionConfigTemporary(bool checked);
    void actionConfigPersistent(bool checked);
};

#endif // MAINWINDOW_H
