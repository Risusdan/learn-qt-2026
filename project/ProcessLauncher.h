// Week 13 — Serial Monitor Advanced

#ifndef PROCESSLAUNCHER_H
#define PROCESSLAUNCHER_H

#include <QWidget>

class QLineEdit;
class QPlainTextEdit;
class QProcess;
class QPushButton;

class ProcessLauncher : public QWidget
{
    Q_OBJECT

public:
    explicit ProcessLauncher(QWidget *parent = nullptr);

private slots:
    void launchProcess();
    void stopProcess();
    void onReadyReadOutput();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);

private:
    void setRunning(bool running);

    QLineEdit      *m_commandEdit = nullptr;
    QPushButton    *m_runButton   = nullptr;
    QPushButton    *m_stopButton  = nullptr;
    QPlainTextEdit *m_outputView  = nullptr;
    QProcess       *m_process     = nullptr;
};

#endif // PROCESSLAUNCHER_H
