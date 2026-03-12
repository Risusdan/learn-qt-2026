// Week 10 — Text Editor Features

#include "FileBrowser.h"

#include <QDir>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QHeaderView>
#include <QLineEdit>
#include <QTreeView>
#include <QVBoxLayout>

FileBrowser::FileBrowser(QWidget *parent)
    : QWidget(parent)
{
    // --- Model ---
    m_model = new QFileSystemModel(this);
    m_model->setNameFilters({
        "*.h", "*.cpp", "*.c", "*.txt", "*.log",
        "*.md", "*.cmake", "CMakeLists.txt"
    });
    m_model->setNameFilterDisables(false);  // Hide non-matching files

    // --- Tree view ---
    m_treeView = new QTreeView(this);
    m_treeView->setModel(m_model);
    m_treeView->setHeaderHidden(false);

    // Show only the Name column
    m_treeView->setColumnHidden(1, true);  // Size
    m_treeView->setColumnHidden(2, true);  // Type
    m_treeView->setColumnHidden(3, true);  // Date Modified

    m_treeView->setAnimated(true);
    m_treeView->setSortingEnabled(true);
    m_treeView->sortByColumn(0, Qt::AscendingOrder);

    // --- Path bar ---
    m_pathBar = new QLineEdit(this);
    m_pathBar->setPlaceholderText(tr("Enter path..."));

    // --- Layout ---
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_pathBar);
    layout->addWidget(m_treeView, /*stretch=*/1);

    // --- Connections ---
    // Double-click on a file emits fileActivated
    connect(m_treeView, &QTreeView::doubleClicked, this,
            [this](const QModelIndex &index) {
                if (!m_model->isDir(index)) {
                    emit fileActivated(m_model->filePath(index));
                }
            });

    // Path bar navigation: press Enter to change root
    connect(m_pathBar, &QLineEdit::returnPressed, this, [this]() {
        const QString path = m_pathBar->text();
        if (QFileInfo::exists(path)) {
            setRootPath(path);
        }
    });

    // Default root: home directory
    setRootPath(QDir::homePath());
}

void FileBrowser::setRootPath(const QString &path)
{
    m_model->setRootPath(path);
    m_treeView->setRootIndex(m_model->index(path));
    m_pathBar->setText(path);
}

QString FileBrowser::rootPath() const
{
    return m_model->rootPath();
}
