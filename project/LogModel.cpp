// Week 7 — Model/View Programming

#include "LogModel.h"

#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

LogModel::LogModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

// ---------------------------------------------------------------------------
// QAbstractTableModel Overrides
// ---------------------------------------------------------------------------

int LogModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;   // flat table — no children
    }
    return m_entries.size();
}

int LogModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return Column::ColumnCount;
}

QVariant LogModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_entries.size()) {
        return {};
    }

    const auto &entry = m_entries.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case Column::Timestamp: return entry.timestamp;
        case Column::Level:     return entry.level;
        case Column::Message:   return entry.message;
        }
    }

    if (role == Qt::ForegroundRole) {
        return colorForLevel(entry.level);
    }

    if (role == Qt::ToolTipRole) {
        // Show the full reconstructed log line as tooltip
        return QStringLiteral("[%1] [%2] %3")
            .arg(entry.timestamp, entry.level, entry.message);
    }

    return {};
}

QVariant LogModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return {};
    }

    switch (section) {
    case Column::Timestamp: return tr("Timestamp");
    case Column::Level:     return tr("Level");
    case Column::Message:   return tr("Message");
    }

    return {};
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void LogModel::addEntry(const LogEntry &entry)
{
    const int row = m_entries.size();
    beginInsertRows(QModelIndex(), row, row);
    m_entries.append(entry);
    endInsertRows();
}

void LogModel::clear()
{
    if (m_entries.isEmpty()) {
        return;
    }
    beginResetModel();
    m_entries.clear();
    endResetModel();
}

bool LogModel::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    beginResetModel();
    m_entries.clear();

    QTextStream in(&file);
    while (!in.atEnd()) {
        m_entries.append(parseLine(in.readLine()));
    }

    endResetModel();
    return true;
}

// ---------------------------------------------------------------------------
// Private Helpers
// ---------------------------------------------------------------------------

LogEntry LogModel::parseLine(const QString &line) const
{
    // Pattern: [2024-01-15 10:30:00] [ERROR] Something failed
    static const QRegularExpression re(R"(^\[(.+?)\]\s+\[(\w+)\]\s+(.*)$)");

    const auto match = re.match(line);
    if (match.hasMatch()) {
        return { match.captured(1), match.captured(2).toUpper(), match.captured(3) };
    }

    // Fallback: treat the whole line as an INFO message
    return { {}, QStringLiteral("INFO"), line };
}

QColor LogModel::colorForLevel(const QString &level) const
{
    if (level == QLatin1String("ERROR"))   return QColor(Qt::red);
    if (level == QLatin1String("WARN"))    return QColor(255, 165, 0);  // orange
    if (level == QLatin1String("WARNING")) return QColor(255, 165, 0);
    if (level == QLatin1String("DEBUG"))   return QColor(Qt::gray);
    return {};   // default text color for INFO and others
}
