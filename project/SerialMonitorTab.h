// Week 12 — Threading

#ifndef SERIALMONITORTAB_H
#define SERIALMONITORTAB_H

#include <QWidget>

class QComboBox;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QStackedWidget;
class QThread;
class SerialWorker;

class SerialMonitorTab : public QWidget
{
    Q_OBJECT

public:
    explicit SerialMonitorTab(QWidget *parent = nullptr);
    ~SerialMonitorTab() override;

signals:
    // Outgoing signals to the UI (MainWindow status bar)
    void connectionChanged(bool connected, const QString &portName);

    // Request signals to the worker thread (queued connections)
    void openRequested(const QString &portName, qint32 baudRate);
    void closeRequested();
    void sendRequested(const QByteArray &data);

private slots:
    void refreshPorts();
    void toggleConnection();
    void sendData();
    void onDataReceived(const QByteArray &data);
    void onWorkerError(const QString &errorString);
    void onConnectionChanged(bool connected, const QString &portName);

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

    // Threading
    QThread      *m_workerThread = nullptr;
    SerialWorker *m_worker       = nullptr;
    bool          m_connected    = false;
};

#endif // SERIALMONITORTAB_H
