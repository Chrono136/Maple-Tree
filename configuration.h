#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QVariant>
#include <QMapIterator>
#include <QJsonDocument>

class Configuration {
public:
    explicit Configuration(QString configpath = "") {
        if (configpath.isEmpty())
            configpath = getTempDirectory("").filePath("settings.json");
        this->configPath = configpath;
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
        configMap[key.toLower()] = value;
    }

    QString getKey(QString key){
        return configMap[key.toLower()];
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

    void load(){
        QFile file(this->configPath);
        file.open(QIODevice::ReadOnly);
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());

        if (doc.isNull()){
            file.close();
            return;
        }

        if (doc.isArray())
        {
            for(auto json : doc.array().toVariantList()){
                QMapIterator<QString, QVariant> i(json.toMap());
                while (i.hasNext()) {
                    i.next();
                    configMap[i.key().toLower()] = i.value().toString();
                }
            }
        }
        file.close();
    }

    void save(){
        QFile file(this->configPath);
        file.open(QIODevice::WriteOnly);
        QVariantMap vmap;
        QMapIterator<QString, QString> i(configMap);

        while (i.hasNext()) {
            i.next();
            vmap.insert(i.key(), i.value());
        }

        QJsonDocument json = QJsonDocument::fromVariant(vmap);
        QByteArray bytearray(json.toJson(QJsonDocument::Indented));

        file.write(bytearray);
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
    QMap<QString, QString> configMap;
};

#endif // CONFIGURATION_H
