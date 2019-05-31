#ifndef TITLEITEM_H
#define TITLEITEM_H

#include <QDialog>
#include <QMenu>
#include <QFileDialog>
#include "titleinfo.h"

namespace Ui {
class TitleItem;
}

class TitleItem : public QDialog
{
    Q_OBJECT

public:
    explicit TitleItem(QWidget *parent = nullptr);
    ~TitleItem();

    void init(QString id);

    int add(QString id);

    int modify(QString id);

    TitleInfo* getInfo(){return this->titleInfo;}

private slots:
    void on_buttonBox_accepted();

    void on_labelImage_customContextMenuRequested(const QPoint &pos);

    void on_lineEditID_textChanged(const QString &arg1);

private:
    Ui::TitleItem *ui;
    TitleInfo* titleInfo;
    QString imagePath;
};

#endif // TITLEITEM_H
