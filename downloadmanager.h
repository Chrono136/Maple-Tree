#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QtConcurrent>
#include <QtCore>
#include <QtNetwork>

class DownloadManager : public QObject {
  Q_OBJECT
 public:
  explicit DownloadManager(QObject* parent = nullptr);

  QFile* downloadSingle(const QUrl& url, const QString& filepath, qulonglong totalsize = 0);
  void append(const QUrl& url, const QString& filepath);
  static DownloadManager* getSelf();

 signals:
  void downloadStarted(QString filename);
  void downloadSuccessful(QString filepath);
  void downloadFinished(qint32 downloadedCount, qint32 totalcount);
  void downloadProgress(qint64 bytesReceived, qint64 bytesTotal, QTime qtime);
  void downloadError(QString errorString);

 private slots:
  void _startNextDownload();
  void _downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
  void _downloadFinished();
  void _downloadReadyRead();

 private:
  bool isHttpRedirect() const;
  void reportRedirect();

  static DownloadManager* self;
  QNetworkAccessManager manager;
  QQueue<QPair<QString, QUrl>> downloadQueue;
  QNetworkReply* currentDownload = nullptr;
  QFile output;
  QTime downloadTime;

  qulonglong totalSize = 0;
  int downloadedCount = 0;
  int totalCount = 0;
  bool block = false;
};

#endif  // DOWNLOADMANAGER_H
