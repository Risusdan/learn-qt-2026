// Week 13 — Serial Monitor Advanced

#ifndef SERIALMONITORTAB_H
#define SERIALMONITORTAB_H

#include <QWidget>

class HexViewer;
class ProcessLauncher;
class QComboBox;
class QFile;
class QGroupBox;
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
    void toggleDisplayMode();
    void toggleTimestamps();
    void toggleLogging();

private:
    void appendToDisplay(const QByteArray &data);
    void updateCounterLabel();

    // Connection bar
    QComboBox   *m_portCombo     = nullptr;
    QComboBox   *m_baudCombo     = nullptr;
    QPushButton *m_connectButton = nullptr;
    QPushButton *m_refreshButton = nullptr;
    QLabel      *m_statusLabel   = nullptr;

    // Display controls
    QPushButton    *m_displayModeButton = nullptr;
    QPushButton    *m_timestampButton   = nullptr;
    QPushButton    *m_logToggleButton   = nullptr;
    QLabel         *m_counterLabel      = nullptr;

    // Display
    QStackedWidget *m_displayStack = nullptr;
    QPlainTextEdit *m_textDisplay  = nullptr;
    HexViewer      *m_hexViewer    = nullptr;

    // Send bar
    QLineEdit   *m_sendEdit        = nullptr;
    QComboBox   *m_lineEndingCombo = nullptr;
    QPushButton *m_sendButton      = nullptr;

    // Process launcher
    QGroupBox       *m_processGroup    = nullptr;
    ProcessLauncher *m_processLauncher = nullptr;

    // Threading
    QThread      *m_workerThread = nullptr;
    SerialWorker *m_worker       = nullptr;
    bool          m_connected    = false;

    // Features
    bool    m_timestampsEnabled = false;
    QFile  *m_logFile           = nullptr;
    qint64  m_bytesReceived     = 0;
    qint64  m_bytesSent         = 0;
};

#endif // SERIALMONITORTAB_H
