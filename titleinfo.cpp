#include <utility>
#include "titleinfo.h"
#include "configuration.h"
#include "downloadmanager.h"
#include "gamelibrary.h"

TitleInfo::TitleInfo(QObject* parent) : QObject(parent)
{
    titleType = TitleType::Game;
}

quint32 TitleInfo::getRpxHash(QString rpxPath)
{
    QFile qfile(rpxPath);
    if (!qfile.open(QIODevice::ReadOnly)) {
        emit Configuration::self->log("TitleInfo::getRpxHash(): " + qfile.errorString());
        return 0;
    }
    QByteArray data(qfile.readAll());
    qfile.close();

    quint32 h = 0x3416DCBF;
    for (auto i = 0; i < data.size(); i++)
    {
        quint32 c = static_cast<quint32>(data.at(i));
        h = (h << 3) | (h >> 29);
        h += c;
    }
    return h;
}

TitleInfo* TitleInfo::Create(QString id, QString basedir) {
	auto* titleBase = new TitleInfo;
	titleBase->baseDirectory = std::move(basedir);
	titleBase->id = std::move(id);
	titleBase->init();
	return titleBase;
}

TitleInfo* TitleInfo::Create(const QFileInfo& metaxml, QString basedir) {
	TitleInfo* titleInfo = Create(getXmlValue(metaxml, "title_id"), std::move(basedir));
	titleInfo->meta_xml = metaxml.filePath();
	return titleInfo;
}

TitleInfo* TitleInfo::DownloadCreate(const QString& id, QString basedir) {
	QString baseURL("http://ccs.cdn.wup.shop.nintendo.net/ccs/download/");
	TitleInfo* ti = Create(id, std::move(basedir));
	ti->download();
	return ti;
}

QString TitleInfo::getXmlValue(const QFileInfo & metaxml, const QString & field) {
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

bool TitleInfo::ValidId(QString id)
{
    auto& db = GameLibrary::self->database;
    if (db.contains(id.toUpper())) {
		return true;
    }
	return false;
}

void TitleInfo::init() {
	if (this->attempt >= 3) {
		GameLibrary::self->log("TitleInfo::init(): Unable to obtain title info", true);
		return;
	}
	if (id.isEmpty() || id.size() != 16) {
		GameLibrary::self->log("TitleInfo::init(): Invalid title id.", true);
		return;
    }
    if (!GameLibrary::self->database.contains(id.toUpper())) {
        GameLibrary::self->log("TitleInfo::init(): id doesn't exist in titlekeys.json", true);
		return;
    }
    info = GameLibrary::self->database[id.toUpper()]->info;
}

TitleInfo* TitleInfo::download(QString version)
{
	QString baseURL("http://ccs.cdn.wup.shop.nintendo.net/ccs/download/");
	if (getKey().isEmpty()) {
        emit GameLibrary::self->log("TitleInfo::download(): Invalid title key", true);
		return nullptr;
	}

    TitleMetaData* tmd = getTMD(version);
    CreateTicket(version);

	auto contentCount = bs16(tmd->ContentCount);
	if (contentCount > 1000)
		return nullptr;

	for (int i = 0; i < contentCount; i++) {
		QString contentID = QString().sprintf("%08x", bs32(tmd->Contents[i].ID));
		QString contentPath = QDir(getDirectory()).filePath(contentID);
		QString downloadURL = baseURL + getID() + QString("/") + contentID;
		qulonglong size = Decrypt::bs64(tmd->Contents[i].Size);
		if (!QFile(contentPath).exists() || QFileInfo(contentPath).size() != static_cast<qint64>(size)) {
			QString sz(Configuration::self->size_human(size));
			QString msg = QString("Downloading Content (%1) %2 of %3 (%4)").arg(contentID).arg(i + 1).arg(contentCount).arg(sz);
			QFile* file = DownloadManager::getSelf()->downloadSingle(downloadURL, contentPath, msg);
			file->close();
		}
	}

	QtConcurrent::run([=] { decryptContent(); });
	return this;
}

TitleInfo* TitleInfo::downloadDlc()
{
    QString id(getID().replace(7, 1, 'c'));
    TitleInfo* titleInfo(Create(id, getBaseDirectory()));
    titleInfo->download();
    return titleInfo;
}

TitleInfo* TitleInfo::downloadPatch(QString version)
{
    QString id(getID().replace(7, 1, 'e'));
    TitleInfo* titleInfo(Create(id, getBaseDirectory()));
    titleInfo->download(version);
    return titleInfo;
}

void TitleInfo::decryptContent() {
	QString tmd = QDir(this->getDirectory()).filePath("tmd");
	QString cetk = QDir(this->getDirectory()).filePath("cetk");

    if (!QFile(tmd).exists()) {
        emit GameLibrary::self->log("TitleInfo::decryptContent(): tmd not found, decryption failed:" + getDirectory(), true);
		return;
    }
    if (!QFile(cetk).exists()) {
        emit GameLibrary::self->log("TitleInfo::decryptContent(): cetk not found, decryption failed:" + getDirectory(), true);
		return;
    }
    Configuration::self->decrypt->start(getDirectory());
}

QString TitleInfo::getDirectory() {
    QDir dir(getBaseDirectory());
    switch (getTitleType()) {
	case TitleType::Patch:
        dir = dir.filePath("Patch");
		break;

	case TitleType::Dlc:
        dir = dir.filePath("DLC");
		break;

    case TitleType::Demo:
        dir = dir.filePath("Demo");
        break;

	case TitleType::Game:
		break;
    }
    QString path(dir.filePath(getFormatName()));
    QDir().mkdir(path);
    return path;
}

QString TitleInfo::getFormatName() {
    switch (getTitleType()) {
	case TitleType::Patch:
		return QString("[") + this->getRegion() + QString("][Update] ") + this->getName();
	case TitleType::Dlc:
		return QString("[") + this->getRegion() + QString("][DLC] ") + this->getName();
    case TitleType::Demo:
        return QString("[") + this->getRegion() + QString("][Demo] ") + this->getName();
	case TitleType::Game:
		return QString("[") + this->getRegion() + QString("] ") + this->getName();
	}
	return nullptr;
}

QString TitleInfo::getBaseDirectory() {
	return QDir(baseDirectory).absolutePath() + QString("/");
}

QString TitleInfo::getCoverArtPath() {
	QString code(this->getProductCode());
	QString cover;

	QDir directory("covers");
	QStringList nameFilter("*" + code + "*.jpg");
	QStringList list = directory.entryList(nameFilter);
	if (list.count() > 0){
		cover = directory.filePath(list.first());
	}
	else {
		cover = directory.filePath("!.jpg");
	}

	return cover;
}

QString TitleInfo::getCoverArtUrl() {
	QString code(this->getProductCode());
	return QString("http://pixxy.in/cover/?code=") + code + QString("&region=") +
		this->getRegion();
}

QString TitleInfo::getXmlLocation() {
	return QString(meta_xml.filePath());
}

QString TitleInfo::getExecutable() {
	QString root = QDir(QDir(this->getXmlLocation()).filePath("../../code")).absolutePath();
	QDirIterator it(root, QStringList() << "*.rpx", QDir::NoFilter);
	while (it.hasNext()) {
		it.next();
		QString filepath(it.filePath());
		return filepath;
	}
	return nullptr;
}

TitleType TitleInfo::getTitleType() {
    setTitleType();
	return titleType;
}

QString TitleInfo::getID() {
	if (info.contains("id")) {
        return info["id"].toUpper();
	}
	return nullptr;

}

QString TitleInfo::getKey() {
	if (info.contains("key")) {
        return info["key"].toUpper();
	}
	return nullptr;

}

QString TitleInfo::getName() {
	if (info.contains("name")) {
		return info["name"].simplified();
	}
	return nullptr;
}

QString TitleInfo::getRegion() {
	if (info.contains("region")) {
		return info["region"];
	}
	return nullptr;

}

QString TitleInfo::getProductCode() {
	if (info.contains("productcode")) {
		return info["productcode"].right(4);
	}
	return nullptr;
}

bool TitleInfo::coverExists()
{
    if (b_coverExistsIsSet){
        return b_coverExists;
    }
    QString code(this->getProductCode());
    if (coversDirectory.entryList(QStringList() << "*"+code+"*.jpg").isEmpty()){
        b_coverExists = false;
    }else{
        b_coverExists = true;
    }
    b_coverExistsIsSet = true;
    return b_coverExists;
}

QByteArray TitleInfo::CreateTicket(QString ver)
{
    if (getID().isEmpty() || getKey().isEmpty()) {
        emit Configuration::self->log("*TitleInfo::CreateTicket(ver): invalid id or key");
    }
    QByteArray data;

    data.insert(0x1E6, QByteArray::fromHex(ver.toLatin1()));
    data.insert(0x1BF, QByteArray::fromHex(getKey().toLatin1()));
    data.insert(0x2CC, QByteArray::fromHex(getID().toLatin1()));

    QFile file(getDirectory() + "/cetk");
    if (!file.open(QIODevice::WriteOnly)) {
        emit Configuration::self->log("Ticket::Create(): " + file.errorString());
      return nullptr;
    }
    file.write(data);
    file.close();

    return data;
}

TitleMetaData* TitleInfo::getTMD(const QString & version) {
    QString tmdpath(getDirectory() + "/tmd");
	QString tmdurl("http://ccs.cdn.wup.shop.nintendo.net/ccs/download/" + getID() + "/tmd");
    if (!version.isEmpty()){
        tmdurl += "." + version;
    }
	if (!QFile(tmdpath).exists()) {
        DownloadManager::getSelf()->downloadSingle(tmdurl, tmdpath);
    }
    QFile* tmdfile = new QFile(tmdpath);
	if (!tmdfile->open(QIODevice::ReadOnly)) {
        emit Configuration::self->log("*TitleInfo::getTMD(): " + tmdfile->errorString());
		return nullptr;
	}
	char* data = new char[static_cast<qulonglong>(tmdfile->size())];
	tmdfile->read(data, tmdfile->size());
	tmdfile->close();
	return reinterpret_cast<TitleMetaData*>(data);
}

void TitleInfo::parseJson(const QByteArray & byteArry, const QString & filepath) {
	QJsonDocument doc = QJsonDocument::fromJson(byteArry);

	if (doc.isNull()) {
		if (QFile(filepath).remove()) {
			this->init();
		}
	}

	if (doc.isArray()) {
		for (const auto& json : doc.array().toVariantList()) {
			QMapIterator<QString, QVariant> i(json.toMap());
			while (i.hasNext()) {
				i.next();
				info[i.key().toLower()] = i.value().toString();
			}
		}
	}
}

void TitleInfo::setTitleType() {
    if (getID().size() != 16)
		return;
    QChar ch = getID().at(7);
	if (ch == 'e' || ch == 'E') {
		titleType = TitleType::Patch;
	}
	else if (ch == 'c' || ch == 'C') {
		titleType = TitleType::Dlc;
	}
    else if (ch == '2') {
        titleType = TitleType::Demo;
	}
    else if (ch == '0') {
        titleType = TitleType::Game;
    }
}
