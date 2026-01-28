#ifndef SCANSERVICE_H
#define SCANSERVICE_H

#include <QObject>
#include <QQueue>
#include <QByteArray>
#include <QTimer>
#include "core/dto/ScanResult.h"
#include "core/dto/DtcEntry.h"
#include "hardware/ObdTransporter.h"

class DtcParser;
class ReadinessParser;

/**
 * @brief The ScanService class
 * Manages the OBD-II scan pipeline and command sequencing.
 */
class ScanService : public QObject
{
    Q_OBJECT

public:
    enum ScanState {
        Idle,
        Connecting,
        Scanning,
        Error
    };

    explicit ScanService(ObdTransporter* transporter, QObject *parent = nullptr);
    ~ScanService();

    /**
     * @brief Start the connection sequence.
     */
    void startConnection();

    /**
     * @brief Start a diagnostic scan.
     * @pre Must be connected to ECU (ConnectedEcu state).
     */
    void startScan();

    /**
     * @brief Cancel current operation (connection or scan).
     */
    void cancel();

    /**
     * @brief Check if currently scanning.
     */
    bool isScanning() const { return m_state == Scanning; }

    /**
     * @brief Check if currently connecting.
     */
    bool isConnecting() const { return m_state == Connecting; }

signals:
    /**
     * @brief Emitted when connection sequence completes successfully.
     * @param protocolName The detected protocol name.
     */
    void connectionComplete(const QString& protocolName);

    /**
     * @brief Emitted when connection sequence fails.
     * @param errorMessage Error description.
     */
    void connectionFailed(const QString& errorMessage);

    /**
     * @brief Emitted when adapter is connected but ECU is not responding.
     */
    void adapterConnectedNoEcu();

    /**
     * @brief Emitted when scan completes successfully.
     * @param result The scan results.
     */
    void scanComplete(const ScanResult& result);

    /**
     * @brief Emitted when scan fails.
     * @param errorMessage Error description.
     */
    void scanFailed(const QString& errorMessage);

    /**
     * @brief Emitted during scan to indicate progress.
     * @param message Progress message.
     */
    void scanProgress(const QString& message);

private slots:
    void onDataReceived(const QByteArray& data);
    void onTimeout();

private:
    enum CommandType {
        CmdConnection,
        CmdScan
    };

    struct Command {
        QByteArray data;
        QString description;
        CommandType type;
    };

    void processNextCommand();
    void handleConnectionResponse(const QByteArray& response);
    void handleScanResponse(const QByteArray& response);
    void parseProtocolName(const QByteArray& response);
    void parseMilStatus(const QByteArray& response);
    void parseDtcResponse(const QByteArray& response, DtcStatus status);
    void parseReadinessResponse(const QByteArray& response);
    void finishScan();
    void reset();

    ObdTransporter* m_transporter;
    DtcParser* m_dtcParser;
    ReadinessParser* m_readinessParser;
    
    QQueue<Command> m_commandQueue;
    QByteArray m_responseBuffer;
    ScanState m_state;
    CommandType m_currentOperation;
    
    // Scan state
    ScanResult m_currentScanResult;
    bool m_milStatusReceived;
    bool m_storedDtcReceived;
    bool m_pendingDtcReceived;
    bool m_readinessReceived;
    
    // Connection state
    QString m_protocolName;
    bool m_ecuResponded;
    
    QTimer* m_timeoutTimer;
    static const int TIMEOUT_MS = 5000; // 5 second timeout
};

#endif // SCANSERVICE_H
