// Week 10 — Text Editor Features

#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QWidget>

class QFileSystemModel;
class QTreeView;
class QLineEdit;

class FileBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit FileBrowser(QWidget *parent = nullptr);

    void setRootPath(const QString &path);
    QString rootPath() const;

signals:
    void fileActivated(const QString &filePath);

private:
    QFileSystemModel *m_model    = nullptr;
    QTreeView        *m_treeView = nullptr;
    QLineEdit        *m_pathBar  = nullptr;
};

#endif // FILEBROWSER_H
