// Week 8 — Filtering, Delegates & Search

#ifndef LOGFILTERPROXY_H
#define LOGFILTERPROXY_H

#include <QSortFilterProxyModel>

class LogFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit LogFilterProxy(QObject *parent = nullptr);

public slots:
    void setLevelFilter(const QString &level);
    void setTextFilter(const QString &text);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QString m_levelFilter;
    QString m_textFilter;
};

#endif // LOGFILTERPROXY_H
