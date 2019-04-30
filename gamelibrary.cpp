#include "gamelibrary.h"
#include "configuration.h"
#include "downloadmanager.h"
#include "mapleseed.h"

GameLibrary* GameLibrary::self;

GameLibrary::GameLibrary(QObject* parent) : QObject(parent) {
  GameLibrary::self = this;
}

GameLibrary::~GameLibrary() {
  this->save(Configuration::self->getLibPath());
}

void GameLibrary::init(const QString& path, bool checked) {
  this->baseDirectory = QDir(path).absolutePath();

  if (!QDir(path).exists()) {
    log("GameLibrary::init(QString path): invalid directory: " + path, true);
    return;
  }

  library.clear();
  QtConcurrent::run([ = ] {
    this->offline(checked);

    if (load(Configuration::self->getLibPath())) {
      return;
    }

    QDirIterator it(path, QStringList() << "meta.xml", QDir::NoFilter, QDirIterator::Subdirectories);
    while (it.hasNext()) {
      it.next();
      if (!it.fileName().contains("meta.xml") || it.filePath().contains("[Update]") || it.filePath().contains("[DLC]"))
        continue;
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
    this->save(Configuration::self->getLibPath());
  });
}

void GameLibrary::offline(bool checked) {
  QString titles(Configuration::self->getPersistentDirectory().filePath("titles.json"));
  QString titlekeys(Configuration::self->getPersistentDirectory().filePath("titlekeys.json"));

  if (checked) {
    log("Offline Mode: " + titles, true);
    database.clear();

    if (!QFile(titles).exists()) {
      DownloadManager::getSelf()->downloadSingle(QUrl("http://api.pixxy.in/title/all"), titles);
    }
    if (!QFile(titlekeys).exists()) {
      DownloadManager::getSelf()->downloadSingle(QUrl("http://api.pixxy.in/titlekey/all"), titlekeys);
    }

    QFile file1(titles);
    if (!file1.open(QIODevice::ReadOnly)) {
      log("GameLibrary::offline(bool): " + file1.errorString(), true);
      return;
    }
    process(file1.readAll());
    file1.close();


    QFile file2(titles);
    if (!file2.open(QIODevice::ReadOnly)) {
      log("GameLibrary::offline(bool): " + file2.errorString(), true);
      return;
    }
    process(file2.readAll());
    file2.close();
  }
}

void GameLibrary::process(QByteArray qbyteArray) {
  QJsonDocument doc = QJsonDocument::fromJson(qbyteArray);

  if (doc.isArray()) {
    QJsonArray array = doc.array();
    quint32 max = static_cast<quint32>(array.size());
    quint32 value = 1;

    for (const auto& json : array.toVariantList()) {
      QMapIterator<QString, QVariant> i(json.toMap());
      TitleInfo* titleinfo = new TitleInfo;
      while (i.hasNext()) {
        i.next();
        titleinfo->info[i.key().toLower()] = i.value().toString();
      }
      QString id(titleinfo->getID());
      if (!titleinfo->getID().isEmpty() && !database.contains(id))
        database[id] = std::move(titleinfo);
      progress(value++, max);
    }
  }
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
  if (library.size() <= 0)
    return false;

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
