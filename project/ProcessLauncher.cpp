// Week 13 — Serial Monitor Advanced

#include "ProcessLauncher.h"

#include <QFont>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QProcess>
#include <QPushButton>
#include <QVBoxLayout>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

ProcessLauncher::ProcessLauncher(QWidget *parent)
    : QWidget(parent)
{
    m_process = new QProcess(this);
    m_process->setProcessChannelMode(QProcess::MergedChannels);

    // ---- Command bar ------------------------------------------------------
    auto *commandLayout = new QHBoxLayout;

    m_commandEdit = new QLineEdit(this);
    m_commandEdit->setPlaceholderText(tr("Enter command..."));
    commandLayout->addWidget(m_commandEdit, 1);

    m_runButton = new QPushButton(tr("Run"), this);
    commandLayout->addWidget(m_runButton);

    m_stopButton = new QPushButton(tr("Stop"), this);
    m_stopButton->setEnabled(false);
    commandLayout->addWidget(m_stopButton);

    // ---- Output display ---------------------------------------------------
    m_outputView = new QPlainTextEdit(this);
    m_outputView->setReadOnly(true);
    m_outputView->setFont(QFont(QStringLiteral("Courier"), 10));

    // ---- Layout -----------------------------------------------------------
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addLayout(commandLayout);
    mainLayout->addWidget(m_outputView, 1);

    // ---- Connections ------------------------------------------------------
    connect(m_runButton, &QPushButton::clicked,
            this, &ProcessLauncher::launchProcess);

    connect(m_stopButton, &QPushButton::clicked,
            this, &ProcessLauncher::stopProcess);

    connect(m_commandEdit, &QLineEdit::returnPressed,
            this, &ProcessLauncher::launchProcess);

    connect(m_process, &QProcess::readyReadStandardOutput,
            this, &ProcessLauncher::onReadyReadOutput);

    connect(m_process, &QProcess::finished,
            this, &ProcessLauncher::onProcessFinished);

    connect(m_process, &QProcess::errorOccurred,
            this, &ProcessLauncher::onProcessError);
}

// ---------------------------------------------------------------------------
// Process Management
// ---------------------------------------------------------------------------

void ProcessLauncher::launchProcess()
{
    const QString command = m_commandEdit->text().trimmed();
    if (command.isEmpty()) {
        return;
    }

    if (m_process->state() != QProcess::NotRunning) {
        return;
    }

    m_outputView->clear();
    m_outputView->appendPlainText(tr("$ %1").arg(command));
    m_outputView->appendPlainText(QString());

    // Split command into program and arguments
    // QProcess::splitCommand handles quoting properly
    QStringList parts = QProcess::splitCommand(command);
    if (parts.isEmpty()) {
        return;
    }

    const QString program = parts.takeFirst();
    m_process->start(program, parts);
    setRunning(true);
}

void ProcessLauncher::stopProcess()
{
    if (m_process->state() == QProcess::NotRunning) {
        return;
    }

    m_process->terminate();

    // If the process doesn't stop within 3 seconds, force kill
    if (!m_process->waitForFinished(3000)) {
        m_process->kill();
    }
}

// ---------------------------------------------------------------------------
// Output Handling
// ---------------------------------------------------------------------------

void ProcessLauncher::onReadyReadOutput()
{
    const QByteArray output = m_process->readAllStandardOutput();
    m_outputView->appendPlainText(QString::fromUtf8(output).trimmed());
}

void ProcessLauncher::onProcessFinished(int exitCode,
                                         QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::CrashExit) {
        m_outputView->appendPlainText(tr("\n[Process crashed]"));
    } else {
        m_outputView->appendPlainText(
            tr("\n[Process finished with exit code %1]").arg(exitCode));
    }

    setRunning(false);
}

void ProcessLauncher::onProcessError(QProcess::ProcessError error)
{
    if (error == QProcess::FailedToStart) {
        m_outputView->appendPlainText(
            tr("\n[Error: Failed to start — %1]").arg(m_process->errorString()));
        setRunning(false);
    }
}

// ---------------------------------------------------------------------------
// UI State
// ---------------------------------------------------------------------------

void ProcessLauncher::setRunning(bool running)
{
    m_runButton->setEnabled(!running);
    m_stopButton->setEnabled(running);
    m_commandEdit->setEnabled(!running);
}
