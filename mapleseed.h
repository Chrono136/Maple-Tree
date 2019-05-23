#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QInputDialog>
#include <QMainWindow>
#include <QPixmap>
#include <QtConcurrent>
#include <QClipboard>
#include "QtCompressor.h"
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

	Configuration* config = new Configuration;
	DownloadManager* downloadManager = new DownloadManager;
	GameLibrary* gameLibrary = new GameLibrary;
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
	QFileInfo selectFile();

public slots:
	void messageLog(QString msg, bool verbose = false);
	void showContextMenuLibrary(const QPoint& pos);
	void showContextMenuTitles(const QPoint& pos);
	void showContextMenu(QListWidget*, const QPoint& pos);
	void disableMenubar();
	void enableMenubar();
	void updateListview(LibraryEntry* tb);
	void updateTitleList(LibraryEntry* entry);
	void downloadStarted(QString filename);
	void downloadSuccessful(QString fileName);
	void downloadError(QString errorString);
	void updateDownloadProgress(qint64 bytesReceived, qint64 bytesTotal, QTime qtime);
	void updateProgress(qint64 min, qint64 max, int curfile, int maxfile);
	void updateBaiscProgress(qint64 min, qint64 max);
	void itemSelectionChanged();
	void TitleSelectionChanged();
	void itemDoubleClicked(QListWidgetItem* item);
	void actionConfigTemporary(bool checked);
	void actionConfigPersistent(bool checked);
	void actionVerboseChecked(bool checked);
	void actionIntegrateCemu(bool checked);
	void actionRefreshLibrary();
	void actionOffline_Mode(bool checked);
	void actionClear_Settings();
	void actionCovertArt(bool checked);
	void filter(QString filter_string);
	void actionCompress();
	void actionDecompress();
};

#endif  // MAINWINDOW_H
