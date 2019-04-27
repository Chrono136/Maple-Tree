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

  void init(const QString& path);
  bool load(QString filepath);
  bool save(QString filepath);

  QString baseDirectory;

  QVector<LibraryEntry*> library;

  static GameLibrary* self;

 signals:
  void changed(LibraryEntry*);

 public slots:
};

#endif  // GAMELIBRARY_H
