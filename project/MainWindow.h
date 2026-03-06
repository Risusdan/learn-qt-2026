// Week 9 — Text Editor Core

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>

class QTabWidget;
class QMenu;
class LogViewer;
class CodeEditor;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onNewFile();
    void onOpenFile();
    void onSaveFile();
    void onAbout();

private:
    void setupMenus();
    void setupTabs();
    void loadSettings();
    void saveSettings();
    void updateRecentFilesMenu();

    QTabWidget  *m_tabWidget       = nullptr;
    QMenu       *m_recentFilesMenu = nullptr;
    QStringList  m_recentFiles;
    LogViewer   *m_logViewer       = nullptr;
    CodeEditor  *m_codeEditor      = nullptr;

    static constexpr int MaxRecentFiles = 5;
};

#endif // MAINWINDOW_H
