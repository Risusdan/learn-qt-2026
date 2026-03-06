// Week 5 — File I/O & Dialogs

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>

class QTabWidget;
class QMenu;

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

    static constexpr int MaxRecentFiles = 5;
};

#endif // MAINWINDOW_H
