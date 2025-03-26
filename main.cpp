#include <QApplication>
#include <QSqlDatabase>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // Проверка драйверов
    qDebug() << "Available drivers:" << QSqlDatabase::drivers();

    MainWindow w;
    w.show();
    return a.exec();
}
