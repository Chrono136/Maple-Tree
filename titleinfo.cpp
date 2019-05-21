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
	if (ti->getKey().isEmpty()) {
		return nullptr;
	}
	TitleMetaData* tmd = ti->getTMD("");
	Ticket::Create(ti);

	auto contentCount = bs16(tmd->ContentCount);
	if (contentCount > 1000)
		return nullptr;

	for (int i = 0; i < contentCount; i++) {
		QString contentID = QString().sprintf("%08x", bs32(tmd->Contents[i].ID));
		QString contentPath = QDir(ti->getDirectory()).filePath(contentID);
		QString downloadURL = baseURL + id + QString("/") + contentID;
		qulonglong size = Decrypt::bs64(tmd->Contents[i].Size);
		if (!QFile(contentPath).exists() || QFileInfo(contentPath).size() != static_cast<qint64>(size)) {
			QString msg = QString("Downloading Content (%1) %2 of %3 (%4)").arg(contentID).arg(i + 1).arg(contentCount).arg(size);
			DownloadManager::getSelf()->downloadSingle(downloadURL, contentPath, msg);
		}
	}
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

void TitleInfo::init() {
	if (this->attempt >= 3) {
		GameLibrary::self->log("TitleInfo::init(): Unable to obtain title info", true);
		return;
	}

	if (id.isEmpty() || id.size() != 16) {
		GameLibrary::self->log("TitleInfo::init(): Invalid title id. Please verify your title id is 16 characters", true);
		return;
	}

	setTitleType();

	if (GameLibrary::self->database.contains(id.toLower())) {
		info = GameLibrary::self->database[id.toLower()]->info;
		QString coverPath = getCoverArtPath();
		if (!QFile(coverPath).exists()) {
			DownloadManager::getSelf()->downloadSingle(getCoverArtUrl(), coverPath);
		}
		return;
	}

	QString url(Configuration::self->getAPI_Url().url());

	if (titleType == TitleType::Game) {
		url += ("title/" + id);
	}
	else {
		url += ("titlekey/" + id);
	}

	QDir jsonDir(Configuration::self->getPersistentDirectory("json"));
	if (!QDir().mkdir(jsonDir.path())) {
		QDir().mkdir(QDir(jsonDir.path()).path());
		QDir().mkdir(jsonDir.path());
	}
	QString filepath(jsonDir.filePath(id + ".json"));

	if (QFile::exists(filepath)) {
		downloadJsonSuccessful(filepath, true);
		return;
	}

	DownloadManager::getSelf()->downloadSingle(url, filepath);
	downloadJsonSuccessful(filepath, true);
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

	decrypt->start(this->getDirectory());
}

QString TitleInfo::getDirectory() const {
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

QString TitleInfo::getFormatName() const {
	switch (titleType) {
	case TitleType::Patch:
		return QString("[") + this->getRegion() + QString("][Update] ") + this->getName();

	case TitleType::Dlc:
		return QString("[") + this->getRegion() + QString("][DLC] ") + this->getName();

	case TitleType::Game:
		return QString("[") + this->getRegion() + QString("] ") + this->getName();
	}

	return nullptr;
}

QString TitleInfo::getBaseDirectory() const {
	return QDir(baseDirectory).absolutePath() + QString("/");
}

QString TitleInfo::getCoverArtPath() const {
	QString code(this->getProductCode());

	QDir temp_dir(Configuration::self->getPersistentDirectory("covers"));
	QString cover = temp_dir.path() + QString("/" + code + ".jpg");

	return cover;
}

QString TitleInfo::getCoverArtUrl() const {
	QString code(this->getProductCode());
	return QString("http://pixxy.in/cover/?code=") + code + QString("&region=") +
		this->getRegion();
}

QString TitleInfo::getXmlLocation() const {
	return QString(meta_xml.filePath());
}

QString TitleInfo::getExecutable() const {
	QString root = QDir(QDir(this->getXmlLocation()).filePath("../../code")).absolutePath();
	QDirIterator it(root, QStringList() << "*.rpx", QDir::NoFilter);
	while (it.hasNext()) {
		it.next();
		QString filepath(it.filePath());
		return filepath;
	}
	return nullptr;
}

TitleType TitleInfo::getTitleType() const {
	return titleType;
}

QString TitleInfo::getID() const {
	if (info.contains("id")) {
		return info["id"].toLower();
	}
	return nullptr;

}

QString TitleInfo::getKey() const {
	if (info.contains("key")) {
		return info["key"];
	}
	return nullptr;

}

QString TitleInfo::getName() const {
	if (info.contains("name")) {
		return info["name"].simplified();
	}
	return nullptr;
}

QString TitleInfo::getRegion() const {
	if (info.contains("region")) {
		return info["region"];
	}
	return nullptr;

}

QString TitleInfo::getProductCode() const {
	if (info.contains("productcode")) {
		return info["productcode"].right(4);
	}
	return nullptr;

}

TitleMetaData* TitleInfo::getTMD(const QString & version) {
	QString tmdpath(this->getDirectory() + "/tmd");
	QString tmdurl("http://ccs.cdn.wup.shop.nintendo.net/ccs/download/" + id + "/tmd");
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
	if (id.size() <= 0)
		return;

	QChar ch = id.data()[7];
	if (ch == 'e' || ch == 'E') {
		titleType = TitleType::Patch;
	}
	else if (ch == 'c' || ch == 'C') {
		titleType = TitleType::Dlc;
	}
	else if (ch == '0') {
		titleType = TitleType::Game;
	}
}

void TitleInfo::downloadJsonSuccessful(const QString & filepath, bool downloadCover) {
	QFileInfo fileinfo(filepath);

	if (fileinfo.suffix() != "json")
		return;

	QFile file(filepath);
	if (!file.open(QIODevice::ReadOnly)) {
		GameLibrary::self->log("TitleInfo::downloadJsonSuccessful: " + file.errorString(), true);
		return;
	}
	QByteArray jsonByteArray(file.readAll());
	this->parseJson(jsonByteArray, filepath);
	file.close();

	if (this->getKey().isEmpty() || this->getName().isEmpty() || this->getRegion().isEmpty()) {
		if (!QFile(filepath).remove()) {
			GameLibrary::self->log("TitleInfo::downloadJsonSuccessful: " + filepath, true);
		}
		attempt++;
		this->init();
	}

	if (downloadCover && !QFile(getCoverArtPath()).exists()) {
		DownloadManager::getSelf()->downloadSingle(getCoverArtUrl(), getCoverArtPath());
	}
}
