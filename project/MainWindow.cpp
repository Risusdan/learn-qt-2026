// Week 7 — Model/View Programming

#include "MainWindow.h"
#include "LogViewer.h"

#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>
#include <QStatusBar>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

// ---------------------------------------------------------------------------
// Construction / Destruction
// ---------------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("DevConsole");
    setMinimumSize(800, 600);

    setupMenus();
    setupTabs();
    loadSettings();
}

MainWindow::~MainWindow()
{
    saveSettings();
}

// ---------------------------------------------------------------------------
// Menu Setup
// ---------------------------------------------------------------------------

void MainWindow::setupMenus()
{
    // ---- File menu --------------------------------------------------------
    auto *fileMenu = menuBar()->addMenu(tr("&File"));

    auto *newAction = new QAction(tr("&New"), this);
    newAction->setShortcut(QKeySequence(tr("Ctrl+N")));
    connect(newAction, &QAction::triggered, this, &MainWindow::onNewFile);
    fileMenu->addAction(newAction);

    auto *openAction = new QAction(tr("&Open..."), this);
    openAction->setShortcut(QKeySequence(tr("Ctrl+O")));
    connect(openAction, &QAction::triggered, this, &MainWindow::onOpenFile);
    fileMenu->addAction(openAction);

    auto *saveAction = new QAction(tr("&Save As..."), this);
    saveAction->setShortcut(QKeySequence(tr("Ctrl+S")));
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSaveFile);
    fileMenu->addAction(saveAction);

    fileMenu->addSeparator();

    m_recentFilesMenu = fileMenu->addMenu(tr("Recent &Files"));

    fileMenu->addSeparator();

    auto *exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(QKeySequence(tr("Ctrl+Q")));
    connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);
    fileMenu->addAction(exitAction);

    // ---- Help menu --------------------------------------------------------
    auto *helpMenu = menuBar()->addMenu(tr("&Help"));

    auto *aboutAction = new QAction(tr("&About"), this);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
    helpMenu->addAction(aboutAction);
}

// ---------------------------------------------------------------------------
// Tab Setup
// ---------------------------------------------------------------------------

void MainWindow::setupTabs()
{
    m_tabWidget = new QTabWidget(this);
    setCentralWidget(m_tabWidget);

    // Tab 1 — Log Viewer (Week 6)
    m_logViewer = new LogViewer(m_tabWidget);
    m_tabWidget->addTab(m_logViewer, tr("Log Viewer"));

    // Update window title when a file is loaded
    connect(m_logViewer, &LogViewer::fileLoaded, this, [this](const QString &filePath) {
        const QString fileName = QFileInfo(filePath).fileName();
        setWindowTitle(tr("DevConsole \u2014 %1").arg(fileName));
    });

    // Show line count in the status bar when it changes
    connect(m_logViewer, &LogViewer::lineCountChanged, this, [this](int count) {
        statusBar()->showMessage(tr("Lines: %1").arg(count));
    });

    // Tab 2 — Text Editor (placeholder)
    auto *textEditorTab = new QWidget(m_tabWidget);
    auto *textLayout    = new QVBoxLayout(textEditorTab);
    auto *textLabel     = new QLabel(tr("Text Editor \u2014 Coming Week 9"), textEditorTab);
    textLabel->setAlignment(Qt::AlignCenter);
    textLayout->addWidget(textLabel);
    m_tabWidget->addTab(textEditorTab, tr("Text Editor"));

    // Tab 3 — Serial Monitor (placeholder)
    auto *serialMonitorTab = new QWidget(m_tabWidget);
    auto *serialLayout     = new QVBoxLayout(serialMonitorTab);
    auto *serialLabel      = new QLabel(tr("Serial Monitor \u2014 Coming Week 11"), serialMonitorTab);
    serialLabel->setAlignment(Qt::AlignCenter);
    serialLayout->addWidget(serialLabel);
    m_tabWidget->addTab(serialMonitorTab, tr("Serial Monitor"));
}

// ---------------------------------------------------------------------------
// Settings Persistence
// ---------------------------------------------------------------------------

void MainWindow::loadSettings()
{
    const QString configPath =
        QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

    QSettings settings(configPath + "/DevConsole.ini", QSettings::IniFormat);

    settings.beginGroup("MainWindow");
    const QPoint pos  = settings.value("pos", QPoint(200, 200)).toPoint();
    const QSize  size = settings.value("size", QSize(800, 600)).toSize();
    settings.endGroup();

    move(pos);
    resize(size);

    settings.beginGroup("RecentFiles");
    m_recentFiles = settings.value("files").toStringList();
    settings.endGroup();

    // Trim the list to the maximum allowed entries
    while (m_recentFiles.size() > MaxRecentFiles) {
        m_recentFiles.removeLast();
    }

    updateRecentFilesMenu();
}

void MainWindow::saveSettings()
{
    const QString configPath =
        QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

    QSettings settings(configPath + "/DevConsole.ini", QSettings::IniFormat);

    settings.beginGroup("MainWindow");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.endGroup();

    settings.beginGroup("RecentFiles");
    settings.setValue("files", m_recentFiles);
    settings.endGroup();
}

// ---------------------------------------------------------------------------
// Recent-Files Menu
// ---------------------------------------------------------------------------

void MainWindow::updateRecentFilesMenu()
{
    m_recentFilesMenu->clear();

    if (m_recentFiles.isEmpty()) {
        auto *emptyAction = new QAction(tr("(No recent files)"), m_recentFilesMenu);
        emptyAction->setEnabled(false);
        m_recentFilesMenu->addAction(emptyAction);
        return;
    }

    for (const QString &filePath : std::as_const(m_recentFiles)) {
        const QString displayName = QFileInfo(filePath).fileName();
        auto *action = new QAction(displayName, m_recentFilesMenu);
        action->setData(filePath);
        connect(action, &QAction::triggered, this, [this, filePath]() {
            m_logViewer->loadFile(filePath);
            m_tabWidget->setCurrentWidget(m_logViewer);
        });
        m_recentFilesMenu->addAction(action);
    }
}

// ---------------------------------------------------------------------------
// Slot Implementations
// ---------------------------------------------------------------------------

void MainWindow::onNewFile()
{
    QMessageBox::information(
        this,
        tr("New File"),
        tr("New file \u2014 not implemented yet."));
}

void MainWindow::onOpenFile()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Open File"),
        QString(),
        tr("All Files (*);;Text Files (*.txt);;Log Files (*.log)"));

    if (filePath.isEmpty()) {
        return;
    }

    // Add to recent files (remove duplicate first, then prepend)
    m_recentFiles.removeAll(filePath);
    m_recentFiles.prepend(filePath);

    while (m_recentFiles.size() > MaxRecentFiles) {
        m_recentFiles.removeLast();
    }

    updateRecentFilesMenu();

    // Load the file into the LogViewer and switch to its tab
    m_logViewer->loadFile(filePath);
    m_tabWidget->setCurrentWidget(m_logViewer);
}

void MainWindow::onSaveFile()
{
    const QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Save File"),
        QString(),
        tr("All Files (*);;Text Files (*.txt);;Log Files (*.log)"));

    if (filePath.isEmpty()) {
        return;
    }

    QMessageBox::information(
        this,
        tr("File Saved"),
        tr("Would save to: %1").arg(filePath));
}

void MainWindow::onAbout()
{
    QMessageBox::about(
        this,
        tr("About DevConsole"),
        tr("<h3>DevConsole</h3>"
           "<p>A multi-tab developer console for log viewing, "
           "text editing, and serial monitoring.</p>"
           "<p>Built with Qt 6 &amp; C++17.</p>"
           "<p>Week 7 — Model/View Programming</p>"));
}
