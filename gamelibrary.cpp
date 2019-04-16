#include "gamelibrary.h"

GameLibrary::GameLibrary(QString *str, QObject *parent) : QObject(parent)
{
    this->baseDirectory = QString(*str);
}

void GameLibrary::init()
{
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
        QtConcurrent::run([&]
        {
            QFileInfo metaxml(filepath);
            if (metaxml.fileName().contains("meta.xml"))
            {
                library.append(TitleInfo::Create(metaxml, this->baseDirectory));
                emit changed(library.last());
            }
        });
        it.next();
    }
}
