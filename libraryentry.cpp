#include "libraryentry.h"
#include "configuration.h"
#include "QtCompressor.h"

LibraryEntry::LibraryEntry(TitleInfo* title)
{
  this->titleInfo = title;
}

LibraryEntry::LibraryEntry()
{

}

QString LibraryEntry::initSave(QString id)
{
    QString cemupath = QFileInfo(Configuration::self->getCemuPath()).absoluteDir().path();
    if (!QDir(cemupath).exists())
    {
        Configuration::log("LibraryEntry::initSave():CemuPath not found: " + cemupath);
        return nullptr;
    }
    QString savedir = QDir(cemupath).filePath("mlc01/usr/save/00050000");
    if (!QDir(savedir).exists())
    {
        Configuration::log("LibraryEntry::initSave():Cemu default save path not found: " + savedir);
        return nullptr;
    }
    savedir.append("/" + id.toUpper().right(8));
    if (!QDir(savedir).exists())
    {
        Configuration::log("LibraryEntry::initSave(): Save data not found: " + savedir);
        return nullptr;
    }
    return savedir;
}

void LibraryEntry::backupSave(QString saveTo)
{
    QString id = titleInfo->getID();
    saveTo = QDir(saveTo).filePath(titleInfo->getFormatName());
    QDir().mkpath(saveTo);
    saveTo = QDir(saveTo).filePath(id.right(8) + "-" + QDateTime::currentDateTime().toString("MM-dd-yyyy hh.mm.ss AP") + ".qta");
    QtConcurrent::run([=]
    {
        QString savedir = initSave(id);
        if (savedir.isEmpty())
        {
            return;
        }
        QtCompressor::compress(savedir, saveTo);
        Configuration::log("Backup Saved: " + saveTo);
    });
}

void LibraryEntry::ImportSave(QString filePath)
{
    QString filename = QFileInfo(filePath).fileName();
    QString id = "00050000" + filename.left(8);
    if (!TitleInfo::ValidId(id))
    {
        Configuration::log("LibraryEntry::ImportSave(): invalid filename, can not detect title id " + filename);
        return;
    }
    QString savedir = initSave(id);
    if (savedir.isEmpty())
    {
        Configuration::log("LibraryEntry::ImportSave(): saveDir is invalid or doesn't exist: " + savedir, true);
        Configuration::log("LibraryEntry::ImportSave(): Save directories are not created by MS, this is to avoid import issues.", true);
        Configuration::log("LibraryEntry::ImportSave(): Try creating a save file by playing the game through Cemu, then import again.", true);
        return;
    }
    QtConcurrent::run([=] {
        QtCompressor::decompress(filePath, QDir(savedir).absolutePath());
        Configuration::log("Backup Imported: " + QDir(savedir).absolutePath());
    });
}
