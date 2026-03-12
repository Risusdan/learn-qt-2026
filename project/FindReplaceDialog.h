// Week 10 — Text Editor Features

#ifndef FINDREPLACEDIALOG_H
#define FINDREPLACEDIALOG_H

#include <QDialog>

class QCheckBox;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;

class FindReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindReplaceDialog(QWidget *parent = nullptr);

    void setTextEdit(QPlainTextEdit *editor);

private slots:
    void onFindNext();
    void onFindPrevious();
    void onReplace();
    void onReplaceAll();

private:
    bool findText(bool backward);

    QLineEdit      *m_findEdit       = nullptr;
    QLineEdit      *m_replaceEdit    = nullptr;
    QCheckBox      *m_caseSensitive  = nullptr;
    QCheckBox      *m_wholeWords     = nullptr;
    QCheckBox      *m_useRegex       = nullptr;
    QLabel         *m_statusLabel    = nullptr;
    QPlainTextEdit *m_editor         = nullptr;
};

#endif // FINDREPLACEDIALOG_H
