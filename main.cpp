#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  //QTextCodec *codec = QTextCodec::codecForName("UTF-8");
  //QTextCodec::setCodecForTr(codec);
  //QTextCodec::setCodecForCStrings(codec);

  MainWindow w;

  QIcon icon(":/icons/make.png");

  w.setWindowTitle("QtCrossword");
  w.setWindowIcon(icon);
  w.showMaximized();

  return a.exec();
}
