#ifndef OBDTRANSPORTER_H
#define OBDTRANSPORTER_H

#include <QObject>
#include <QByteArray>
#include <QString>

/**
 * @brief The ObdTransporter class
 * Abstract Interface (HAL) for OBDII communication.
 * Implementations: TcpTransporter (Emulator), BleTransporter (Veepeak).
 */
class ObdTransporter : public QObject
{
    Q_OBJECT

public:
    explicit ObdTransporter(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~ObdTransporter() {}

    // --- The Contract (Pure Virtual Functions) ---

    /**
     * @brief Connects to the device.
     * @param identifier IP Address (for Emulator) or MAC Address (for BLE).
     */
    virtual void connectToDevice(const QString &identifier) = 0;

    /**
     * @brief Disconnects and cleans up resources.
     */
    virtual void disconnectFromDevice() = 0;

    /**
     * @brief Sends a raw AT command or Hex mode request.
     * @param cmd The command string (e.g., "AT Z\r" or "03\r").
     */
    virtual void sendCommand(const QByteArray &cmd) = 0;

    /**
     * @brief Check connection status.
     */
    virtual bool isConnected() const = 0;

signals:
    // --- Signals for the UI to subscribe to ---

    void connected();
    void disconnected();
    void errorOccurred(const QString &errorMsg);

    /**
     * @brief Emitted when raw data arrives from the adapter.
     */
    void dataReceived(const QByteArray &data);
};

#endif // OBDTRANSPORTER_H
