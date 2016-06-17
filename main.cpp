#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;

  QIcon icon(":/icons/make.png");

  w.setWindowTitle("QtCrossword");
  w.setWindowIcon(icon);
  w.showNormal();

  return a.exec();
}
