#include "titleinfo.h"
#include "downloadmanager.h"
#include "ticket.h"

TitleInfo::TitleInfo(QObject* parent) : QObject(parent) {}

TitleInfo* TitleInfo::Create(QString id, QString basedir) {
  TitleInfo* titleBase = new TitleInfo;
  titleBase->baseDirectory = basedir;
  titleBase->id = id;
  titleBase->init();
  return titleBase;
}

TitleInfo* TitleInfo::Create(QFileInfo metaxml, QString basedir) {
  return Create(getXmlValue(metaxml, "title_id"), basedir);
}

TitleInfo* TitleInfo::DownloadCreate(QString id, QString basedir) {
  QString baseURL("http://ccs.cdn.wup.shop.nintendo.net/ccs/download/");
  TitleInfo* ti = Create(id, basedir);
  TitleMetaData* tmd = ti->getTMD("");
  Ticket::Create(ti);

  quint16 contentCount = bs16(tmd->ContentCount);
  if (contentCount > 1000)
    return nullptr;
  for (int i = 0; i < contentCount; i++) {
    QString contentID = QString().sprintf("%08x", bs32(tmd->Contents[i].ID));
    QString contentPath = QDir(ti->getDirectory()).filePath(contentID);
    QString downloadURL = baseURL + id + QString("/") + contentID;
    qulonglong size = Decrypt::bs64(tmd->Contents[i].Size);
    if (!QFile(contentPath).exists() ||
        QFileInfo(contentPath).size() != static_cast<qint64>(size)) {
      // QString msg = QString("Downloading Content (%1) %2 of %3...
      // (%4)").arg(contentID).arg(i + 1).arg(contentCount).arg(size);
      DownloadManager::getSelf()->downloadSingle(downloadURL, contentPath);
    }
  }
  return ti;
}

QString TitleInfo::getXmlValue(QFileInfo metaxml, QString field) {
  QString value;
  if (QFile(metaxml.filePath()).exists()) {
    QDomDocument doc;
    QFile file(metaxml.filePath());
    if (!file.open(QIODevice::ReadOnly) || !doc.setContent(&file))
      return nullptr;

    QDomNodeList rates = doc.elementsByTagName("menu");
    for (int i = 0; i < rates.size(); i++) {
      QDomNode n = rates.item(i);
      QDomElement title_id = n.firstChildElement(field);
      if (title_id.isNull())
        continue;
      value = title_id.text();
    }
  }
  return value;
}

QDir TitleInfo::getTempDirectory(QString folder) {
  QDir tempDir(
      QDir(QDir::tempPath()).filePath(QCoreApplication::applicationName()));
  QDir dir(QDir(tempDir).filePath(folder));
  if (!dir.exists())
    QDir().mkdir(dir.path());
  return dir;
}

void TitleInfo::init() {
  if (id.isNull() || id.isEmpty() || id.size() <= 0) {
    QMessageBox::information(
        nullptr, "Download Title Error",
        "Invalid title id. Please verify your title id is 16 characters");
    return;
  }

  setTitleType();
  QString jsonurl("http://api.tsumes.com/");

  if (titleType == TitleType::Game) {
    jsonurl += QString("title/" + id);
  } else {
    jsonurl += QString("titlekey/" + id);
  }

  QString filepath(this->getTempDirectory("json").filePath(id + ".json"));

  if (QFile::exists(filepath)) {
    downloadJsonSuccessful(filepath, true);
    return;
  }

  DownloadManager::getSelf()->downloadSingle(jsonurl, filepath);
  downloadJsonSuccessful(filepath, true);
}

void TitleInfo::decryptContent(Decrypt* decrypt) {
  QString tmd = QDir(this->getDirectory()).filePath("tmd");
  QString cetk = QDir(this->getDirectory()).filePath("cetk");

  if (!QFile(tmd).exists()) {
    QMessageBox::critical(
        nullptr, "Error",
        "tmd not found, decryption failed:" + this->getDirectory());
    return;
  }
  if (!QFile(cetk).exists()) {
    QMessageBox::critical(
        nullptr, "Error",
        "cetk not found, decryption failed:" + this->getDirectory());
    return;
  }

  decrypt->start(this->getDirectory());
}

QString TitleInfo::getDirectory() const {
  return this->baseDirectory + "/" + this->getFormatName();
}

QString TitleInfo::getFormatName() const {
  switch (titleType) {
    case TitleType::Patch:
      return QString("[Update]") + QString("[") + this->getRegion() +
             QString("]") + this->getName();

    case TitleType::Dlc:
      return QString("[DLC]") + QString("[") + this->getRegion() +
             QString("]") + this->getName();

    case TitleType::Game:
      return QString("[Game]") + QString("[") + this->getRegion() +
             QString("]") + this->getName();
  }

  return nullptr;
}

QString TitleInfo::getBaseDirectory() const {
  return QDir(baseDirectory).absolutePath() + QString("/");
}

QString TitleInfo::getCoverArtPath() const {
  QString code(this->getProductCode());

  QString temp_dir(this->getTempDirectory("covers").path());
  QString cover = temp_dir + QString("/" + code + ".jpg");

  if (!QDir(temp_dir).exists())
    QDir().mkdir(temp_dir);

  return cover;
}

QString TitleInfo::getCoverArtUrl() const {
  QString code(this->getProductCode());
  return QString("http://pixxy.in/cover/?code=") + code + QString("&region=") +
         this->getRegion();
}

QString TitleInfo::getID() const {
  if (info.contains("id")) {
    return info["id"];
  } else {
    return nullptr;
  }
}

QString TitleInfo::getKey() const {
  if (info.contains("key")) {
    return info["key"];
  } else {
    return nullptr;
  }
}

QString TitleInfo::getName() const {
  if (info.contains("name")) {
    return info["name"];
  } else {
    return nullptr;
  }
}

QString TitleInfo::getRegion() const {
  if (info.contains("region")) {
    return info["region"];
  } else {
    return nullptr;
  }
}

QString TitleInfo::getProductCode() const {
  if (info.contains("productcode")) {
    return info["productcode"].right(4);
  } else {
    return nullptr;
  }
}

TitleMetaData* TitleInfo::getTMD(QString version) {
  QString tmdpath(this->getDirectory() + "/tmd");
  QString tmdurl("http://ccs.cdn.wup.shop.nintendo.net/ccs/download/" + id +
                 "/tmd");

  if (!version.isEmpty())
    tmdurl += "." + version;

  QFile* tmdfile;
  if (!QFile(tmdpath).exists()) {
    DownloadManager::getSelf()->downloadSingle(tmdurl, tmdpath);
    tmdfile = new QFile(tmdpath);
  } else {
    tmdfile = new QFile(tmdpath);
    if (!tmdfile->open(QIODevice::ReadOnly)) {
      QMessageBox::information(nullptr,
                               "*TitleInfo::getTMD():", tmdfile->errorString());
      return nullptr;
    }
  }
  if (tmdfile) {
    char* data = new char[static_cast<qulonglong>(tmdfile->size())];
    tmdfile->read(data, tmdfile->size());
    tmdfile->close();
    return reinterpret_cast<TitleMetaData*>(data);
  }
  return nullptr;
}

void TitleInfo::parseJson(QByteArray byteArry, QString filepath) {
  QJsonDocument doc = QJsonDocument::fromJson(byteArry);

  if (doc.isNull()) {
    if (QFile(filepath).remove()) {
      this->init();
    }
  }

  if (doc.isArray()) {
    for (auto json : doc.array().toVariantList()) {
      QMapIterator<QString, QVariant> i(json.toMap());
      while (i.hasNext()) {
        i.next();
        info[i.key().toLower()] = i.value().toString();
      }
    }
  }
}

void TitleInfo::setTitleType() {
  if (id.size() <= 0)
    return;

  QChar ch = id.data()[7];
  if (ch == 'e' || ch == 'E') {
    titleType = TitleType::Patch;
  } else if (ch == 'e' || ch == 'C') {
    titleType = TitleType::Dlc;
  } else if (ch == '0') {
    titleType = TitleType::Game;
  }
}

void TitleInfo::downloadJsonSuccessful(QString filepath, bool downloadCover) {
  QFileInfo fileinfo(filepath);

  if (fileinfo.suffix() != "json")
    return;

  QFile file(filepath);
  if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::information(nullptr, "error", file.errorString());
    return;
  }
  QByteArray jsonByteArray(file.readAll());
  this->parseJson(jsonByteArray, filepath);

  if (downloadCover && !QFile(getCoverArtPath()).exists()) {
    DownloadManager::getSelf()->downloadSingle(getCoverArtUrl(),
                                               getCoverArtPath());
  }
}
