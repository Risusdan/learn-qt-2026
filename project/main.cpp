// Week 14 — Cross-Platform & Deployment

#include "MainWindow.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("DevConsole");
    app.setOrganizationName("LearnQt");

    // Use Fusion style for consistent cross-platform look.
    // Qt 6 handles high-DPI automatically — no opt-in needed.
    app.setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
