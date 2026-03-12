// Week 10 — Text Editor Features

#ifndef TEXTEDITORTAB_H
#define TEXTEDITORTAB_H

#include <QWidget>

class CodeEditor;
class CppHighlighter;
class FileBrowser;
class FindReplaceDialog;
class QSplitter;
class QTabWidget;

class TextEditorTab : public QWidget
{
    Q_OBJECT

public:
    explicit TextEditorTab(QWidget *parent = nullptr);

    void openFile(const QString &filePath);
    void newFile();
    bool saveCurrentFile();
    bool saveCurrentFileAs(const QString &filePath);
    CodeEditor *currentEditor() const;
    void showFindReplace();

signals:
    void fileOpened(const QString &filePath);
    void fileSaved(const QString &filePath);
    void currentFileChanged(const QString &filePath);

private slots:
    void onTabCloseRequested(int index);
    void onCurrentTabChanged(int index);

private:
    CodeEditor *createEditorTab(const QString &title);

    QSplitter        *m_splitter        = nullptr;
    FileBrowser      *m_fileBrowser     = nullptr;
    QTabWidget       *m_editorTabs      = nullptr;
    FindReplaceDialog *m_findReplaceDialog = nullptr;
};

#endif // TEXTEDITORTAB_H
