#ifndef DOWNLOADQUEUE_H
#define DOWNLOADQUEUE_H

#include <QObject>
#include <QMap>
#include <QTimer>
#include <QProgressBar>
#include "configuration.h"
#include "downloadmanager.h"

class QueueInfo : public QObject
{
    Q_OBJECT
public:
    explicit QueueInfo(QObject *parent = nullptr) : QObject(parent)
    {
        bytesReceived = 0;
        pgbar = new QProgressBar();
        pgbar->setStyleSheet("QProgressBar {\nborder: 1px solid black;\ntext-align: center;\npadding: 1px;\nwidth: 15px;\n}\n\nQProgressBar::chunk {\nbackground-color: #cd9bff;\nborder: 1px solid black;\n}");
        pgbar->setAlignment(Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignVCenter);
    }

    QString name;
    QString directory;
    qint64 totalSize;
    qint64 bytesReceived;
    QList<QPair<QString, QUrl>> urls;
    QProgressBar *pgbar;

public slots:
    void updateProgress()
    {
        pgbar->setRange(0, static_cast<int>(totalSize));
        pgbar->setValue(static_cast<int>(bytesReceived));
    }
};

class DownloadQueue : public QObject
{
    Q_OBJECT
public:
    explicit DownloadQueue(QObject *parent = nullptr);

    static bool exists(QueueInfo *info);
    static void bytesReceived(qint64 bytes);
    static void startQueue();

    static DownloadQueue *self;

signals:
    void Start();
    void ObjectAdded(QueueInfo *info);
    void ObjectFinished(QueueInfo *info);
    void QueueFinished(QList<QueueInfo*> history);
    void QueueProgress(qint64 received, qint64 total, QTime time);

public slots:
    static void add(QueueInfo *info);

private:
    QList<QueueInfo*> history;
    QList<QueueInfo*> sessionHistory;
    QQueue<QueueInfo*> queue;
    QTime downloadTime;
    static QueueInfo *currentItem;

public:
    QMutex mutex;
};

#endif // DOWNLOADQUEUE_H
