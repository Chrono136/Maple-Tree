#ifndef TITLEBASE_H
#define TITLEBASE_H

#include <QFile>
#include <QMessageBox>
#include <QObject>
#include <QTableWidget>
#include <QtConcurrent>
#include <QtXml>
#include "decrypt.h"

enum TitleType { Game = 0, Patch = 1, Dlc = 2 };
typedef Decrypt::TitleMetaData TitleMetaData;

class TitleInfo : public QObject {
  Q_OBJECT
 public:
  explicit TitleInfo(QObject* parent = nullptr);

  static TitleInfo* Create(QString id, QString basedir);
  static TitleInfo* Create(const QFileInfo& metaxml, QString basedir);
  static TitleInfo* DownloadCreate(const QString& id, QString basedir);
  static QString getXmlValue(const QFileInfo& metaxml, const QString& field);
  static QDir getTempDirectory(const QString& folder);

  void init();
  void decryptContent(Decrypt* decrypt);
  QString getDirectory() const;
  QString getFormatName() const;
  QString getBaseDirectory() const;
  QString getCoverArtPath() const;
  QString getCoverArtUrl() const;
  QString getXmlLocation() const;
  QString getExecutable() const;
  TitleType getTitleType() const;
  QString getID() const;
  QString getKey() const;
  QString getName() const;
  QString getRegion() const;
  QString getProductCode() const;

 private:
  TitleMetaData* getTMD(const QString& version);
  void parseJson(const QByteArray& byteArry, const QString& filepath);
  void setTitleType();
  void downloadJsonSuccessful(const QString& filepath, bool downloadCover = false);

  QString id;
  QMap<QString, QString> info;
  QString baseDirectory;
  QFileInfo meta_xml;
  TitleType titleType;
  quint8 attempt = 0;

 signals:

 public slots:
};

#endif  // TITLEBASE_H
