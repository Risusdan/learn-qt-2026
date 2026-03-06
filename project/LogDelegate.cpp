// Week 8 — Filtering, Delegates & Search

#include "LogDelegate.h"

#include <QPainter>
#include <QApplication>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

LogDelegate::LogDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

// ---------------------------------------------------------------------------
// Public
// ---------------------------------------------------------------------------

void LogDelegate::setSearchTerm(const QString &term)
{
    m_searchTerm = term;
}

// ---------------------------------------------------------------------------
// paint() Override
// ---------------------------------------------------------------------------

void LogDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const
{
    // Column 1 = Level — custom colored background
    if (index.column() == 1) {
        paintLevelCell(painter, option, index);
        return;
    }

    // Column 2 = Message — search term highlighting
    if (index.column() == 2 && !m_searchTerm.isEmpty()) {
        paintMessageCell(painter, option, index);
        return;
    }

    // All other columns — default rendering
    QStyledItemDelegate::paint(painter, option, index);
}

// ---------------------------------------------------------------------------
// Private — Level Cell
// ---------------------------------------------------------------------------

void LogDelegate::paintLevelCell(QPainter *painter,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const
{
    painter->save();

    const QString level = index.data(Qt::DisplayRole).toString();

    QColor bgColor;
    QColor textColor(Qt::black);

    if (level == QLatin1String("ERROR")) {
        bgColor  = QColor(220, 50, 50);     // red
        textColor = Qt::white;
    } else if (level == QLatin1String("WARN") || level == QLatin1String("WARNING")) {
        bgColor  = QColor(255, 165, 0);     // orange
        textColor = QColor(50, 50, 50);      // dark
    } else if (level == QLatin1String("DEBUG")) {
        bgColor  = QColor(220, 220, 220);   // light gray
    } else {
        // INFO or other — use default rendering
        painter->restore();
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    // Draw background
    painter->fillRect(option.rect, bgColor);

    // Handle selection highlight
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight().color().lighter(130));
    }

    // Draw text centered
    painter->setPen(textColor);
    const QRect textRect = option.rect.adjusted(4, 0, -4, 0);
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, level);

    painter->restore();
}

// ---------------------------------------------------------------------------
// Private — Message Cell (Search Highlighting)
// ---------------------------------------------------------------------------

void LogDelegate::paintMessageCell(QPainter *painter,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    painter->save();

    // Draw selection or default background
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    } else {
        painter->fillRect(option.rect, option.palette.base());
    }

    const QString text = index.data(Qt::DisplayRole).toString();
    const QRect textRect = option.rect.adjusted(4, 0, -4, 0);
    const QFontMetrics fm(option.font);

    // Split text and highlight matching portions
    int pos = 0;
    int drawX = textRect.x();
    const int y = textRect.y();
    const int h = textRect.height();

    while (pos < text.length()) {
        const int matchPos = text.indexOf(m_searchTerm, pos, Qt::CaseInsensitive);

        if (matchPos == -1) {
            // Draw remaining text normally
            const QString remaining = text.mid(pos);
            if (option.state & QStyle::State_Selected) {
                painter->setPen(option.palette.highlightedText().color());
            } else {
                painter->setPen(option.palette.text().color());
            }
            painter->drawText(QRect(drawX, y, textRect.right() - drawX, h),
                              Qt::AlignLeft | Qt::AlignVCenter, remaining);
            break;
        }

        // Draw text before match
        if (matchPos > pos) {
            const QString before = text.mid(pos, matchPos - pos);
            if (option.state & QStyle::State_Selected) {
                painter->setPen(option.palette.highlightedText().color());
            } else {
                painter->setPen(option.palette.text().color());
            }
            painter->drawText(QRect(drawX, y, textRect.right() - drawX, h),
                              Qt::AlignLeft | Qt::AlignVCenter, before);
            drawX += fm.horizontalAdvance(before);
        }

        // Draw highlighted match
        const QString match = text.mid(matchPos, m_searchTerm.length());
        const int matchWidth = fm.horizontalAdvance(match);
        painter->fillRect(QRect(drawX, y, matchWidth, h), QColor(255, 255, 0)); // yellow
        painter->setPen(Qt::black);
        painter->drawText(QRect(drawX, y, matchWidth, h),
                          Qt::AlignLeft | Qt::AlignVCenter, match);
        drawX += matchWidth;

        pos = matchPos + m_searchTerm.length();
    }

    painter->restore();
}
