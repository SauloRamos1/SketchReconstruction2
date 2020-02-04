#include <QtWidgets/QApplication>

#include "include/main_window.h"


int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    MainWindow window;


    window.showMaximized();


    return app.exec();
}
