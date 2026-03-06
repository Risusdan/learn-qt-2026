// Week 8 — Filtering, Delegates & Search

#include "LogFilterProxy.h"

#include <QAbstractItemModel>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

LogFilterProxy::LogFilterProxy(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

// ---------------------------------------------------------------------------
// Public Slots
// ---------------------------------------------------------------------------

void LogFilterProxy::setLevelFilter(const QString &level)
{
    m_levelFilter = level;
    invalidateFilter();
}

void LogFilterProxy::setTextFilter(const QString &text)
{
    m_textFilter = text;
    invalidateFilter();
}

// ---------------------------------------------------------------------------
// Filter Logic
// ---------------------------------------------------------------------------

bool LogFilterProxy::filterAcceptsRow(int sourceRow,
                                       const QModelIndex &sourceParent) const
{
    const auto *model = sourceModel();
    if (!model) {
        return true;
    }

    // --- Level filter (column 1) ---
    if (!m_levelFilter.isEmpty() && m_levelFilter != QLatin1String("All")) {
        const QModelIndex levelIndex = model->index(sourceRow, 1, sourceParent);
        const QString level = model->data(levelIndex).toString();
        if (level.compare(m_levelFilter, Qt::CaseInsensitive) != 0) {
            return false;
        }
    }

    // --- Text filter (search across all columns) ---
    if (!m_textFilter.isEmpty()) {
        bool found = false;
        for (int col = 0; col < model->columnCount(sourceParent); ++col) {
            const QModelIndex idx = model->index(sourceRow, col, sourceParent);
            if (model->data(idx).toString().contains(m_textFilter, Qt::CaseInsensitive)) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }

    return true;
}
