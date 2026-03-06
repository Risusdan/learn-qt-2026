// Week 5 — File I/O & Dialogs

#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("DevConsole");
    app.setOrganizationName("LearnQt");

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
