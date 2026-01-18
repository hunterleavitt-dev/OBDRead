#ifndef BLETRANSPORTER_H
#define BLETRANSPORTER_H

#include "ObdTransporter.h"

/**
 * @brief The BleTransporter class
 * Bluetooth Low Energy (BLE) implementation for OBD-II communication.
 * Used with adapters like Veepeak OBDCheck BLE+.
 *
 * NOTE: This is a stub implementation. Full BLE support requires
 * Qt Bluetooth module and platform-specific setup.
 */
class BleTransporter : public ObdTransporter
{
    Q_OBJECT

public:
    explicit BleTransporter(QObject *parent = nullptr);
    ~BleTransporter() override;

    // ObdTransporter interface
    void connectToDevice(const QString &identifier) override;
    void disconnectFromDevice() override;
    void sendCommand(const QByteArray &cmd) override;
    bool isConnected() const override;

private:
    bool m_connected;
};

#endif // BLETRANSPORTER_H
