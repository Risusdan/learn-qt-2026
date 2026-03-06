// Week 11 — Serial Monitor Connection

#include "SerialMonitorTab.h"

#include <QComboBox>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStackedWidget>
#include <QVBoxLayout>

// ---------------------------------------------------------------------------
// Construction / Destruction
// ---------------------------------------------------------------------------

SerialMonitorTab::SerialMonitorTab(QWidget *parent)
    : QWidget(parent)
{
    // ---- Serial port (no parent yet — we assign it below) -----------------
    m_serialPort = new QSerialPort(this);

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

    // ---- Middle display area ----------------------------------------------
    m_displayStack = new QStackedWidget(this);

    m_textDisplay = new QPlainTextEdit(this);
    m_textDisplay->setReadOnly(true);
    m_textDisplay->setFont(QFont(QStringLiteral("Courier"), 10));
    m_displayStack->addWidget(m_textDisplay);  // page 0

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

    // ---- Overall layout ---------------------------------------------------
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(toolbarLayout);
    mainLayout->addWidget(m_displayStack, 1);  // stretch factor 1
    mainLayout->addLayout(sendLayout);

    // ---- Signal-slot connections ------------------------------------------
    connect(m_refreshButton, &QPushButton::clicked,
            this, &SerialMonitorTab::refreshPorts);

    connect(m_connectButton, &QPushButton::clicked,
            this, &SerialMonitorTab::toggleConnection);

    connect(m_serialPort, &QSerialPort::readyRead,
            this, &SerialMonitorTab::onReadyRead);

    connect(m_sendButton, &QPushButton::clicked,
            this, &SerialMonitorTab::sendData);

    connect(m_sendEdit, &QLineEdit::returnPressed,
            this, &SerialMonitorTab::sendData);

    // ---- Initial port scan ------------------------------------------------
    refreshPorts();
}

SerialMonitorTab::~SerialMonitorTab()
{
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
    }
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
    if (m_serialPort->isOpen()) {
        // ---- Disconnect ---------------------------------------------------
        m_serialPort->close();
        m_connectButton->setText(tr("Connect"));
        m_statusLabel->setText(tr("Disconnected"));
        emit connectionChanged(false, QString());
        return;
    }

    // ---- Connect ----------------------------------------------------------
    const QString portName = m_portCombo->currentText();
    const qint32  baudRate = m_baudCombo->currentData().toInt();

    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(baudRate);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if (!m_serialPort->open(QIODevice::ReadWrite)) {
        m_statusLabel->setText(
            tr("Error: %1").arg(m_serialPort->errorString()));
        return;
    }

    m_connectButton->setText(tr("Disconnect"));
    m_statusLabel->setText(tr("Connected: %1 @ %2").arg(portName).arg(baudRate));
    emit connectionChanged(true, portName);
}

// ---------------------------------------------------------------------------
// Receiving Data
// ---------------------------------------------------------------------------

void SerialMonitorTab::onReadyRead()
{
    m_readBuffer.append(m_serialPort->readAll());

    // Split by newline — process every complete line
    QList<QByteArray> parts = m_readBuffer.split('\n');

    // The last element is either empty (if buffer ended with '\n')
    // or an incomplete line — keep it in the buffer for next time.
    m_readBuffer = parts.takeLast();

    for (const QByteArray &line : std::as_const(parts)) {
        appendToDisplay(line);
    }
}

// ---------------------------------------------------------------------------
// Sending Data
// ---------------------------------------------------------------------------

void SerialMonitorTab::sendData()
{
    if (!m_serialPort->isOpen()) {
        return;
    }

    QByteArray data = m_sendEdit->text().toUtf8();

    // Append selected line ending
    const int endingIndex = m_lineEndingCombo->currentIndex();
    switch (endingIndex) {
    case 0:  /* None */ break;
    case 1:  data.append('\n');           break;  // LF
    case 2:  data.append('\r');           break;  // CR
    case 3:  data.append("\r\n");         break;  // CRLF
    default: break;
    }

    m_serialPort->write(data);
    m_sendEdit->clear();
}

// ---------------------------------------------------------------------------
// Display Helper
// ---------------------------------------------------------------------------

void SerialMonitorTab::appendToDisplay(const QByteArray &data)
{
    m_textDisplay->appendPlainText(QString::fromUtf8(data));

    // Auto-scroll to bottom
    QScrollBar *scrollBar = m_textDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}
