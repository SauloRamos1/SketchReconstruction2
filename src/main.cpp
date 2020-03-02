#include <QtWidgets/QApplication>

#include "include/main_window.h"


int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    MainWindow window;


    window.setFixedSize(1280,720);

    //window.showMaximized();
    window.show();


    return app.exec();
}
