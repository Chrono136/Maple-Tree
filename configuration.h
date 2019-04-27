#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMapIterator>
#include <QVariant>
#include <QUrl>

class Configuration {
 public:
  explicit Configuration(QString configpath = "") {
    if (configpath.isEmpty()) {
      configpath = getPersistentDirectory("").filePath("settings.json");
    }
    this->configPath = configpath;
    this->setKey("ConfigType", QString("Persistent"));
    this->setBaseDirectory(this->getBaseDirectory());
    Configuration::self = this;
  }
  ~Configuration() {
    if (getKeyString("ConfigType") == "Temporary") {
      if (this->getBaseDirectory() != QDir::currentPath()) {
        QDir().rmdir(this->getBaseDirectory());
        QDir().rmdir(this->getPersistentDirectory("").absolutePath());
      }
    } else {
      save();
    }
  }

  void setKeyBool(QString key, bool value) { jsonObject[key.toLower()] = value; }
  void setKey(QString key, QString value) { jsonObject[key.toLower()] = value; }
  bool getKeyBool(QString key) { return jsonObject[key.toLower()].toBool(); }
  QString getKeyString(QString key) { return jsonObject[key.toLower()].toString(); }

  QUrl getAPI_Url() {
    QString url(getKeyString("API_Url"));
    if (url.isEmpty()) {
      this->setKey("API_Url", "http://api.pixxy.in/");
    }
    return url;
  }

  QString getLibPath() {
    QDir dir(this->getPersistentDirectory(""));
    QString path(dir.filePath("library.json"));
    return path;
  }

  QString getBaseDirectory() {
    QString baseDir(getKeyString("BaseDirectory"));
    if (baseDir.isEmpty()) {
      return QDir::currentPath();
    } else {
      return baseDir;
    }
  }
  void setBaseDirectory(QString path) {
    setKey("BaseDirectory", QDir(path).absolutePath());
  }

  bool load() {
    QFile file(this->configPath);
    if (!file.open(QIODevice::ReadOnly)) {
      qWarning("Couldn't open settings file.");
      return false;
    }
    QByteArray byteArray = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(byteArray);
    QVariant variant = doc.toVariant();
    QMapIterator<QString, QVariant> it(variant.toMap());

    while (it.hasNext()) {
      it.next();
      jsonObject[it.key().toLower()] = it.value().toJsonValue();
    }
    return true;
  }

  bool save() {
    QFile file(this->configPath);
    if (!file.open(QIODevice::WriteOnly)) {
      qWarning("Couldn't save settings file.");
      return false;
    }

    QJsonDocument saveDoc(jsonObject);
    file.write(saveDoc.toJson(QJsonDocument::Indented));
    return true;
  }

  static QDir getTempDirectory(QString folder) {
    QDir tempDir(QDir(QDir::tempPath()).filePath(QCoreApplication::applicationName()));
    QDir dir(QDir(tempDir).filePath(folder));
    QDir().mkdir(dir.path());
    return dir;
  }

  static QDir getPersistentDirectory(QString folder) {
    QDir tempDir(QDir(QDir::homePath()).filePath(QCoreApplication::applicationName()));
    QDir dir(QDir(tempDir).filePath(folder));
    QDir().mkdir(dir.path());
    return dir;
  }

  QString configPath;
  static Configuration* self;

 private:
  QJsonObject jsonObject;
};

#endif  // CONFIGURATION_H
