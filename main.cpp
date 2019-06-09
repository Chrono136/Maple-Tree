#include <QApplication>
#include <QtConcurrent>
#include "mapleseed.h"
#include "debug.h"

int main(int argc, char* argv[]) {
    qInstallMessageHandler(Debug::messageOutput);
    QLoggingCategory::installFilter(Debug::categoryFilter);
    QApplication a(argc, argv);
    MapleSeed w;
    w.show();
    return a.exec();
}
