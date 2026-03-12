// Week 10 — Text Editor Features

#include "TextEditorTab.h"
#include "CodeEditor.h"
#include "CppHighlighter.h"
#include "FileBrowser.h"
#include "FindReplaceDialog.h"

#include <QFileInfo>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QSplitter>
#include <QSyntaxHighlighter>
#include <QTabWidget>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

TextEditorTab::TextEditorTab(QWidget *parent)
    : QWidget(parent)
{
    // --- File browser (left panel) ---
    m_fileBrowser = new FileBrowser(this);

    // --- Editor tabs (right panel) ---
    m_editorTabs = new QTabWidget(this);
    m_editorTabs->setTabsClosable(true);
    m_editorTabs->setMovable(true);

    // --- Splitter ---
    m_splitter = new QSplitter(Qt::Horizontal, this);
    m_splitter->addWidget(m_fileBrowser);
    m_splitter->addWidget(m_editorTabs);
    m_splitter->setStretchFactor(0, 0);  // File browser doesn't stretch
    m_splitter->setStretchFactor(1, 1);  // Editor takes remaining space
    m_splitter->setSizes({200, 600});

    // --- Find/Replace dialog (modeless) ---
    m_findReplaceDialog = new FindReplaceDialog(this);

    // --- Layout ---
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_splitter);

    // --- Connections ---
    connect(m_fileBrowser, &FileBrowser::fileActivated,
            this, &TextEditorTab::openFile);

    connect(m_editorTabs, &QTabWidget::tabCloseRequested,
            this, &TextEditorTab::onTabCloseRequested);

    connect(m_editorTabs, &QTabWidget::currentChanged,
            this, &TextEditorTab::onCurrentTabChanged);

    // Start with one empty tab
    newFile();
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void TextEditorTab::openFile(const QString &filePath)
{
    // Check if this file is already open — switch to it
    for (int i = 0; i < m_editorTabs->count(); ++i) {
        auto *editor = qobject_cast<CodeEditor *>(m_editorTabs->widget(i));
        if (editor && editor->currentFilePath() == filePath) {
            m_editorTabs->setCurrentIndex(i);
            return;
        }
    }

    // If the current tab is an empty, unmodified "Untitled" tab, reuse it
    auto *current = currentEditor();
    if (current && current->currentFilePath().isEmpty()
        && !current->isModified() && current->toPlainText().isEmpty()) {
        if (current->openFile(filePath)) {
            const QString name = QFileInfo(filePath).fileName();
            m_editorTabs->setTabText(m_editorTabs->currentIndex(), name);
            emit fileOpened(filePath);
        }
        return;
    }

    // Open in a new tab
    auto *editor = createEditorTab(QFileInfo(filePath).fileName());
    if (editor->openFile(filePath)) {
        m_editorTabs->setCurrentWidget(editor);
        emit fileOpened(filePath);
    }
}

void TextEditorTab::newFile()
{
    auto *editor = createEditorTab(tr("Untitled"));
    m_editorTabs->setCurrentWidget(editor);
}

bool TextEditorTab::saveCurrentFile()
{
    auto *editor = currentEditor();
    if (!editor) {
        return false;
    }

    if (editor->currentFilePath().isEmpty()) {
        return false;  // Caller should use saveCurrentFileAs()
    }

    if (editor->saveFile(editor->currentFilePath())) {
        emit fileSaved(editor->currentFilePath());
        return true;
    }
    return false;
}

bool TextEditorTab::saveCurrentFileAs(const QString &filePath)
{
    auto *editor = currentEditor();
    if (!editor || filePath.isEmpty()) {
        return false;
    }

    if (editor->saveFile(filePath)) {
        const QString name = QFileInfo(filePath).fileName();
        m_editorTabs->setTabText(m_editorTabs->currentIndex(), name);
        emit fileSaved(filePath);
        return true;
    }
    return false;
}

CodeEditor *TextEditorTab::currentEditor() const
{
    return qobject_cast<CodeEditor *>(m_editorTabs->currentWidget());
}

void TextEditorTab::showFindReplace()
{
    m_findReplaceDialog->setTextEdit(currentEditor());
    m_findReplaceDialog->show();
    m_findReplaceDialog->raise();
    m_findReplaceDialog->activateWindow();
}

// ---------------------------------------------------------------------------
// Private Slots
// ---------------------------------------------------------------------------

void TextEditorTab::onTabCloseRequested(int index)
{
    auto *editor = qobject_cast<CodeEditor *>(m_editorTabs->widget(index));
    if (!editor) {
        return;
    }

    // Prompt to save if modified
    if (editor->isModified()) {
        const QString name = m_editorTabs->tabText(index);
        auto result = QMessageBox::question(
            this,
            tr("Save Changes"),
            tr("Do you want to save changes to \"%1\"?").arg(name),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (result == QMessageBox::Cancel) {
            return;
        }
        if (result == QMessageBox::Save) {
            if (!editor->currentFilePath().isEmpty()) {
                editor->saveFile(editor->currentFilePath());
            }
            // If no file path, user needs to Save As — skip for simplicity
        }
    }

    m_editorTabs->removeTab(index);
    editor->deleteLater();

    // If no tabs remain, create a new empty one
    if (m_editorTabs->count() == 0) {
        newFile();
    }
}

void TextEditorTab::onCurrentTabChanged(int index)
{
    Q_UNUSED(index);

    auto *editor = currentEditor();
    if (editor) {
        m_findReplaceDialog->setTextEdit(editor);
        emit currentFileChanged(editor->currentFilePath());
    }
}

// ---------------------------------------------------------------------------
// Private Helpers
// ---------------------------------------------------------------------------

CodeEditor *TextEditorTab::createEditorTab(const QString &title)
{
    auto *editor = new CodeEditor(m_editorTabs);

    // Apply syntax highlighter for C/C++ files
    const QString lower = title.toLower();
    if (lower.endsWith(".h") || lower.endsWith(".cpp") || lower.endsWith(".c")) {
        new CppHighlighter(editor->document());
    }

    const int tabIndex = m_editorTabs->addTab(editor, title);
    m_editorTabs->setCurrentIndex(tabIndex);

    // Connect fileOpened to apply highlighter when a file is opened later
    connect(editor, &CodeEditor::fileOpened, this, [this, editor](const QString &filePath) {
        const QString ext = QFileInfo(filePath).suffix().toLower();
        // Check if a highlighter is already attached
        if (!editor->document()->findChild<QSyntaxHighlighter *>()) {
            if (ext == "h" || ext == "cpp" || ext == "c") {
                new CppHighlighter(editor->document());
            }
        }
    });

    return editor;
}
