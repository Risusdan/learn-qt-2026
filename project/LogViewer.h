// Week 6 — Log Viewer Basic

#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QWidget>

class QPlainTextEdit;
class QPushButton;
class QLabel;
class QDragEnterEvent;
class QDropEvent;

class LogViewer : public QWidget
{
    Q_OBJECT

public:
    explicit LogViewer(QWidget *parent = nullptr);

    void loadFile(const QString &filePath);
    int lineCount() const;

signals:
    void fileLoaded(const QString &filePath);
    void lineCountChanged(int count);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void toggleAutoScroll();
    void onScrollChanged(int value);

private:
    QPlainTextEdit *m_textDisplay      = nullptr;
    QPushButton    *m_autoScrollButton = nullptr;
    QLabel         *m_lineCountLabel   = nullptr;
    bool            m_autoScroll       = true;
};

#endif // LOGVIEWER_H
