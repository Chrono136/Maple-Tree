#ifndef LIBRARYENTRY_H
#define LIBRARYENTRY_H

#include "titleinfo.h"

class LibraryEntry {
 public:
  LibraryEntry(TitleInfo* title);
  LibraryEntry();

  QString directory;
  QString rpx;
  QString metaxml;
  TitleInfo* titleInfo;
};

#endif // LIBRARYENTRY_H
