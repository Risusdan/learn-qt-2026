// Week 13 — Serial Monitor Advanced

#ifndef HEXVIEWER_H
#define HEXVIEWER_H

#include <QAbstractScrollArea>
#include <QByteArray>

class HexViewer : public QAbstractScrollArea
{
    Q_OBJECT

public:
    explicit HexViewer(QWidget *parent = nullptr);

    void setData(const QByteArray &data);
    void appendData(const QByteArray &data);
    void clear();

    int bytesPerLine() const { return BytesPerLine; }

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateScrollBar();
    int  lineCount() const;
    int  visibleLineCount() const;

    QByteArray m_data;

    // Layout metrics (recalculated in resizeEvent)
    int m_charWidth   = 0;
    int m_lineHeight  = 0;
    int m_offsetWidth = 0;   // width of "00000000  " offset gutter
    int m_hexWidth    = 0;   // width of hex byte columns
    int m_asciiX      = 0;   // x position of ASCII column

    static constexpr int BytesPerLine = 16;
};

#endif // HEXVIEWER_H
