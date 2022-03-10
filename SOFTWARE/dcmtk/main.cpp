#include "mainwindow.h"
#include <QApplication>

#include <QFile>
#include <QDebug>


QByteArray readTextFile(const QString &file_path) {
  QFile input_file(file_path);
  QByteArray input_data;

  if (input_file.open(QIODevice::Text | QIODevice::Unbuffered | QIODevice::ReadOnly)) {
    input_data = input_file.readAll();
    input_file.close();
    return input_data;
  }
  else {
    return QByteArray();
  }
}


int main(int argc, char *argv[])
{
  QString style_sheet = readTextFile(":/rc/dark_blue.qss");

  qDebug() << style_sheet;

  QApplication a(argc, argv);
  MainWindow w;

  a.setStyleSheet(style_sheet);
  w.showMaximized();

  return a.exec();
}
