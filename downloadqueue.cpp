#include "downloadqueue.h"
#include "gamelibrary.h"

DownloadQueue *DownloadQueue::self;
QueueInfo *DownloadQueue::currentItem;

DownloadQueue::DownloadQueue(QObject *parent) : QObject(parent)
{
    self = this;
    connect(this, &DownloadQueue::Start, startQueue);
}

bool DownloadQueue::exists(QueueInfo *info)
{
    bool result = false;
    for(auto item : DownloadQueue::self->queue)
    {
        if (item->name == info->name){
            result = true;
            break;
        }
    }
    return result;
}

void DownloadQueue::bytesReceived(qint64 bytes)
{
    if (!bytes){
        return;
    }
    currentItem->bytesReceived += bytes;
    currentItem->updateProgress();
    emit self->QueueProgress(currentItem->bytesReceived, currentItem->totalSize, self->downloadTime);
}

void DownloadQueue::startQueue()
{
    DownloadManager manager;
    connect(&manager, &DownloadManager::bytesReceived, bytesReceived);

    auto queue = &self->queue;

    if (queue->isEmpty()) {
        bytesReceived(0);
        emit self->QueueFinished(self->history);
        self->history.clear();
        return;
    }

    self->downloadTime.start();
    currentItem = queue->first();

    for (auto pair : currentItem->urls)
    {
        QString filepath(pair.first);
        QUrl url(pair.second);
        manager.downloadSingle(url, filepath);
    }

    self->history.append(queue->dequeue());
    self->sessionHistory.append(self->history.last());

    emit self->ObjectFinished(currentItem);
    qInfo() << "Remove from Queue '" << currentItem->name << "' size:" << Configuration::size_human(currentItem->totalSize);
    emit self->Start();
}

void DownloadQueue::add(QueueInfo *info)
{
    if (self->queue.isEmpty())
        QTimer::singleShot(250, Qt::CoarseTimer, startQueue);

    self->queue.enqueue(info);
    emit self->ObjectAdded(info);
    qInfo() << "Add to Queue '" << info->name << "' size:" << Configuration::size_human(info->totalSize);
}
