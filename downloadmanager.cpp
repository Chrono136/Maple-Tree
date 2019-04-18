#include "downloadmanager.h"

DownloadManager* DownloadManager::self;

DownloadManager::DownloadManager(QObject* parent) : QObject(parent) {
  DownloadManager::self = this;
}

QFile* DownloadManager::downloadSingle(const QUrl& url,
                                       const QString filepath) {
  downloadQueue.enqueue({filepath, url});
  ++totalCount;
  block = true;
  _startNextDownload();
  _downloadFinished();
  block = false;
  return &output;
}

void DownloadManager::append(const QUrl& url, const QString filepath) {
  if (downloadQueue.isEmpty())
    QTimer::singleShot(0, this, &DownloadManager::_startNextDownload);

  downloadQueue.enqueue({filepath, url});
  ++totalCount;
}

DownloadManager* DownloadManager::getSelf() { return self; }

void DownloadManager::_startNextDownload() {
  if (downloadQueue.isEmpty()) {
    emit downloadFinished(downloadedCount, totalCount);
    return;
  }

  QPair<QString, QUrl> pair = downloadQueue.dequeue();
  QString filename = pair.first;
  QString dir(QFileInfo(filename).dir().path());
  QUrl url = pair.second;

  if (!QDir(dir).exists())
    QDir().mkdir(dir);

  output.setFileName(filename);
  if (!output.open(QIODevice::WriteOnly)) {
    emit downloadError("_startNextDownload():" + output.errorString());
    _startNextDownload();
    return;
  }

  QNetworkRequest request(url);
  currentDownload = manager.get(request);
  connect(currentDownload, &QNetworkReply::readyRead, this,
          &DownloadManager::_downloadReadyRead);
  connect(currentDownload, &QNetworkReply::downloadProgress, this,
          &DownloadManager::_downloadProgress);

  downloadTime.start();
  emit downloadStarted(filename);

  QEventLoop loop;
  if (block) {
    connect(currentDownload, &QNetworkReply::finished, &loop,
            &QEventLoop::quit);
    loop.exec();
  } else {
    connect(currentDownload, &QNetworkReply::finished, this,
            &DownloadManager::_downloadFinished);
  }
}

void DownloadManager::_downloadProgress(qint64 bytesReceived,
                                        qint64 bytesTotal) {
  emit downloadProgress(bytesReceived, bytesTotal, downloadTime);
}

void DownloadManager::_downloadFinished() {
  output.close();

  if (currentDownload->error()) {
    emit downloadError("_downloadFinished():" + currentDownload->errorString());
    output.remove();
  } else {
    if (isHttpRedirect()) {
      reportRedirect();
      output.remove();
    } else {
      emit downloadSuccessful(output.fileName());
      ++downloadedCount;
    }
  }

  currentDownload->deleteLater();
  _startNextDownload();
}

void DownloadManager::_downloadReadyRead() {
  output.write(currentDownload->readAll());
}

bool DownloadManager::isHttpRedirect() const {
  int statusCode =
      currentDownload->attribute(QNetworkRequest::HttpStatusCodeAttribute)
      .toInt();
  return statusCode == 301 || statusCode == 302 || statusCode == 303 ||
         statusCode == 305 || statusCode == 307 || statusCode == 308;
}

void DownloadManager::reportRedirect() {
  QUrl requestUrl = currentDownload->request().url();

  QVariant target =
      currentDownload->attribute(QNetworkRequest::RedirectionTargetAttribute);
  if (!target.isValid())
    return;

  QUrl redirectUrl = target.toUrl();
  if (redirectUrl.isRelative())
    redirectUrl = requestUrl.resolved(redirectUrl);
}
