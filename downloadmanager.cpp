#include "downloadmanager.h"

DownloadManager::DownloadManager(QObject* parent) : QObject(parent) {}

QFile* DownloadManager::downloadSingle(const QUrl& url, const QString& filepath, QString msg)
{
	if (!msg.isEmpty())
        qInfo() << msg;

    startDownload(url, filepath);

    return new QFile(filepath);
}

QByteArray DownloadManager::downloadBytes(const QUrl &url)
{
    WriteToFile = false;
    startDownload(url, nullptr);
    return buffer.readAll();
}

void DownloadManager::startDownload(const QUrl& url, const QString& filename)
{
    if (WriteToFile)
    {
        QString dir(QFileInfo(filename).dir().path());
        QDir().mkdir(dir);

        output.setFileName(filename);
        if (!output.open(QIODevice::WriteOnly)) {
            emit downloadError("_startNextDownload():" + output.errorString());
            emit downloadError("_startNextDownload():" + filename);
            emit downloadError("_startNextDownload():" + url.url());
            return;
        }
    }
    else {
        buffer.open(QBuffer::ReadWrite);
    }

	currentDownload = manager.get(QNetworkRequest(url));
    connect(currentDownload, &QNetworkReply::readyRead, this, &DownloadManager::readyRead);
    connect(currentDownload, &QNetworkReply::downloadProgress, this, &DownloadManager::progress);

	downloadTime.start();
	emit downloadStarted(filename);

    QEventLoop loop;
    connect(currentDownload, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    finished();
}

void DownloadManager::progress(qint64 received, qint64 total) {
    emit downloadProgress(received, total, downloadTime);
}

void DownloadManager::finished()
{
    if (WriteToFile) {
        output.close();
        emit downloadSuccessful(output.fileName());
    }
    else {
        buffer.seek(0);
        emit downloadSuccessful(nullptr);
    }

    currentDownload->deleteLater();

	emit downloadProgress(0, 100, downloadTime);
}

void DownloadManager::readyRead() {
    if (currentDownload->isReadable())
    {
        qint64 written;
        QByteArray qByteArray = currentDownload->readAll();
        if (WriteToFile) {
            written = output.write(qByteArray);
        }
        else {
            written = buffer.write(qByteArray);
        }
        emit bytesReceived(written);
    }
}
