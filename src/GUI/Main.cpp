#include "MainWindow.hpp"
#include "Collector.hpp"
#include "Utiler.hpp"
#include <QtWidgets/QApplication>

CollectorDriver* g_pCollectorDriver = NULL;
UtilerDriver* g_pUtilerDriver = NULL;

VOID UnloadAllDriver() {
    g_pCollectorDriver->Unload();
    g_pUtilerDriver->Unload();
}

VOID LoadAllDriver() {
    g_pCollectorDriver = new CollectorDriver;
    g_pUtilerDriver = new UtilerDriver;
}

int main(int argc, char *argv[]) {

    LoadAllDriver();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    int retval = a.exec();

    UnloadAllDriver();

    return retval;
}
