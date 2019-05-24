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

    //load database entries
    database.clear();
    QString titlesPath(Configuration::self->getPersistentDirectory().filePath("titles.json"));
    QString titlekeysPath(Configuration::self->getPersistentDirectory().filePath("titlekeys.json"));

    if (!QFile(titlesPath).exists()) {
        DownloadManager::getSelf()->downloadSingle(QUrl("http://api.pixxy.in/title/all"), titlesPath);
    }
    if (!QFile(titlekeysPath).exists()) {
        DownloadManager::getSelf()->downloadSingle(QUrl("http://api.pixxy.in/titlekey/all"), titlekeysPath);
    }

    QFile* qfile = new QFile(titlesPath);
    if (!qfile->open(QIODevice::ReadOnly)) {
        log("GameLibrary::setupDatabase(): " + qfile->errorString(), true);
        return;
    }
    process(qfile->readAll());
    qfile->close();

    qfile = new QFile(titlekeysPath);
    if (!qfile->open(QIODevice::ReadOnly)) {
        log("GameLibrary::setupDatabase(): " + qfile->errorString(), true);
        return;
    }
    process(qfile->readAll());
    qfile->close();
    delete qfile;

    QtConcurrent::run([=] { this->setupLibrary(); });
}

void GameLibrary::setupLibrary(bool force)
{
    return setupLibrary(this->baseDirectory, force);
}

void GameLibrary::setupLibrary(QString directory, bool force) {
    library.clear();
    if (!directory.isEmpty()) {
        if (QDir(this->baseDirectory).exists()) {
            this->baseDirectory = QDir(directory).absolutePath();
        }
        else {
            log("GameLibrary::init(QString path): invalid directory: " + this->baseDirectory, true);
            return;
        }
    }
    if (force) {
        QDirIterator it(this->baseDirectory, QStringList() << "meta.xml", QDir::NoFilter, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            it.next();
            if (!it.fileName().contains("meta.xml") || it.filePath().contains("[Update]") || it.filePath().contains("[DLC]"))
                continue;
            auto titleinfo = TitleInfo::Create(it.fileInfo(), this->baseDirectory);
            if (titleinfo->getTitleType() == TitleType::Game) {
                LibraryEntry* entry = new LibraryEntry(std::move(titleinfo));
                entry->rpx = entry->titleInfo->getExecutable();
                entry->directory = QDir(it.filePath() + "/../../").absolutePath();
                entry->metaxml = it.filePath();
                library[entry->titleInfo->getID()] = std::move(entry);
                emit changed(library[entry->titleInfo->getID()]);
                log("Added to library: " + library[entry->titleInfo->getID()]->metaxml, true);
            }
        }
        this->save(Configuration::self->getLibPath());
    }
    else {
        load(Configuration::self->getLibPath());
    }
    emit log("Game library updated: " + this->baseDirectory, true);
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
            if (!titleinfo->getID().isEmpty() && !database.contains(id)) {
                database[id] = std::move(titleinfo);
                LibraryEntry* entry = new LibraryEntry(database[id]);
                emit addTitle(entry);
                log("Added to title list: " + entry->titleInfo->getName(), false);
            }
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

void GameLibrary::dump()
{
    QJsonObject json;
    QJsonArray array;
    for (const auto& entry : database){
        auto info = entry->info;
        if (info["key"].length() != 32 || info["name"].toUpper() == "NONE" || info["id"].at(6) != '0'){
            continue;
        }
        QJsonObject object;
        object["id"] = info.value("id").toUpper();
        object["key"] = info.value("key").toUpper();
        object["name"] = info.value("name");
        object["region"] = info.value("region").toUpper();
        QString code(info.value("productcode").toUpper());
        if (code.isEmpty() || code.toUpper() == "NONE"){
            QString baseId(info["id"].right(8));
            baseId.prepend("00050000");
            if (database.contains(baseId)){
                if (!database[baseId]->info.value("productcode").isEmpty()){
                    code = QString(database[baseId]->info.value("productcode"));
                }
            }
        }
        object["productcode"] = code;
        array.append(object);
    }
    json["titlekeys"] = array;
    QJsonDocument doc(json);
    QFile file("titlekeys.json");
    if (!file.open(QIODevice::WriteOnly)){
        emit log("Couldn't save titlekeys.json", false);
        return;
    }
    file.write(doc.toJson());
}
