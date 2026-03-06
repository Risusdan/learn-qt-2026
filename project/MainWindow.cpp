// Week 14 — Cross-Platform & Deployment

#include "MainWindow.h"
#include "CodeEditor.h"
#include "LogViewer.h"
#include "SerialMonitorTab.h"
#include "TextEditorTab.h"

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
#include <QSysInfo>
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

    auto *saveAction = new QAction(tr("&Save"), this);
    saveAction->setShortcut(QKeySequence(tr("Ctrl+S")));
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSaveFile);
    fileMenu->addAction(saveAction);

    auto *saveAsAction = new QAction(tr("Save &As..."), this);
    saveAsAction->setShortcut(QKeySequence(tr("Ctrl+Shift+S")));
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::onSaveFileAs);
    fileMenu->addAction(saveAsAction);

    fileMenu->addSeparator();

    m_recentFilesMenu = fileMenu->addMenu(tr("Recent &Files"));

    fileMenu->addSeparator();

    auto *exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(QKeySequence(tr("Ctrl+Q")));
    connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);
    fileMenu->addAction(exitAction);

    // ---- Edit menu --------------------------------------------------------
    auto *editMenu = menuBar()->addMenu(tr("&Edit"));

    m_undoAction = new QAction(tr("&Undo"), this);
    m_undoAction->setShortcut(QKeySequence(tr("Ctrl+Z")));
    connect(m_undoAction, &QAction::triggered, this, [this]() {
        if (auto *editor = m_textEditorTab->currentEditor()) {
            editor->undo();
        }
    });
    editMenu->addAction(m_undoAction);

    m_redoAction = new QAction(tr("&Redo"), this);
    m_redoAction->setShortcut(QKeySequence(tr("Ctrl+Y")));
    connect(m_redoAction, &QAction::triggered, this, [this]() {
        if (auto *editor = m_textEditorTab->currentEditor()) {
            editor->redo();
        }
    });
    editMenu->addAction(m_redoAction);

    editMenu->addSeparator();

    auto *findReplaceAction = new QAction(tr("&Find/Replace..."), this);
    findReplaceAction->setShortcut(QKeySequence(tr("Ctrl+F")));
    connect(findReplaceAction, &QAction::triggered,
            this, &MainWindow::onFindReplace);
    editMenu->addAction(findReplaceAction);

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

    // Tab 1 — Log Viewer
    m_logViewer = new LogViewer(m_tabWidget);
    m_tabWidget->addTab(m_logViewer, tr("Log Viewer"));

    connect(m_logViewer, &LogViewer::fileLoaded, this, [this](const QString &filePath) {
        const QString fileName = QFileInfo(filePath).fileName();
        setWindowTitle(tr("DevConsole — %1").arg(fileName));
        addToRecentFiles(filePath);
    });

    connect(m_logViewer, &LogViewer::lineCountChanged, this, [this](int count) {
        statusBar()->showMessage(tr("Lines: %1").arg(count));
    });

    // Tab 2 — Text Editor
    m_textEditorTab = new TextEditorTab(m_tabWidget);
    m_tabWidget->addTab(m_textEditorTab, tr("Text Editor"));

    connect(m_textEditorTab, &TextEditorTab::fileOpened, this, [this](const QString &filePath) {
        const QString fileName = QFileInfo(filePath).fileName();
        setWindowTitle(tr("DevConsole — %1").arg(fileName));
        addToRecentFiles(filePath);
    });

    connect(m_textEditorTab, &TextEditorTab::fileSaved, this, [this](const QString &filePath) {
        const QString fileName = QFileInfo(filePath).fileName();
        statusBar()->showMessage(tr("Saved: %1").arg(fileName), 3000);
    });

    // Tab 3 — Serial Monitor
    m_serialMonitorTab = new SerialMonitorTab(m_tabWidget);
    m_tabWidget->addTab(m_serialMonitorTab, tr("Serial Monitor"));

    connect(m_serialMonitorTab, &SerialMonitorTab::connectionChanged,
            this, [this](bool connected, const QString &portName) {
        if (connected) {
            statusBar()->showMessage(tr("Connected: %1").arg(portName));
        } else {
            statusBar()->showMessage(tr("Disconnected"), 3000);
        }
    });
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
            // Route by extension: .log → LogViewer, everything else → TextEditor
            if (filePath.endsWith(".log", Qt::CaseInsensitive)) {
                m_logViewer->loadFile(filePath);
                m_tabWidget->setCurrentWidget(m_logViewer);
            } else {
                m_textEditorTab->openFile(filePath);
                m_tabWidget->setCurrentWidget(m_textEditorTab);
            }
        });
        m_recentFilesMenu->addAction(action);
    }
}

void MainWindow::addToRecentFiles(const QString &filePath)
{
    m_recentFiles.removeAll(filePath);
    m_recentFiles.prepend(filePath);

    while (m_recentFiles.size() > MaxRecentFiles) {
        m_recentFiles.removeLast();
    }

    updateRecentFilesMenu();
}

// ---------------------------------------------------------------------------
// Slot Implementations
// ---------------------------------------------------------------------------

void MainWindow::onNewFile()
{
    m_textEditorTab->newFile();
    m_tabWidget->setCurrentWidget(m_textEditorTab);
}

void MainWindow::onOpenFile()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Open File"),
        QString(),
        tr("All Files (*);;C++ Files (*.h *.cpp *.c);;Text Files (*.txt);;Log Files (*.log)"));

    if (filePath.isEmpty()) {
        return;
    }

    addToRecentFiles(filePath);

    // Route by active tab or file extension
    if (filePath.endsWith(".log", Qt::CaseInsensitive)
        || m_tabWidget->currentWidget() == m_logViewer) {
        m_logViewer->loadFile(filePath);
        m_tabWidget->setCurrentWidget(m_logViewer);
    } else {
        m_textEditorTab->openFile(filePath);
        m_tabWidget->setCurrentWidget(m_textEditorTab);
    }
}

void MainWindow::onSaveFile()
{
    if (m_tabWidget->currentWidget() == m_textEditorTab) {
        if (!m_textEditorTab->saveCurrentFile()) {
            onSaveFileAs();
        }
    }
}

void MainWindow::onSaveFileAs()
{
    if (m_tabWidget->currentWidget() != m_textEditorTab) {
        return;
    }

    const QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Save File As"),
        QString(),
        tr("All Files (*);;C++ Files (*.h *.cpp *.c);;Text Files (*.txt)"));

    if (filePath.isEmpty()) {
        return;
    }

    m_textEditorTab->saveCurrentFileAs(filePath);
    addToRecentFiles(filePath);
}

void MainWindow::onFindReplace()
{
    m_tabWidget->setCurrentWidget(m_textEditorTab);
    m_textEditorTab->showFindReplace();
}

void MainWindow::onAbout()
{
    QMessageBox::about(
        this,
        tr("About DevConsole"),
        tr("<h3>DevConsole</h3>"
           "<p>A multi-tab developer console for log viewing, "
           "text editing, and serial monitoring.</p>"
           "<p>Built with Qt %1 &amp; C++17.</p>"
           "<p>Platform: %2</p>"
           "<p>Architecture: %3</p>"
           "<p>Week 14 — Cross-Platform &amp; Deployment</p>")
            .arg(qVersion(),
                 QSysInfo::prettyProductName(),
                 QSysInfo::currentCpuArchitecture()));
}
