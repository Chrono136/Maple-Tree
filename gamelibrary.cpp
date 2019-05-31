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

void GameLibrary::init(const QString& directory) {
    if (QDir(directory).exists()) {
        this->baseDirectory = QDir(directory).absolutePath();
    }
    else {
        log("GameLibrary::init(QString path): invalid directory: " + this->baseDirectory, true);
        log("Falling back to current directory: " + QDir(".").absolutePath(), true);
        Configuration::self->setBaseDirectory(this->baseDirectory = QDir(".").absolutePath());
        return this->init(this->baseDirectory);
    }

    database.clear();
    QDir dir(QDir(".").absolutePath());
    QString titlekeysPath(dir.filePath("titlekeys.json"));
    if (!QFile(jsonFile = titlekeysPath).exists()) {
        DownloadManager::getSelf()->downloadSingle(QUrl("http://pixxy.in/mapleseed/titlekeys.json"), titlekeysPath);
    }
    QFile qfile(titlekeysPath);
    if (!qfile.open(QIODevice::ReadOnly)) {
        log("GameLibrary::setupDatabase(): " + qfile.errorString(), true);
        return;
    }
    QByteArray byteArray(qfile.readAll());
    qfile.close();
    QtConcurrent::run([=] {
        setupDatabase(byteArray);
        setupLibrary();
    });
}

void GameLibrary::setupLibrary(bool force)
{
    return setupLibrary(this->baseDirectory, force);
}

void GameLibrary::setupLibrary(QString directory, bool force) {
    library.clear();
    if (!directory.isEmpty()) {
        if (QDir(directory).exists()) {
            this->baseDirectory = QDir(directory).absolutePath();
        }
        else {
            log("GameLibrary::init(QString path): invalid directory: " + this->baseDirectory, true);
            return;
        }
    }
    if (force) {
        QDir dir(this->baseDirectory);
        QStringList list(dir.entryList());
        QtConcurrent::blockingMapped(list, &GameLibrary::processLibItem);
        this->save(Configuration::self->getLibPath());
    }
    else {
        this->load(Configuration::self->getLibPath());
    }
    emit log("Library loaded: " + this->baseDirectory, true);
}

QString GameLibrary::processLibItem(const QString &d)
{
    auto self = GameLibrary::self;
    QDir baseDir(QDir(self->baseDirectory).filePath(d));
    QDir dir(baseDir.filePath("meta"));
    if (baseDir.path().contains("[Update]") || baseDir.path().contains("[DLC]"))
        return d;
    QFileInfo metaxml(dir.filePath("meta.xml"));
    if (metaxml.exists()){
        auto titleinfo = TitleInfo::Create(metaxml, self->baseDirectory);
        if (titleinfo->getTitleType() == TitleType::Game) {
            LibraryEntry* entry = new LibraryEntry(std::move(titleinfo));
            entry->rpx = entry->titleInfo->getExecutable();
            entry->directory = baseDir.absolutePath();
            entry->metaxml = metaxml.filePath();
            if (entry->titleInfo->getTitleType() == TitleType::Game) {
                self->library[entry->titleInfo->getID()] = std::move(entry);
                emit self->changed(self->library[entry->titleInfo->getID()]);
                self->log("Added to library: " + self->library[entry->titleInfo->getID()]->metaxml, false);
            }else {
                self->log("Skipped, wrong type: " + self->library[entry->titleInfo->getID()]->metaxml, false);
            }
        }
    }
    return d;
}

void GameLibrary::setupDatabase(QByteArray qbyteArray) {
    QJsonDocument doc = QJsonDocument::fromJson(qbyteArray);
    if (doc["titlekeys"].isArray()) {
        QJsonArray array = doc["titlekeys"].toArray();
        QtConcurrent::blockingMapped(array.toVariantList(), &GameLibrary::processDbItem);
        emit log("Database loaded: " + this->jsonFile, true);
    }
}

QVariant GameLibrary::processDbItem(const QVariant &item)
{
    auto &self = GameLibrary::self;
    QMapIterator<QString, QVariant> i(item.toMap());
    TitleInfo* titleinfo = new TitleInfo;
    while (i.hasNext()) {
        i.next();
        titleinfo->info[i.key().toLower()] = i.value().toString();
    }
    self->mutex.lock();
    QString id(titleinfo->getID());
    if (!titleinfo->getID().isEmpty() && !self->database.contains(id)) {
        self->database[id] = std::move(titleinfo);
        LibraryEntry* entry = new LibraryEntry(self->database[id]);
        if (entry->titleInfo->getTitleType() == TitleType::Game) {
            emit self->addTitle(entry);
            self->log("Added game: " + entry->titleInfo->getFormatName(), false);
        }else {
            self->log("Skipped, wrong type: " + entry->titleInfo->getFormatName(), false);
        }
    }
    self->mutex.unlock();
    return item;
}

bool GameLibrary::saveDatabase(QString filepath)
{
    if (database.size() <= 0)
        return false;

    QJsonObject json;
    QJsonArray array;
    foreach(const TitleInfo * titleInfo, database) {
        QJsonObject jobject;
        jobject["id"] = titleInfo->info["id"].toUpper();
        jobject["name"] = titleInfo->info["name"];
        jobject["key"] = titleInfo->info["key"].toUpper();
        jobject["productcode"] = titleInfo->info["productcode"].toUpper();
        jobject["region"] = titleInfo->info["region"].toUpper();
        array.append(jobject);
    }
    json["titlekeys"] = array;

    QFile saveFile(filepath);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }
    this->log("Saving database: " + filepath, true);
    QJsonDocument saveDoc(json);
    return saveFile.write(saveDoc.toJson());
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
        for (int index = 0; index < array.size(); ++index) {
            QJsonObject entryobject = array[index].toObject();
            LibraryEntry* entry = new LibraryEntry;
            QString id = entryobject["id"].toString();
            entry->directory = entryobject["directory"].toString();
            entry->rpx = entryobject["rpx"].toString();
            entry->metaxml = entryobject["metaxml"].toString();
            entry->titleInfo = TitleInfo::Create(id, this->baseDirectory);
            library[entry->titleInfo->getID()] = std::move(entry);
            emit changed(library.last());
        }
    }
    else {
        return false;
    }
    return true;
}

bool GameLibrary::save(QString filepath) {
    if (library.size() <= 0)
        return false;

    QJsonObject json;
    QJsonArray array;
    foreach(const LibraryEntry * entry, library) {
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

