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

  void init(const QString& path);

  QString baseDirectory;

  QVector<TitleInfo*> library;

  static GameLibrary* self;

 signals:
  void changed(TitleInfo*);

 public slots:
};

#endif  // GAMELIBRARY_H
