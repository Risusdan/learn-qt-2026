// Week 12 — Threading

#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <QObject>

class QSerialPort;

class SerialWorker : public QObject
{
    Q_OBJECT

public:
    explicit SerialWorker(QObject *parent = nullptr);

public slots:
    void openPort(const QString &portName, qint32 baudRate);
    void closePort();
    void sendData(const QByteArray &data);

signals:
    void dataReceived(const QByteArray &data);
    void errorOccurred(const QString &errorString);
    void connectionChanged(bool connected, const QString &portName);

private slots:
    void onReadyRead();

private:
    QSerialPort *m_serialPort = nullptr;
    QByteArray   m_readBuffer;
};

#endif // SERIALWORKER_H
