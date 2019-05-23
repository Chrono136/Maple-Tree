#ifndef TITLEINFOITEM_H
#define TITLEINFOITEM_H

#include <QListWidgetItem>
#include "titleinfo.h"

class TitleInfoItem : public QListWidgetItem {
 public:
  TitleInfoItem(LibraryEntry* entry) {
    this->setText(entry->titleInfo->getFormatName());
    this->item = entry;
  }
  LibraryEntry* getItem() const { return this->item; }

 private:
	 LibraryEntry* item;
};

#endif  // TITLEINFOITEM_H
