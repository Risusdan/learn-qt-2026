// Week 9 — Text Editor Core

#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);

    // Line number area
    int lineNumberAreaWidth() const;
    void lineNumberAreaPaintEvent(QPaintEvent *event);

    // File operations
    bool openFile(const QString &filePath);
    bool saveFile(const QString &filePath);
    QString currentFilePath() const;
    bool isModified() const;

signals:
    void fileOpened(const QString &filePath);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);
    void highlightCurrentLine();

private:
    class LineNumberArea;  // Nested class — forward decl
    LineNumberArea *m_lineNumberArea = nullptr;
    QString m_filePath;
};

// Nested class — thin widget that delegates painting to CodeEditor
class CodeEditor::LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(CodeEditor *editor)
        : QWidget(editor), m_codeEditor(editor) {}

    QSize sizeHint() const override
    {
        return QSize(m_codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        m_codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *m_codeEditor;
};

#endif // CODEEDITOR_H
