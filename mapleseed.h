#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QInputDialog>
#include <QMainWindow>
#include <QPixmap>
#include <QtConcurrent>
#include <QClipboard>
#include <QList>
#include <QStandardItemModel>
#include "titleitem.h"
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
    QMutex mutex;

	void initialize();
	void defineActions();
    void defaultConfiguration();
	QDir* selectDirectory();
    QFileInfo selectFile(QString defaultDir = "");
    void CopyToClipboard(QString text);
    void executeCemu(QString rpxPath);

public slots:
    void messageLog(QString msg, bool verbose = false);
    void gameLibraryLoadComplete();
    void SelectionChanged(QListWidget* listWidget);
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
    void filter(QString region, QString filter_string);
    static QListWidgetItem* processItemFilter(QListWidgetItem* item);

private slots:
    void on_actionQuit_triggered();

    void on_actionChangeLibrary_triggered();

    void on_actionDecryptContent_triggered();

    void on_actionVerbose_triggered(bool checked);

    void on_actionIntegrateCemu_triggered(bool checked);

    void on_actionRefreshLibrary_triggered();

    void on_actionClearSettings_triggered();

    void on_actionCovertArt_triggered();

    void on_actionCompress_triggered();

    void on_actionDecompress_triggered();

    void on_actionDownload_triggered();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_itemSelectionChanged();

    void on_listWidget_customContextMenuRequested(const QPoint &pos);

    void on_titlelistWidget_itemSelectionChanged();

    void on_titlelistWidget_customContextMenuRequested(const QPoint &pos);

    void on_searchInput_textEdited(const QString &arg1);

    void on_regionBox_currentTextChanged(const QString &arg1);

    void on_checkBoxEShopTitles_stateChanged(int arg1);
};

#endif  // MAINWINDOW_H
