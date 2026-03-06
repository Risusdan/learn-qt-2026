// Week 10 — Text Editor Features

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>

class QAction;
class QTabWidget;
class QMenu;
class LogViewer;
class TextEditorTab;

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
    void onSaveFileAs();
    void onFindReplace();
    void onAbout();

private:
    void setupMenus();
    void setupTabs();
    void loadSettings();
    void saveSettings();
    void updateRecentFilesMenu();
    void addToRecentFiles(const QString &filePath);

    QTabWidget     *m_tabWidget        = nullptr;
    QMenu          *m_recentFilesMenu  = nullptr;
    QStringList     m_recentFiles;
    LogViewer      *m_logViewer        = nullptr;
    TextEditorTab  *m_textEditorTab    = nullptr;
    QAction        *m_undoAction       = nullptr;
    QAction        *m_redoAction       = nullptr;

    static constexpr int MaxRecentFiles = 5;
};

#endif // MAINWINDOW_H
