#ifndef GAMELIBRARY_H
#define GAMELIBRARY_H

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QMessageBox>
#include <QObject>
#include <QVector>
#include <QtConcurrent>
#include "titleinfo.h"
#include "libraryentry.h"

class GameLibrary : public QObject {
    Q_OBJECT
public:
    explicit GameLibrary(QObject* parent = nullptr);
    ~GameLibrary();

    void init(const QString& directory);
    void setupLibrary(bool force = false);
    void setupLibrary(QString directory, bool force);
    static QString processLibItem(const QString &baseDir);
    void setupDatabase(QByteArray qbyteArray);
    static QVariant processDbItem(const QVariant &item);
    bool saveDatabase();
    bool load(QString filepath);
    bool save(QString filepath);

    QString jsonFile;
    QString baseDirectory;
    QMap<QString, LibraryEntry*> library;
    QMap<QString, TitleInfo*> database;

    static GameLibrary* self;

signals:
    void changed(LibraryEntry*);
    void addTitle(LibraryEntry*);
    void log(QString msg, bool verbose);
    void progress(quint32 min, quint32 max);
    void loadComplete();

private:
    QMutex mutex;
};

#endif  // GAMELIBRARY_H
