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
    explicit GameLibrary(QString *dir = nullptr, QObject *parent = nullptr);

    QVector<TitleInfo*> library;

    void init();

    QString baseDirectory;

signals:
    void changed(TitleInfo *);

public slots:
};

#endif // GAMELIBRARY_H
