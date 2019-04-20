#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QInputDialog>
#include <QMainWindow>
#include <QPixmap>
#include <QtConcurrent>
#include "configuration.h"
#include "downloadmanager.h"
#include "gamelibrary.h"
#include "titleinfoitem.h"

namespace Ui {
class MainWindow;
}

class MapleSeed : public QMainWindow {
  Q_OBJECT

 public:
  explicit MapleSeed(QWidget* parent = nullptr);
  ~MapleSeed();

  Configuration* config;
  Decrypt* decrypt;
  GameLibrary* gameLibrary;
  DownloadManager* downloadManager;

 private:
  Ui::MainWindow* ui;

  void initialize();
  void defineActions();

  void menuQuit();
  void actionChange_Library();
  void actionDownload_Title();
  void actionUpdate();
  void actionDLC();
  void decryptContent();
  QDir* selectDirectory();

 public slots:
  void messageLog(QString msg, bool verbose = false);
  void disableMenubar();
  void enableMenubar();
  void updateListview(TitleInfo* tb);
  void downloadStarted(QString filename);
  void downloadSuccessful(QString fileName);
  void downloadError(QString errorString);
  void updateDownloadProgress(qint64 bytesReceived, qint64 bytesTotal, QTime qtime);
  void updateDecryptProgress(qint64 min, qint64 max);
  void itemSelectionChanged();
  void actionConfigTemporary(bool checked);
  void actionConfigPersistent(bool checked);
  void actionVerboseChecked(bool checked);
};

#endif  // MAINWINDOW_H
