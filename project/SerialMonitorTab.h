// Week 11 — Serial Monitor Connection

#ifndef SERIALMONITORTAB_H
#define SERIALMONITORTAB_H

#include <QWidget>

class QComboBox;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QSerialPort;
class QStackedWidget;

class SerialMonitorTab : public QWidget
{
    Q_OBJECT

public:
    explicit SerialMonitorTab(QWidget *parent = nullptr);
    ~SerialMonitorTab() override;

signals:
    void connectionChanged(bool connected, const QString &portName);

private slots:
    void refreshPorts();
    void toggleConnection();
    void onReadyRead();
    void sendData();

private:
    void appendToDisplay(const QByteArray &data);

    // Connection bar
    QComboBox   *m_portCombo     = nullptr;
    QComboBox   *m_baudCombo     = nullptr;
    QPushButton *m_connectButton = nullptr;
    QPushButton *m_refreshButton = nullptr;
    QLabel      *m_statusLabel   = nullptr;

    // Display
    QStackedWidget *m_displayStack = nullptr;
    QPlainTextEdit *m_textDisplay  = nullptr;

    // Send bar
    QLineEdit   *m_sendEdit        = nullptr;
    QComboBox   *m_lineEndingCombo = nullptr;
    QPushButton *m_sendButton      = nullptr;

    // Serial
    QSerialPort *m_serialPort = nullptr;
    QByteArray   m_readBuffer;
};

#endif // SERIALMONITORTAB_H
