#ifndef TITLEBASE_H
#define TITLEBASE_H

#include <QFile>
#include <QMessageBox>
#include <QObject>
#include <QTableWidget>
#include <QtConcurrent>
#include <QtXml>
#include "decrypt.h"

enum TitleType { Game = 0, Demo = 1, Patch = 2, Dlc = 3 };
typedef Decrypt::TitleMetaData TitleMetaData;

class TitleInfo : public QObject {
	Q_OBJECT
public:
	explicit TitleInfo(QObject* parent = nullptr);

	static TitleInfo* Create(QString id, QString basedir);
	static TitleInfo* Create(const QFileInfo& metaxml, QString basedir);
	static TitleInfo* DownloadCreate(const QString& id, QString basedir);
	static QString getXmlValue(const QFileInfo& metaxml, const QString& field);
	static bool ValidId(QString id);
	void init();
	TitleInfo* download(QString version = "");
	TitleInfo* downloadDlc();
	TitleInfo* downloadPatch(QString version = "");
    void decryptContent();
    QString getDirectory();
    QString getFormatName();
    QString getBaseDirectory();
    QString getCoverArtPath();
    QString getCoverArtUrl();
    QString getXmlLocation();
    QString getExecutable();
    TitleType getTitleType();
    QString getID();
    QString getKey();
    QString getName();
    QString getRegion();
    QString getProductCode();
    bool coverExists();

	QMap<QString, QString> info;

private:
    QByteArray CreateTicket(QString version);
    TitleMetaData* getTMD(const QString& version);
	void parseJson(const QByteArray& byteArry, const QString& filepath);
    void setTitleType();

	QString id;
	QString baseDirectory;
	QFileInfo meta_xml;
	TitleType titleType;
	quint8 attempt = 0;
    QDir coversDirectory = QDir("covers");
    bool b_coverExists = false;
    bool b_coverExistsIsSet = false;

signals:

public slots:
};

#endif  // TITLEBASE_H
