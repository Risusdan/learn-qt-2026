// Week 13 — Serial Monitor Advanced

#include "SerialMonitorTab.h"
#include "HexViewer.h"
#include "ProcessLauncher.h"
#include "SerialWorker.h"

#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFont>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QSerialPortInfo>
#include <QStackedWidget>
#include <QThread>
#include <QTime>
#include <QVBoxLayout>

// ---------------------------------------------------------------------------
// Construction / Destruction
// ---------------------------------------------------------------------------

SerialMonitorTab::SerialMonitorTab(QWidget *parent)
    : QWidget(parent)
{
    // ---- Worker thread setup ----------------------------------------------
    m_workerThread = new QThread(this);
    m_worker = new SerialWorker;  // No parent — will be moved to thread
    m_worker->moveToThread(m_workerThread);

    // ---- Top toolbar row --------------------------------------------------
    auto *toolbarLayout = new QHBoxLayout;

    m_portCombo = new QComboBox(this);
    m_portCombo->setMinimumWidth(150);
    toolbarLayout->addWidget(m_portCombo);

    m_baudCombo = new QComboBox(this);
    m_baudCombo->addItem(QStringLiteral("9600"),   9600);
    m_baudCombo->addItem(QStringLiteral("19200"),  19200);
    m_baudCombo->addItem(QStringLiteral("38400"),  38400);
    m_baudCombo->addItem(QStringLiteral("57600"),  57600);
    m_baudCombo->addItem(QStringLiteral("115200"), 115200);
    m_baudCombo->setCurrentIndex(4);  // 115200 default
    toolbarLayout->addWidget(m_baudCombo);

    m_refreshButton = new QPushButton(tr("Refresh"), this);
    toolbarLayout->addWidget(m_refreshButton);

    m_connectButton = new QPushButton(tr("Connect"), this);
    toolbarLayout->addWidget(m_connectButton);

    m_statusLabel = new QLabel(tr("Disconnected"), this);
    toolbarLayout->addWidget(m_statusLabel);

    toolbarLayout->addStretch();

    // ---- Display controls row ---------------------------------------------
    auto *controlsLayout = new QHBoxLayout;

    m_displayModeButton = new QPushButton(tr("Hex"), this);
    m_displayModeButton->setCheckable(true);
    m_displayModeButton->setToolTip(tr("Toggle ASCII / Hex display"));
    controlsLayout->addWidget(m_displayModeButton);

    m_timestampButton = new QPushButton(tr("Timestamps"), this);
    m_timestampButton->setCheckable(true);
    m_timestampButton->setToolTip(tr("Prepend timestamps to received lines"));
    controlsLayout->addWidget(m_timestampButton);

    m_logToggleButton = new QPushButton(tr("Log to File"), this);
    m_logToggleButton->setCheckable(true);
    m_logToggleButton->setToolTip(tr("Log received data to a file"));
    controlsLayout->addWidget(m_logToggleButton);

    m_counterLabel = new QLabel(tr("RX: 0  TX: 0"), this);
    controlsLayout->addStretch();
    controlsLayout->addWidget(m_counterLabel);

    // ---- Middle display area ----------------------------------------------
    m_displayStack = new QStackedWidget(this);

    m_textDisplay = new QPlainTextEdit(this);
    m_textDisplay->setReadOnly(true);
    m_textDisplay->setFont(QFont(QStringLiteral("Courier"), 10));
    m_displayStack->addWidget(m_textDisplay);  // page 0 — ASCII

    m_hexViewer = new HexViewer(this);
    m_displayStack->addWidget(m_hexViewer);    // page 1 — Hex

    m_displayStack->setCurrentIndex(0);

    // ---- Bottom send bar --------------------------------------------------
    auto *sendLayout = new QHBoxLayout;

    m_sendEdit = new QLineEdit(this);
    m_sendEdit->setPlaceholderText(tr("Type message..."));
    sendLayout->addWidget(m_sendEdit);

    m_lineEndingCombo = new QComboBox(this);
    m_lineEndingCombo->addItem(QStringLiteral("None"));
    m_lineEndingCombo->addItem(QStringLiteral("LF"));
    m_lineEndingCombo->addItem(QStringLiteral("CR"));
    m_lineEndingCombo->addItem(QStringLiteral("CRLF"));
    m_lineEndingCombo->setCurrentIndex(1);  // LF default
    sendLayout->addWidget(m_lineEndingCombo);

    m_sendButton = new QPushButton(tr("Send"), this);
    sendLayout->addWidget(m_sendButton);

    // ---- Process launcher (collapsible) -----------------------------------
    m_processGroup = new QGroupBox(tr("Process Launcher"), this);
    m_processGroup->setCheckable(true);
    m_processGroup->setChecked(false);

    m_processLauncher = new ProcessLauncher(m_processGroup);
    auto *processLayout = new QVBoxLayout(m_processGroup);
    processLayout->addWidget(m_processLauncher);

    // ---- Overall layout ---------------------------------------------------
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(toolbarLayout);
    mainLayout->addLayout(controlsLayout);
    mainLayout->addWidget(m_displayStack, 1);  // stretch factor 1
    mainLayout->addLayout(sendLayout);
    mainLayout->addWidget(m_processGroup);

    // ---- UI signal-slot connections ---------------------------------------
    connect(m_refreshButton, &QPushButton::clicked,
            this, &SerialMonitorTab::refreshPorts);

    connect(m_connectButton, &QPushButton::clicked,
            this, &SerialMonitorTab::toggleConnection);

    connect(m_sendButton, &QPushButton::clicked,
            this, &SerialMonitorTab::sendData);

    connect(m_sendEdit, &QLineEdit::returnPressed,
            this, &SerialMonitorTab::sendData);

    connect(m_displayModeButton, &QPushButton::clicked,
            this, &SerialMonitorTab::toggleDisplayMode);

    connect(m_timestampButton, &QPushButton::clicked,
            this, &SerialMonitorTab::toggleTimestamps);

    connect(m_logToggleButton, &QPushButton::clicked,
            this, &SerialMonitorTab::toggleLogging);

    // ---- Cross-thread connections (queued automatically) ------------------
    // Requests from main thread → worker thread
    connect(this, &SerialMonitorTab::openRequested,
            m_worker, &SerialWorker::openPort);

    connect(this, &SerialMonitorTab::closeRequested,
            m_worker, &SerialWorker::closePort);

    connect(this, &SerialMonitorTab::sendRequested,
            m_worker, &SerialWorker::sendData);

    // Responses from worker thread → main thread
    connect(m_worker, &SerialWorker::dataReceived,
            this, &SerialMonitorTab::onDataReceived);

    connect(m_worker, &SerialWorker::errorOccurred,
            this, &SerialMonitorTab::onWorkerError);

    connect(m_worker, &SerialWorker::connectionChanged,
            this, &SerialMonitorTab::onConnectionChanged);

    // ---- Start worker thread ----------------------------------------------
    m_workerThread->start();

    // ---- Initial port scan ------------------------------------------------
    refreshPorts();
}

SerialMonitorTab::~SerialMonitorTab()
{
    // Close log file if active
    if (m_logFile) {
        m_logFile->close();
        delete m_logFile;
        m_logFile = nullptr;
    }

    // Signal the worker to close the port
    emit closeRequested();

    // Shut down the worker thread cleanly
    m_workerThread->quit();
    m_workerThread->wait();

    // Delete worker (no parent, so we own it)
    delete m_worker;
}

// ---------------------------------------------------------------------------
// Port Enumeration
// ---------------------------------------------------------------------------

void SerialMonitorTab::refreshPorts()
{
    m_portCombo->clear();

    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : ports) {
        m_portCombo->addItem(info.portName(), info.systemLocation());
    }
}

// ---------------------------------------------------------------------------
// Connect / Disconnect
// ---------------------------------------------------------------------------

void SerialMonitorTab::toggleConnection()
{
    if (m_connected) {
        emit closeRequested();
    } else {
        const QString portName = m_portCombo->currentText();
        const qint32  baudRate = m_baudCombo->currentData().toInt();
        emit openRequested(portName, baudRate);
    }
}

// ---------------------------------------------------------------------------
// Sending Data
// ---------------------------------------------------------------------------

void SerialMonitorTab::sendData()
{
    if (!m_connected) {
        return;
    }

    QByteArray data = m_sendEdit->text().toUtf8();

    // Append selected line ending
    const int endingIndex = m_lineEndingCombo->currentIndex();
    switch (endingIndex) {
    case 0:  /* None */ break;
    case 1:  data.append('\n');    break;  // LF
    case 2:  data.append('\r');    break;  // CR
    case 3:  data.append("\r\n");  break;  // CRLF
    default: break;
    }

    m_bytesSent += data.size();
    updateCounterLabel();

    emit sendRequested(data);
    m_sendEdit->clear();
}

// ---------------------------------------------------------------------------
// Worker Thread Callbacks (received on main thread via queued connections)
// ---------------------------------------------------------------------------

void SerialMonitorTab::onDataReceived(const QByteArray &data)
{
    m_bytesReceived += data.size();
    updateCounterLabel();

    // Log to file if active
    if (m_logFile && m_logFile->isOpen()) {
        m_logFile->write(data);
        m_logFile->write("\n");
    }

    // Feed data to both displays
    appendToDisplay(data);
    m_hexViewer->appendData(data + '\n');
}

void SerialMonitorTab::onWorkerError(const QString &errorString)
{
    m_statusLabel->setText(tr("Error: %1").arg(errorString));
}

void SerialMonitorTab::onConnectionChanged(bool connected, const QString &portName)
{
    m_connected = connected;

    if (connected) {
        m_connectButton->setText(tr("Disconnect"));
        m_statusLabel->setText(tr("Connected: %1").arg(portName));
        m_bytesReceived = 0;
        m_bytesSent = 0;
        updateCounterLabel();
    } else {
        m_connectButton->setText(tr("Connect"));
        m_statusLabel->setText(tr("Disconnected"));
    }

    emit connectionChanged(connected, portName);
}

// ---------------------------------------------------------------------------
// Display Mode Toggle (ASCII / Hex)
// ---------------------------------------------------------------------------

void SerialMonitorTab::toggleDisplayMode()
{
    const bool hexMode = m_displayModeButton->isChecked();
    m_displayStack->setCurrentIndex(hexMode ? 1 : 0);
    m_displayModeButton->setText(hexMode ? tr("ASCII") : tr("Hex"));
}

// ---------------------------------------------------------------------------
// Timestamps Toggle
// ---------------------------------------------------------------------------

void SerialMonitorTab::toggleTimestamps()
{
    m_timestampsEnabled = m_timestampButton->isChecked();
}

// ---------------------------------------------------------------------------
// Logging Toggle
// ---------------------------------------------------------------------------

void SerialMonitorTab::toggleLogging()
{
    if (m_logToggleButton->isChecked()) {
        // Start logging — ask user for file path
        const QString filePath = QFileDialog::getSaveFileName(
            this, tr("Log File"), QString(), tr("Log Files (*.log);;All Files (*)"));

        if (filePath.isEmpty()) {
            m_logToggleButton->setChecked(false);
            return;
        }

        m_logFile = new QFile(filePath, this);
        if (!m_logFile->open(QIODevice::WriteOnly | QIODevice::Append)) {
            m_statusLabel->setText(
                tr("Log error: %1").arg(m_logFile->errorString()));
            delete m_logFile;
            m_logFile = nullptr;
            m_logToggleButton->setChecked(false);
            return;
        }
    } else {
        // Stop logging
        if (m_logFile) {
            m_logFile->close();
            delete m_logFile;
            m_logFile = nullptr;
        }
    }
}

// ---------------------------------------------------------------------------
// Display Helper
// ---------------------------------------------------------------------------

void SerialMonitorTab::appendToDisplay(const QByteArray &data)
{
    QString line = QString::fromUtf8(data);

    // Prepend timestamp if enabled
    if (m_timestampsEnabled) {
        const QString timestamp =
            QTime::currentTime().toString(QStringLiteral("[HH:mm:ss.zzz] "));
        line.prepend(timestamp);
    }

    m_textDisplay->appendPlainText(line);

    // Auto-scroll to bottom
    QScrollBar *scrollBar = m_textDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

// ---------------------------------------------------------------------------
// Counter Label
// ---------------------------------------------------------------------------

void SerialMonitorTab::updateCounterLabel()
{
    m_counterLabel->setText(
        tr("RX: %1  TX: %2").arg(m_bytesReceived).arg(m_bytesSent));
}
