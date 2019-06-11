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
        qWarning() << qfile.errorString();
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
    titleInfo->meta_xml = metaxml;
	return titleInfo;
}

TitleInfo* TitleInfo::DownloadCreate(const QString& id, QString basedir) {
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
            QDomElement element = n.firstChildElement(field);
            if (element.isNull())
				continue;
            value = element.text();
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
        qWarning() << "Unable to obtain title info" << id;
		return;
	}
    if (id.isEmpty() || id.size() != 16) {
        qWarning() << "Invalid title id" << id;
		return;
    }
    if (!GameLibrary::self->database.contains(id.toUpper())) {
        qWarning() << "id doesn't exist in titlekeys.json" << id;
		return;
    }
    info = GameLibrary::self->database[id.toUpper()]->info;
}

TitleInfo* TitleInfo::download(QString version)
{
	QString baseURL("http://ccs.cdn.wup.shop.nintendo.net/ccs/download/");
    if (getKey().isEmpty() || getKey().length() != 32) {
        qWarning() << "Invalid title key" << getKey();
		return nullptr;
	}

    QString directory(getDirectory());
    if (!QDir(directory).exists())
    {
        QDir().mkpath(directory);
    }

    auto tmd = getTMD(version);
    CreateTicket(version);

	auto contentCount = bs16(tmd->ContentCount);
	if (contentCount > 1000)
        return nullptr;

    qulonglong totalSize = 0;
    for (int i = 0; i < contentCount; i++)
    {
        totalSize += Decrypt::bs64(tmd->Contents[i].Size);
    }

    contentSize = totalSize;
    for (int i = 0; i < contentCount; i++)
    {
		QString contentID = QString().sprintf("%08x", bs32(tmd->Contents[i].ID));
        QString contentPath = QDir(directory).filePath(contentID);
		QString downloadURL = baseURL + getID() + QString("/") + contentID;
		qulonglong size = Decrypt::bs64(tmd->Contents[i].Size);
        if (!QFile(contentPath).exists() || QFileInfo(contentPath).size() != static_cast<qint64>(size))
        {
            QString sz(Configuration::self->size_human(totalSize));
            QString msg = QString("Download Status: %1 of %2 (%3)").arg(i + 1).arg(contentCount).arg(sz);
			QFile* file = DownloadManager::getSelf()->downloadSingle(downloadURL, contentPath, msg);
            file->close();
		}
        totalSize -= size;
	}

    QtConcurrent::run([=]
    {
        decryptContent();
        emit GameLibrary::self->processLibItem(directory);
    });
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

void TitleInfo::decryptContent()
{
    QString directory(getDirectory());
    QString tmd = QDir(directory).filePath("tmd");
    QString cetk = QDir(directory).filePath("cetk");

    if (!QFile(tmd).exists()) {
        qCritical() << "tmd not found, decryption failed" << directory;
		return;
    }
    if (!QFile(cetk).exists()) {
        qCritical() << "cetk not found, decryption failed" << directory;
		return;
    }
    Configuration::self->decrypt->start(directory);
}

qulonglong TitleInfo::getSize()
{
    return contentSize;
}

QString TitleInfo::getDirectory()
{
    if (meta_xml.exists())
    {
        return QDir(meta_xml.dir().filePath("..")).absolutePath();
    }

    QDir dir(getBaseDirectory());
    switch (getTitleType())
    {
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
    return QDir(dir.filePath(getFormatName())).absolutePath();
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
    return QString("http://pixxy.in/cover/?code=") + code + QString("&region=") + getRegion();
}

QString TitleInfo::getXmlLocation() {
	return QString(meta_xml.filePath());
}

QString TitleInfo::getExecutable() {
    QString root = meta_xml.dir().filePath("../code");
	QDirIterator it(root, QStringList() << "*.rpx", QDir::NoFilter);
    while (it.hasNext())
    {
        it.next();
        return QFileInfo(it.filePath()).absoluteFilePath();
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
    if (getID().isEmpty() || getKey().isEmpty())
    {
        qWarning() << "invalid id or key" << getID() << getKey();
    }
    QByteArray data;

    data.insert(0x1E6, QByteArray::fromHex(ver.toLatin1()));
    data.insert(0x1BF, QByteArray::fromHex(getKey().toLatin1()));
    data.insert(0x2CC, QByteArray::fromHex(getID().toLatin1()));

    QFile file(getDirectory() + "/cetk");
    if (!file.open(QIODevice::WriteOnly))
    {
        qCritical() << file.errorString();
        return nullptr;
    }
    file.write(data);
    file.close();

    return data;
}

TitleMetaData* TitleInfo::getTMD(const QString & version)
{
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
        qCritical() << tmdfile->errorString();
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
