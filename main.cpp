#include "mapleseed.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MapleSeed w;
    w.show();

    return a.exec();
}
