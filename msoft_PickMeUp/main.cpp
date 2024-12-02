#include <QApplication>
#include "mainwindow.h"

// Starting point of the program
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}