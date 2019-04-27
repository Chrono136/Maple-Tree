#include "gamelibrary.h"
#include "configuration.h"

GameLibrary::GameLibrary(QObject* parent) : QObject(parent) {}

GameLibrary::~GameLibrary() {
  this->save(Configuration::self->getLibPath());
}

void GameLibrary::init(const QString& path) {
  this->baseDirectory = QDir(path).absolutePath();

  if (!QDir(path).exists()) {
    QMessageBox::critical(nullptr, "directory error", "GameLibrary::init(QString path): invalid directory: " + path);
    return;
  }

  library.clear();
  QtConcurrent::run([ = ] {
    if (!load(Configuration::self->getLibPath())) {
      QDirIterator it(path, QStringList() << "meta.xml", QDir::NoFilter, QDirIterator::Subdirectories);
      while (it.hasNext()) {
        it.next();
        if (it.fileName().contains("meta.xml") && !it.filePath().contains("[Update]")) {
          auto titleinfo = TitleInfo::Create(it.fileInfo(), path);
          if (titleinfo->getTitleType() == TitleType::Game) {
            LibraryEntry* entry = new LibraryEntry(std::move(titleinfo));
            entry->rpx = entry->titleInfo->getExecutable();
            entry->directory = QDir(it.filePath() + "/../../").absolutePath();
            entry->metaxml = it.filePath();
            library.append(entry);
            emit changed(entry);
            log("Added to library: " + entry->metaxml, true);
          }
        }
      }
      this->save(Configuration::self->getLibPath());
    }
  });
}

bool GameLibrary::load(QString filepath) {
  QFile loadFile(filepath);
  if (!loadFile.open(QIODevice::ReadOnly)) {
    qWarning("Couldn't open save file.");
    return false;
  }

  QByteArray saveData = loadFile.readAll();
  QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
  auto json = loadDoc.object();

  if (json.contains("Library") && json["Library"].isArray()) {
    this->log("Loading library: " + filepath, true);
    QJsonArray array = json["Library"].toArray();
    library.clear();
    library.reserve(array.size());
    for (int index = 0; index < array.size(); ++index) {
      QJsonObject entryobject = array[index].toObject();
      LibraryEntry* entry = new LibraryEntry;
      QString id = entryobject["id"].toString();
      entry->directory = entryobject["directory"].toString();
      entry->rpx = entryobject["rpx"].toString();
      entry->metaxml = entryobject["metaxml"].toString();
      entry->titleInfo = TitleInfo::Create(id, this->baseDirectory);
      library.append(std::move(entry));
      emit changed(library.last());
    }
  } else {
    return false;
  }
  return true;
}

bool GameLibrary::save(QString filepath) {
  QJsonObject json;
  QJsonArray array;
  foreach (const LibraryEntry* entry, library) {
    QJsonObject jobject;
    jobject["id"] = entry->titleInfo->getID();
    jobject["directory"] = entry->directory;
    jobject["rpx"] = entry->rpx;
    jobject["metaxml"] = entry->metaxml;
    array.append(jobject);
  }
  json["Library"] = array;

  QFile saveFile(filepath);
  if (!saveFile.open(QIODevice::WriteOnly)) {
    qWarning("Couldn't open save file.");
    return false;
  }
  this->log("Saving library: " + filepath, true);
  QJsonDocument saveDoc(json);
  return saveFile.write(saveDoc.toJson());
}
