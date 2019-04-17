#ifndef GAMELIBRARY_H
#define GAMELIBRARY_H

#include <QObject>
#include <QDir>
#include <QDirIterator>
#include <QVector>
#include <QFile>
#include <QMessageBox>
#include <QtConcurrent>
#include "titleinfo.h"

class GameLibrary : public QObject
{
    Q_OBJECT
public:
    explicit GameLibrary(QObject *parent = nullptr);

    QVector<TitleInfo*> library;

    void init(QString path);

    QString baseDirectory;

    static GameLibrary *self;

signals:
    void changed(TitleInfo *);

public slots:
};

#endif // GAMELIBRARY_H
