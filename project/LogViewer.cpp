// Week 7 — Model/View Programming

#include "LogViewer.h"
#include "LogModel.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMimeData>
#include <QPushButton>
#include <QTableView>
#include <QVBoxLayout>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

LogViewer::LogViewer(QWidget *parent)
    : QWidget(parent)
{
    // --- Model ---------------------------------------------------------------
    m_model = new LogModel(this);

    // --- Table view ----------------------------------------------------------
    m_tableView = new QTableView(this);
    m_tableView->setModel(m_model);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setSortingEnabled(true);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->horizontalHeader()->setStretchLastSection(true);

    // --- Bottom toolbar ------------------------------------------------------
    m_autoScrollButton = new QPushButton(tr("Auto-scroll: ON"), this);
    m_lineCountLabel   = new QLabel(tr("Lines: 0"), this);

    auto *toolbarLayout = new QHBoxLayout;
    toolbarLayout->addWidget(m_autoScrollButton);
    toolbarLayout->addStretch();
    toolbarLayout->addWidget(m_lineCountLabel);

    // --- Overall layout ------------------------------------------------------
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_tableView, /*stretch=*/1);
    mainLayout->addLayout(toolbarLayout);

    // --- Drag-and-drop -------------------------------------------------------
    setAcceptDrops(true);

    // --- Connections ---------------------------------------------------------
    connect(m_autoScrollButton, &QPushButton::clicked,
            this, &LogViewer::toggleAutoScroll);
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void LogViewer::loadFile(const QString &filePath)
{
    if (!m_model->loadFromFile(filePath)) {
        qWarning("LogViewer: could not open '%s'", qPrintable(filePath));
        return;
    }

    const int count = lineCount();
    m_lineCountLabel->setText(tr("Lines: %1").arg(count));

    emit fileLoaded(filePath);
    emit lineCountChanged(count);

    // Scroll to bottom if auto-scroll is enabled
    if (m_autoScroll) {
        m_tableView->scrollToBottom();
    }
}

int LogViewer::lineCount() const
{
    return m_model->entryCount();
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
