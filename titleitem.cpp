#include "titleitem.h"
#include "ui_titleitem.h"
#include "gamelibrary.h"

TitleItem::TitleItem(QWidget *parent) : QDialog(parent), ui(new Ui::TitleItem)
{
    ui->setupUi(this);
}

TitleItem::~TitleItem()
{
    delete ui;
}

void TitleItem::init(QString id)
{
    QMessageBox::information(nullptr, "Notice!", "No checks are ran for these values, please verify your inputs before accepting");
    titleInfo = TitleInfo::Create(id, GameLibrary::self->baseDirectory);
}

int TitleItem::add(QString id)
{
    init(id);
    ui->labelImage->setPixmap(QPixmap("covers/!.jpg"));
    return this->exec();
}

int TitleItem::modify(QString id)
{
    init(id);
    if (ui == nullptr) return -1;
    if (titleInfo == nullptr) return -1;
    ui->lineEditID->setEnabled(false);
    ui->lineEditID->setText(titleInfo->getID());
    ui->lineEditKey->setText(titleInfo->getKey());
    ui->lineEditName->setText(titleInfo->getName());
    ui->lineEditRegion->setText(titleInfo->getRegion());
    ui->lineEditProductcode->setText(titleInfo->info["productcode"]);
    ui->labelImage->setPixmap(QPixmap(titleInfo->getCoverArtPath()));
    return this->exec();
}

void TitleItem::on_buttonBox_accepted()
{
    titleInfo->info["id"] = ui->lineEditID->text().toUpper();
    titleInfo->info["key"] = ui->lineEditKey->text().toUpper();
    titleInfo->info["name"] = ui->lineEditName->text();
    titleInfo->info["region"] = ui->lineEditRegion->text().toUpper();
    titleInfo->info["productcode"] = ui->lineEditProductcode->text().toUpper();

    if (titleInfo->info["productcode"].length() < 10)
        titleInfo->info["productcode"] = "000-0-0000";

    QString coverpath = QFileInfo("covers/"+titleInfo->getProductCode()+".jpg").absoluteFilePath();
    if (QFile::exists(coverpath))
        QFile::remove(coverpath);
    QFile::copy(imagePath, coverpath);
}

void TitleItem::on_labelImage_customContextMenuRequested(const QPoint &pos)
{
    QPoint globalPos = ui->labelImage->mapToGlobal(pos);

    QMenu menu;
    menu.addAction("Update Image", this, [&] {
        QFileDialog dialog;
        dialog.setNameFilter("*.jpg");
        if (dialog.exec()) {
            QStringList entries(dialog.selectedFiles());
            ui->labelImage->setPixmap(QPixmap(imagePath = entries.first()));
        }
    })->setEnabled(true);
    menu.exec(globalPos);
}

void TitleItem::on_lineEditID_textChanged(const QString &arg1)
{
    ui->labelID->setText(arg1);
}
