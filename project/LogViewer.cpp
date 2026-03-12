// Week 8 — Filtering, Delegates & Search

#include "LogViewer.h"
#include "LogModel.h"
#include "LogFilterProxy.h"
#include "LogDelegate.h"

#include <QComboBox>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
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
    // --- Model + Proxy -------------------------------------------------------
    m_model      = new LogModel(this);
    m_proxyModel = new LogFilterProxy(this);
    m_proxyModel->setSourceModel(m_model);

    // --- Delegate ------------------------------------------------------------
    m_delegate = new LogDelegate(this);

    // --- Table view ----------------------------------------------------------
    m_tableView = new QTableView(this);
    m_tableView->setModel(m_proxyModel);
    m_tableView->setItemDelegate(m_delegate);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setSortingEnabled(true);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->horizontalHeader()->setStretchLastSection(true);

    // --- Filter toolbar (top) ------------------------------------------------
    m_levelCombo = new QComboBox(this);
    m_levelCombo->addItems({tr("All"), tr("ERROR"), tr("WARN"), tr("INFO"), tr("DEBUG")});

    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText(tr("Search..."));
    m_searchEdit->setClearButtonEnabled(true);

    auto *filterLayout = new QHBoxLayout;
    filterLayout->addWidget(new QLabel(tr("Level:"), this));
    filterLayout->addWidget(m_levelCombo);
    filterLayout->addSpacing(12);
    filterLayout->addWidget(new QLabel(tr("Search:"), this));
    filterLayout->addWidget(m_searchEdit, /*stretch=*/1);

    // --- Bottom toolbar ------------------------------------------------------
    m_autoScrollButton = new QPushButton(tr("Auto-scroll: ON"), this);
    m_lineCountLabel   = new QLabel(tr("Lines: 0"), this);

    auto *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(m_autoScrollButton);
    bottomLayout->addStretch();
    bottomLayout->addWidget(m_lineCountLabel);

    // --- Overall layout ------------------------------------------------------
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(filterLayout);
    mainLayout->addWidget(m_tableView, /*stretch=*/1);
    mainLayout->addLayout(bottomLayout);

    // --- Drag-and-drop -------------------------------------------------------
    setAcceptDrops(true);

    // --- Connections ---------------------------------------------------------
    connect(m_autoScrollButton, &QPushButton::clicked,
            this, &LogViewer::toggleAutoScroll);

    connect(m_levelCombo, &QComboBox::currentTextChanged,
            m_proxyModel, &LogFilterProxy::setLevelFilter);

    connect(m_searchEdit, &QLineEdit::textChanged,
            this, [this](const QString &text) {
                m_proxyModel->setTextFilter(text);
                m_delegate->setSearchTerm(text);
                m_tableView->viewport()->update();
                updateLineCount();
            });

    // Update line count when filtering changes
    connect(m_proxyModel, &QAbstractItemModel::rowsInserted,
            this, &LogViewer::updateLineCount);
    connect(m_proxyModel, &QAbstractItemModel::rowsRemoved,
            this, &LogViewer::updateLineCount);
    connect(m_proxyModel, &QAbstractItemModel::modelReset,
            this, &LogViewer::updateLineCount);
    connect(m_proxyModel, &QAbstractItemModel::layoutChanged,
            this, &LogViewer::updateLineCount);
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

    updateLineCount();

    emit fileLoaded(filePath);
    emit lineCountChanged(lineCount());

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

void LogViewer::updateLineCount()
{
    const int filtered = m_proxyModel->rowCount();
    const int total = m_model->entryCount();

    if (filtered == total) {
        m_lineCountLabel->setText(tr("Lines: %1").arg(total));
    } else {
        m_lineCountLabel->setText(tr("Lines: %1 / %2").arg(filtered).arg(total));
    }
}
