// Week 9 — Text Editor Core

#include "CodeEditor.h"

#include <QFile>
#include <QFontDatabase>
#include <QPainter>
#include <QTextBlock>
#include <QTextStream>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent)
{
    m_lineNumberArea = new LineNumberArea(this);

    // Monospace font
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setPointSize(11);
    setFont(font);

    // Tab stop = 4 spaces
    const int tabWidth = fontMetrics().horizontalAdvance(' ') * 4;
    setTabStopDistance(tabWidth);

    // Connections for line number area
    connect(this, &QPlainTextEdit::blockCountChanged,
            this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest,
            this, &CodeEditor::updateLineNumberArea);
    connect(this, &QPlainTextEdit::cursorPositionChanged,
            this, &CodeEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

// ---------------------------------------------------------------------------
// Line Number Area
// ---------------------------------------------------------------------------

int CodeEditor::lineNumberAreaWidth() const
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    // Minimum 3 digits wide + padding on each side
    digits = qMax(digits, 3);
    const int padding = fontMetrics().horizontalAdvance(' ');
    return padding + fontMetrics().horizontalAdvance('9') * digits + padding;
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(m_lineNumberArea);
    painter.fillRect(event->rect(), QColor(245, 245, 245));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    const QColor currentLineColor(100, 100, 100);
    const QColor otherLineColor(180, 180, 180);
    const int currentBlockNumber = textCursor().blockNumber();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            const QString number = QString::number(blockNumber + 1);

            painter.setPen(blockNumber == currentBlockNumber
                               ? currentLineColor : otherLineColor);
            painter.drawText(0, top,
                             m_lineNumberArea->width() - fontMetrics().horizontalAdvance(' '),
                             fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy) {
        m_lineNumberArea->scroll(0, dy);
    } else {
        m_lineNumberArea->update(0, rect.y(),
                                 m_lineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth(0);
    }
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(QColor(255, 255, 220));  // Light yellow
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

// ---------------------------------------------------------------------------
// Resize
// ---------------------------------------------------------------------------

void CodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);

    const QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(
        QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

// ---------------------------------------------------------------------------
// File Operations
// ---------------------------------------------------------------------------

bool CodeEditor::openFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("CodeEditor: could not open '%s': %s",
                 qPrintable(filePath),
                 qPrintable(file.errorString()));
        return false;
    }

    QTextStream in(&file);
    setPlainText(in.readAll());

    m_filePath = filePath;
    document()->setModified(false);
    emit fileOpened(filePath);
    return true;
}

bool CodeEditor::saveFile(const QString &filePath)
{
    const QString path = filePath.isEmpty() ? m_filePath : filePath;
    if (path.isEmpty()) {
        return false;
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning("CodeEditor: could not save '%s': %s",
                 qPrintable(path),
                 qPrintable(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out << toPlainText();

    m_filePath = path;
    document()->setModified(false);
    return true;
}

QString CodeEditor::currentFilePath() const
{
    return m_filePath;
}

bool CodeEditor::isModified() const
{
    return document()->isModified();
}
