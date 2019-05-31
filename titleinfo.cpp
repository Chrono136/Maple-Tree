#include <utility>
#include "titleinfo.h"
#include "configuration.h"
#include "downloadmanager.h"
#include "ticket.h"
#include "gamelibrary.h"

TitleInfo::TitleInfo(QObject* parent) : QObject(parent) {
	titleType = TitleType::Game;
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
    auto& db(GameLibrary::self->database);
	if (this->attempt >= 3) {
		GameLibrary::self->log("TitleInfo::init(): Unable to obtain title info", true);
		return;
	}
	if (id.isEmpty() || id.size() != 16) {
		GameLibrary::self->log("TitleInfo::init(): Invalid title id.", true);
		return;
    }
    if (!db.contains(id.toUpper())) {
        GameLibrary::self->log("TitleInfo::init(): id doesn't exist in titlekeys.json", true);
		return;
    }
    info = db[id.toUpper()]->info;
}

TitleInfo* TitleInfo::download(QString version)
{
	this->baseDirectory = Configuration::self->getBaseDirectory();
	QString baseURL("http://ccs.cdn.wup.shop.nintendo.net/ccs/download/");
	if (getKey().isEmpty()) {
		GameLibrary::self->log("TitleInfo::download(): Invalid title key", true);
		return nullptr;
	}
	TitleMetaData* tmd = getTMD(version);
	Ticket::Create(this);

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
	QString id(getID().replace(7, 1, 'e'));
    TitleInfo* titleInfo(Create(id, getBaseDirectory()));
    titleInfo->download();
    return titleInfo;
}

TitleInfo* TitleInfo::downloadPatch(QString version)
{
	QString id(getID().replace(7, 1, 'c'));
    TitleInfo* titleInfo(Create(id, getBaseDirectory()));
    titleInfo->download(version);
    return titleInfo;
}

void TitleInfo::decryptContent(Decrypt * decrypt) {
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
	if (decrypt == NULL) {
		decrypt = Configuration::self->decrypt;
	}
	decrypt->start(this->getDirectory());
}

QString TitleInfo::getDirectory() {
	QDir dir(this->baseDirectory);
	switch (titleType) {
	case TitleType::Patch:
		dir = dir.filePath("Updates");
		QDir().mkdir(dir.absolutePath());
		break;

	case TitleType::Dlc:
		dir = dir.filePath("DLC");
		QDir().mkdir(dir.absolutePath());
		break;

	case TitleType::Game:
		break;
	}
    return dir.filePath(this->getFormatName());
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

TitleMetaData* TitleInfo::getTMD(const QString & version) {
	QString tmdpath(this->getDirectory() + "/tmd");
	QString tmdurl("http://ccs.cdn.wup.shop.nintendo.net/ccs/download/" + getID() + "/tmd");
	if (!version.isEmpty())
		tmdurl += "." + version;

	QFile * tmdfile;
	if (!QFile(tmdpath).exists()) {
		DownloadManager::getSelf()->downloadSingle(tmdurl, tmdpath);
		tmdfile = new QFile(tmdpath);
	}
	else {
		tmdfile = new QFile(tmdpath);
	}
	if (!tmdfile->open(QIODevice::ReadOnly)) {
		QMessageBox::information(nullptr, "*TitleInfo::getTMD():", tmdfile->errorString());
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
