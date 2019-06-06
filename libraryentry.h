#ifndef LIBRARYENTRY_H
#define LIBRARYENTRY_H

#include "titleinfo.h"
#include "QtConcurrent"

class LibraryEntry {
 public:
  LibraryEntry(TitleInfo* title);
  LibraryEntry();

  static QString initSave(QString id);
  void backupSave(QString saveTo);
  void ImportSave(QString filePath);

  QString directory;
  QString rpx;
  QString metaxml;
  TitleInfo* titleInfo;
};

#endif // LIBRARYENTRY_H
