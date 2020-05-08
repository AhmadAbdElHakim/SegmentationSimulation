#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.setWindowState(Qt::WindowMaximized);
    w.show();

    QObject::connect(w.exit, SIGNAL(clicked()), &app, SLOT(quit()));

    return app.exec();
}
