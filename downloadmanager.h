#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QtCore>
#include <QtConcurrent>
#include <QtNetwork>

class DownloadManager : public QObject {
  Q_OBJECT
 public:
  explicit DownloadManager(QObject* parent = nullptr);

  QFile* downloadSingle(const QUrl& url, const QString& filepath, QString msg = "");
  QByteArray downloadBytes(const QUrl& url);

 signals:
  void downloadStarted(QString filename);
  void downloadSuccessful(QString filepath);
  void downloadFinished(qint32 downloadedCount, qint32 totalcount);
  void downloadProgress(qint64 bytesReceived, qint64 bytesTotal, QTime qtime);
  void downloadError(QString errorString);
  void bytesReceived(qint64 bytes);

 private slots:
  void startDownload(const QUrl& url, const QString& filepath);
  void progress(qint64 bytesReceived, qint64 bytesTotal);
  void finished();
  void readyRead();

 private:
  QNetworkAccessManager manager;
  QNetworkReply* currentDownload = nullptr;
  bool WriteToFile = true;
  QBuffer buffer;
  QFile output;

public:
  QTime downloadTime;
};

#endif  // DOWNLOADMANAGER_H
