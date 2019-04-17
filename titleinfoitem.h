#ifndef TITLEINFOITEM_H
#define TITLEINFOITEM_H

#include <QListWidgetItem>
#include "titleinfo.h"

class TitleInfoItem : public QListWidgetItem {
 public:
  TitleInfoItem(TitleInfo *tb) {
    this->setText(tb->getFormatName());
    this->item = tb;
  }
  TitleInfo *getItem() const { return this->item; }

 private:
  TitleInfo *item;
};

#endif  // TITLEINFOITEM_H
