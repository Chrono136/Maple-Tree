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
  bool actionOffline_ModeIsChecked();

  Configuration* config;
  Decrypt* decrypt;
  GameLibrary* gameLibrary;
  DownloadManager* downloadManager;
  static MapleSeed* self;

 private:
  Ui::MainWindow* ui;
  QProcess* process;

  void initialize();
  void defineActions();
  void defaultConfiguration();

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
  void updateListview(LibraryEntry* tb);
  void downloadStarted(QString filename);
  void downloadSuccessful(QString fileName);
  void downloadError(QString errorString);
  void updateDownloadProgress(qint64 bytesReceived, qint64 bytesTotal, QTime qtime);
  void updateBaiscProgress(qint64 min, qint64 max);
  void itemSelectionChanged();
  void itemDoubleClicked(QListWidgetItem* item);
  void actionConfigTemporary(bool checked);
  void actionConfigPersistent(bool checked);
  void actionVerboseChecked(bool checked);
  void actionIntegrateCemu(bool checked);
  void actionRefreshLibrary();
  void actionOffline_Mode(bool checked);
  void actionClear_Settings();
};

#endif  // MAINWINDOW_H
