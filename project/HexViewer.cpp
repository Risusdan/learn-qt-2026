// Week 13 — Serial Monitor Advanced

#include "HexViewer.h"

#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QScrollBar>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

HexViewer::HexViewer(QWidget *parent)
    : QAbstractScrollArea(parent)
{
    // Use a monospace font for proper column alignment
    QFont mono(QStringLiteral("Courier"), 10);
    mono.setFixedPitch(true);
    viewport()->setFont(mono);

    // Calculate initial metrics
    QFontMetrics fm(mono);
    m_charWidth  = fm.horizontalAdvance(QLatin1Char('0'));
    m_lineHeight = fm.height();

    // Offset gutter: "00000000  " = 8 hex digits + 2 spaces = 10 chars
    m_offsetWidth = m_charWidth * 10;

    // Hex area: 16 bytes × 3 chars each ("XX ") + 1 extra space at byte 8 = 49 chars
    m_hexWidth = m_charWidth * 49;

    // ASCII column starts after offset + hex + a small gap
    m_asciiX = m_offsetWidth + m_hexWidth + m_charWidth;

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void HexViewer::setData(const QByteArray &data)
{
    m_data = data;
    updateScrollBar();
    viewport()->update();
}

void HexViewer::appendData(const QByteArray &data)
{
    m_data.append(data);
    updateScrollBar();

    // Auto-scroll to bottom
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
    viewport()->update();
}

void HexViewer::clear()
{
    m_data.clear();
    updateScrollBar();
    viewport()->update();
}

// ---------------------------------------------------------------------------
// Painting
// ---------------------------------------------------------------------------

void HexViewer::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(viewport());
    painter.setFont(viewport()->font());

    const int firstLine = verticalScrollBar()->value();
    const int lastLine  = qMin(firstLine + visibleLineCount(), lineCount());

    const QColor offsetColor(0x66, 0x66, 0x66);   // gray
    const QColor hexColor(0x00, 0x00, 0x00);       // black
    const QColor asciiColor(0x00, 0x66, 0x99);     // teal
    const QColor nonPrintColor(0xAA, 0xAA, 0xAA);  // light gray

    for (int line = firstLine; line < lastLine; ++line) {
        const int y = (line - firstLine) * m_lineHeight + m_lineHeight;
        const int byteOffset = line * BytesPerLine;
        const int bytesInLine = qMin(BytesPerLine,
                                     static_cast<int>(m_data.size()) - byteOffset);

        // ---- Offset gutter ------------------------------------------------
        painter.setPen(offsetColor);
        const QString offsetStr =
            QStringLiteral("%1").arg(byteOffset, 8, 16, QLatin1Char('0')).toUpper();
        painter.drawText(0, y, offsetStr);

        // ---- Hex bytes ----------------------------------------------------
        painter.setPen(hexColor);
        for (int i = 0; i < bytesInLine; ++i) {
            const unsigned char byte =
                static_cast<unsigned char>(m_data.at(byteOffset + i));
            const QString hexStr =
                QStringLiteral("%1").arg(byte, 2, 16, QLatin1Char('0')).toUpper();

            // Extra space between bytes 7 and 8 for visual grouping
            int xOffset = i * 3;
            if (i >= 8) {
                xOffset += 1;  // extra char gap after byte 7
            }
            const int x = m_offsetWidth + xOffset * m_charWidth;
            painter.drawText(x, y, hexStr);
        }

        // ---- ASCII column -------------------------------------------------
        for (int i = 0; i < bytesInLine; ++i) {
            const unsigned char byte =
                static_cast<unsigned char>(m_data.at(byteOffset + i));
            const bool printable = (byte >= 0x20 && byte <= 0x7E);

            painter.setPen(printable ? asciiColor : nonPrintColor);
            const QChar ch = printable ? QChar(byte) : QLatin1Char('.');
            painter.drawText(m_asciiX + i * m_charWidth, y, ch);
        }
    }
}

// ---------------------------------------------------------------------------
// Layout
// ---------------------------------------------------------------------------

void HexViewer::resizeEvent(QResizeEvent *event)
{
    QAbstractScrollArea::resizeEvent(event);

    // Recalculate metrics in case font changed
    QFontMetrics fm(viewport()->font());
    m_charWidth  = fm.horizontalAdvance(QLatin1Char('0'));
    m_lineHeight = fm.height();
    m_offsetWidth = m_charWidth * 10;
    m_hexWidth    = m_charWidth * 49;
    m_asciiX      = m_offsetWidth + m_hexWidth + m_charWidth;

    updateScrollBar();
}

void HexViewer::updateScrollBar()
{
    const int total   = lineCount();
    const int visible = visibleLineCount();
    verticalScrollBar()->setRange(0, qMax(0, total - visible));
    verticalScrollBar()->setPageStep(visible);
}

int HexViewer::lineCount() const
{
    if (m_data.isEmpty()) {
        return 0;
    }
    return (m_data.size() + BytesPerLine - 1) / BytesPerLine;
}

int HexViewer::visibleLineCount() const
{
    if (m_lineHeight <= 0) {
        return 1;
    }
    return viewport()->height() / m_lineHeight;
}
