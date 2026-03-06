// Week 6 — Log Viewer Basic

#include "LogViewer.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFile>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QMimeData>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QTextStream>
#include <QVBoxLayout>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

LogViewer::LogViewer(QWidget *parent)
    : QWidget(parent)
{
    // --- Main text display ---------------------------------------------------
    m_textDisplay = new QPlainTextEdit(this);
    m_textDisplay->setReadOnly(true);
    m_textDisplay->setLineWrapMode(QPlainTextEdit::NoWrap);
    m_textDisplay->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    m_textDisplay->setMaximumBlockCount(50000);

    // --- Bottom toolbar ------------------------------------------------------
    m_autoScrollButton = new QPushButton(tr("Auto-scroll: ON"), this);
    m_lineCountLabel   = new QLabel(tr("Lines: 0"), this);

    auto *toolbarLayout = new QHBoxLayout;
    toolbarLayout->addWidget(m_autoScrollButton);
    toolbarLayout->addStretch();
    toolbarLayout->addWidget(m_lineCountLabel);

    // --- Overall layout ------------------------------------------------------
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_textDisplay, /*stretch=*/1);
    mainLayout->addLayout(toolbarLayout);

    // --- Drag-and-drop -------------------------------------------------------
    setAcceptDrops(true);

    // --- Connections ---------------------------------------------------------
    connect(m_autoScrollButton, &QPushButton::clicked,
            this, &LogViewer::toggleAutoScroll);

    connect(m_textDisplay->verticalScrollBar(), &QScrollBar::valueChanged,
            this, &LogViewer::onScrollChanged);
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void LogViewer::loadFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("LogViewer: could not open '%s': %s",
                 qPrintable(filePath),
                 qPrintable(file.errorString()));
        return;
    }

    m_textDisplay->clear();

    QTextStream in(&file);
    while (!in.atEnd()) {
        m_textDisplay->appendPlainText(in.readLine());
    }

    const int count = lineCount();
    m_lineCountLabel->setText(tr("Lines: %1").arg(count));

    emit fileLoaded(filePath);
    emit lineCountChanged(count);

    // Scroll to the bottom if auto-scroll is enabled
    if (m_autoScroll) {
        auto *scrollBar = m_textDisplay->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
    }
}

int LogViewer::lineCount() const
{
    return m_textDisplay->document()->blockCount();
}

// ---------------------------------------------------------------------------
// Drag-and-Drop
// ---------------------------------------------------------------------------

void LogViewer::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void LogViewer::dropEvent(QDropEvent *event)
{
    const QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) {
        return;
    }

    const QString localPath = urls.first().toLocalFile();
    if (!localPath.isEmpty()) {
        loadFile(localPath);
    }
}

// ---------------------------------------------------------------------------
// Private Slots
// ---------------------------------------------------------------------------

void LogViewer::toggleAutoScroll()
{
    m_autoScroll = !m_autoScroll;
    m_autoScrollButton->setText(
        m_autoScroll ? tr("Auto-scroll: ON") : tr("Auto-scroll: OFF"));
}

void LogViewer::onScrollChanged(int value)
{
    const auto *scrollBar = m_textDisplay->verticalScrollBar();
    constexpr int threshold = 10;

    // Re-enable auto-scroll when user scrolls to the bottom
    const bool atBottom = (value >= scrollBar->maximum() - threshold);

    if (atBottom != m_autoScroll) {
        m_autoScroll = atBottom;
        m_autoScrollButton->setText(
            m_autoScroll ? tr("Auto-scroll: ON") : tr("Auto-scroll: OFF"));
    }
}
