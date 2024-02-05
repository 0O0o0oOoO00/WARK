#include "MainWindow.hpp"
#include "DC.hpp"
#include <QtWidgets/QApplication>

Driver* g_pDriver = NULL;

int main(int argc, char *argv[]) {

    g_pDriver = new Driver;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    int retval = a.exec();

    g_pDriver->Unload();

    return retval;
}
