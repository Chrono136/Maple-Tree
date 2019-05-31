#ifndef TITLEINFOITEM_H
#define TITLEINFOITEM_H

#include <QListWidgetItem>
#include "titleinfo.h"
#include "libraryentry.h"

class TitleInfoItem : public QListWidgetItem {
 public:
  TitleInfoItem(LibraryEntry* entry) {
    this->setText(entry->titleInfo->getFormatName());
    this->item = entry;
  }
  LibraryEntry* getItem() const { return this->item; }
  void setItem(LibraryEntry* itm) { item = itm; }

 private:
	 LibraryEntry* item;
};

#endif  // TITLEINFOITEM_H
