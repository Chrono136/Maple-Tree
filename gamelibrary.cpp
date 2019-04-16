#include "gamelibrary.h"

GameLibrary::GameLibrary(QObject *parent) : QObject(parent)
{
}

void GameLibrary::init(QString path)
{
    this->baseDirectory = QDir(path).absolutePath();
    QDir dir = QDir(this->baseDirectory);

    if (!dir.exists())
    {
        QMessageBox mb;
        mb.setWindowTitle("Directory Error!!");
        mb.setText("Invalid directory!!");
        mb.setStandardButtons(QMessageBox::Ok);
        mb.setIcon(QMessageBox::Critical);
        return;
    }

    QDirIterator it(dir.path(), QStringList() << "meta.xml", QDir::NoFilter, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString filepath(it.filePath());
        QFileInfo metaxml(filepath);
        if (metaxml.fileName().contains("meta.xml"))
        {
            library.append(TitleInfo::Create(metaxml, this->baseDirectory));
            emit changed(library.last());
        }
        it.next();
    }
}
