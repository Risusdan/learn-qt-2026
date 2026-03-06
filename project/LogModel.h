// Week 7 — Model/View Programming

#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QAbstractTableModel>
#include <QColor>
#include <QString>
#include <QVector>

struct LogEntry
{
    QString timestamp;
    QString level;
    QString message;
};

class LogModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column { Timestamp = 0, Level, Message, ColumnCount };

    explicit LogModel(QObject *parent = nullptr);

    // --- QAbstractTableModel overrides ---
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // --- Public API ---
    void addEntry(const LogEntry &entry);
    void clear();
    bool loadFromFile(const QString &filePath);

    int entryCount() const { return m_entries.size(); }

private:
    LogEntry parseLine(const QString &line) const;
    QColor colorForLevel(const QString &level) const;

    QVector<LogEntry> m_entries;
};

#endif // LOGMODEL_H
