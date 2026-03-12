// Week 8 — Filtering, Delegates & Search

#ifndef LOGDELEGATE_H
#define LOGDELEGATE_H

#include <QStyledItemDelegate>

class LogDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit LogDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    void setSearchTerm(const QString &term);

private:
    void paintLevelCell(QPainter *painter, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const;
    void paintMessageCell(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    QString m_searchTerm;
};

#endif // LOGDELEGATE_H
