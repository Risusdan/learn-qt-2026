// Week 8 — Filtering, Delegates & Search

#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QWidget>

class QTableView;
class QComboBox;
class QLineEdit;
class QPushButton;
class QLabel;
class QDragEnterEvent;
class QDropEvent;
class LogModel;
class LogFilterProxy;
class LogDelegate;

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
    void updateLineCount();

private:
    QTableView      *m_tableView       = nullptr;
    LogModel        *m_model           = nullptr;
    LogFilterProxy  *m_proxyModel      = nullptr;
    LogDelegate     *m_delegate        = nullptr;
    QComboBox       *m_levelCombo      = nullptr;
    QLineEdit       *m_searchEdit      = nullptr;
    QPushButton     *m_autoScrollButton = nullptr;
    QLabel          *m_lineCountLabel  = nullptr;
    bool             m_autoScroll      = true;
};

#endif // LOGVIEWER_H
