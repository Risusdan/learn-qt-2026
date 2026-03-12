// Week 12 — Threading

#include "SerialWorker.h"

#include <QSerialPort>
#include <QSerialPortInfo>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

SerialWorker::SerialWorker(QObject *parent)
    : QObject(parent)
{
    // NOTE: Do NOT create QSerialPort here.
    // This constructor runs on the main thread. QSerialPort must be created
    // on the worker thread so its internal event processing happens there.
    // We create it in openPort(), which is called via a queued connection
    // and therefore executes on the worker thread.
}

// ---------------------------------------------------------------------------
// Port Management (runs on worker thread)
// ---------------------------------------------------------------------------

void SerialWorker::openPort(const QString &portName, qint32 baudRate)
{
    // Close any existing connection first
    closePort();

    // Create QSerialPort here — on the worker thread
    m_serialPort = new QSerialPort(this);

    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(baudRate);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    connect(m_serialPort, &QSerialPort::readyRead,
            this, &SerialWorker::onReadyRead);

    if (!m_serialPort->open(QIODevice::ReadWrite)) {
        emit errorOccurred(m_serialPort->errorString());
        delete m_serialPort;
        m_serialPort = nullptr;
        return;
    }

    emit connectionChanged(true, portName);
}

void SerialWorker::closePort()
{
    if (m_serialPort) {
        if (m_serialPort->isOpen()) {
            m_serialPort->close();
        }
        delete m_serialPort;
        m_serialPort = nullptr;
        m_readBuffer.clear();
        emit connectionChanged(false, QString());
    }
}

// ---------------------------------------------------------------------------
// Sending Data (runs on worker thread)
// ---------------------------------------------------------------------------

void SerialWorker::sendData(const QByteArray &data)
{
    if (m_serialPort && m_serialPort->isOpen()) {
        m_serialPort->write(data);
    }
}

// ---------------------------------------------------------------------------
// Receiving Data (runs on worker thread)
// ---------------------------------------------------------------------------

void SerialWorker::onReadyRead()
{
    m_readBuffer.append(m_serialPort->readAll());

    // Split by newline — process every complete line
    QList<QByteArray> parts = m_readBuffer.split('\n');

    // Last element is either empty or an incomplete line — keep it
    m_readBuffer = parts.takeLast();

    for (const QByteArray &line : std::as_const(parts)) {
        emit dataReceived(line);
    }
}
