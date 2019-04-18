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

class GameLibrary : public QObject {
  Q_OBJECT
 public:
  explicit GameLibrary(QObject* parent = nullptr);

  QVector<TitleInfo*> library;

  void init(QString path);

  QString baseDirectory;

  static GameLibrary* self;

 signals:
  void changed(TitleInfo*);

 public slots:
};

#endif  // GAMELIBRARY_H
