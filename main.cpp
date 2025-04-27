#include <QApplication>
#include <QDebug>

#include "MainWindow/MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}