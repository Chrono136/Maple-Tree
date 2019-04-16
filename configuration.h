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
            configpath = getTempDirectory("config.json").path();
        configFile = new QFile(this->configPath = configpath);
    }
    ~Configuration(){
        save();
        configFile->close();
        delete configFile;
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
        configFile->seek(0);
        QJsonDocument doc = QJsonDocument::fromJson(configFile->readAll());

        if (doc.isNull()){
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
    }

    void save(){
        QVariantMap vmap;
        QMapIterator<QString, QString> i(configMap);

        while (i.hasNext()) {
            i.next();
            vmap.insert(i.key(), i.value());
        }

        QJsonDocument json = QJsonDocument::fromVariant(vmap);
        QByteArray bytearray(json.toJson(QJsonDocument::Indented));

        configFile->remove();
        configFile->write(bytearray);
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
    QFile *configFile;
};

#endif // CONFIGURATION_H
