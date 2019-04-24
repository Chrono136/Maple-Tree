#include "gamelibrary.h"

GameLibrary::GameLibrary(QObject* parent) : QObject(parent) {}

void GameLibrary::init(const QString& path) {
  this->baseDirectory = QDir(path).absolutePath();
  QDir dir = QDir(this->baseDirectory);

  if (!dir.exists()) {
    QMessageBox::critical(
        nullptr, "directory error", "GameLibrary::init(QString path): invalid directory: " + dir.path());
    return;
  }

  QtConcurrent::run([ = ] {
    QDirIterator it(dir.path(), QStringList() << "meta.xml", QDir::NoFilter, QDirIterator::Subdirectories);
    while (it.hasNext()) {
      it.next();
      QString filepath(it.filePath());
      QFileInfo metaxml(filepath);
      if (metaxml.fileName().contains("meta.xml")) {
        auto titleinfo = TitleInfo::Create(metaxml, this->baseDirectory);
        library.append(titleinfo);
        if (titleinfo->getID().at(7) == '0') {
          emit changed(titleinfo);
        }
      }
    }
  });
}
