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

class Configuration {
 public:
  explicit Configuration(QString configpath = "") {
    if (configpath.isEmpty())
      configpath = getTempDirectory("").filePath("settings.json");
    this->configPath = configpath;
    this->setKey("ConfigType", QString("Persistent"));
    this->setBaseDirectory(this->getBaseDirectory());
  }
  ~Configuration() {
    if (getKeyString("ConfigType") == "Temporary") {
      if (this->getBaseDirectory() != QDir::currentPath()) {
        QDir().rmdir(this->getBaseDirectory());
      }
    } else {
      save();
    }
  }

  void setKeyBool(QString key, bool value) { jsonObject[key.toLower()] = value; }
  void setKey(QString key, QString value) { jsonObject[key.toLower()] = value; }
  bool getKeyBool(QString key) { return jsonObject[key.toLower()].toBool(); }
  QString getKeyString(QString key) { return jsonObject[key.toLower()].toString(); }

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
    if (!dir.exists())
      QDir().mkdir(dir.path());
    return dir;
  }

 private:
  QString configPath;
  QJsonObject jsonObject;
};

#endif  // CONFIGURATION_H
