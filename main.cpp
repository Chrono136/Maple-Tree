#include <QApplication>
#include <QtConcurrent>
#include "mapleseed.h"


int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  MapleSeed w;
  w.show();
  return a.exec();
}
