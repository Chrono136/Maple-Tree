#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QVariant>
#include <QMapIterator>
#include <QJsonDocument>

class Configuration {
public:
    explicit Configuration(QString configpath = "") {
        if (configpath.isEmpty())
            configpath = getTempDirectory("").filePath("settings.json");
        this->configPath = configpath;
        this->setKey("ConfigType", "Persistent");
        this->setBaseDirectory(this->getBaseDirectory());
    }
    ~Configuration() {
        if (getKey("ConfigType") == "Temporary"){
            if (this->getBaseDirectory() != QDir::currentPath()){
                QDir().rmdir(this->getBaseDirectory());
            }
        }else {
            save();
        }
    }

    void setKey(QString key, QString value){
        jsonObject[key.toLower()] = value;
    }
    QString getKey(QString key){
        return jsonObject[key.toLower()].toString();
    }

    QString getBaseDirectory(){
        QString baseDir(getKey("BaseDirectory"));
        if  (baseDir.isEmpty()){
            return QDir::currentPath();
        }
        else {
            return baseDir;
        }
    }
    void setBaseDirectory(QString path){
        setKey("BaseDirectory", QDir(path).absolutePath());
    }

    bool load(){
        QFile file(this->configPath);
        if (!file.open(QIODevice::ReadOnly)){
            qWarning("Couldn't open settings file.");
            return false;
        }
        QByteArray byteArray = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(byteArray);
        QVariant variant = doc.toVariant();
        QMapIterator<QString, QVariant> it(variant.toMap());

        while (it.hasNext()) {
            it.next();
            jsonObject[it.key().toLower()] = it.value().toString();
        }
        return true;
    }

    bool save(){
        QFile file(this->configPath);
        if (!file.open(QIODevice::WriteOnly)){
            qWarning("Couldn't save settings file.");
            return false;
        }

        QJsonDocument saveDoc(jsonObject);
        file.write(saveDoc.toJson(QJsonDocument::Indented));
        return true;
    }

    static QDir getTempDirectory(QString folder)
    {
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

#endif // CONFIGURATION_H
