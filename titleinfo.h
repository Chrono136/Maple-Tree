#ifndef TITLEBASE_H
#define TITLEBASE_H

#include <QObject>
#include <QtXml>
#include <QFile>
#include <QTableWidget>
#include <QMessageBox>
#include <QtConcurrent>
#include "decrypt.h"

enum TitleType {Game=0, Patch=1, Dlc=2};
typedef Decrypt::TitleMetaData TitleMetaData;

class TitleInfo : public QObject
{
    Q_OBJECT
public:
    explicit TitleInfo(QObject *parent = nullptr);

    static TitleInfo *Create(QString id, QString basedir);
    static TitleInfo *Create(QFileInfo metaxml, QString basedir);
    static TitleInfo *DownloadCreate(QString id, QString basedir);
    static QString getXmlValue(QFileInfo metaxml, QString field);
    static QDir getTempDirectory(QString folder);

    void init();
    void decryptContent(Decrypt *decrypt);
    QString getDirectory() const;
    QString getFormatName() const;
    QString getBaseDirectory() const;
    QString getCoverArt() const;
    QString getID() const;
    QString getKey() const;
    QString getName() const;
    QString getRegion() const;
    QString getProductCode() const;

private:
    TitleMetaData *getTMD(QString version);
    void parseJson(QByteArray byteArry, QString filepath);
    void setTitleType();

    QString id;
    QMap<QString, QString> info;
    QString baseDirectory;
    QFileInfo meta_xml;
    TitleType titleType;

signals:

public slots:
    void downloadJsonSuccessful(QString filepath);
};

#endif // TITLEBASE_H
