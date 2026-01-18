#include "BleTransporter.h"
#include <QDebug>

BleTransporter::BleTransporter(QObject *parent)
    : ObdTransporter(parent)
    , m_connected(false)
{
}

BleTransporter::~BleTransporter()
{
    disconnectFromDevice();
}

void BleTransporter::connectToDevice(const QString &identifier)
{
    // TODO: implement BLE connection using Qt Bluetooth
    // (requires adding Qt6::Bluetooth to CMakeLists.txt)
    qWarning() << "BleTransporter::connectToDevice() not yet implemented for:" << identifier;
    emit errorOccurred("BLE transport not yet implemented");
}

void BleTransporter::disconnectFromDevice()
{
    if (m_connected) {
        m_connected = false;
        emit disconnected();
    }
}

void BleTransporter::sendCommand(const QByteArray &cmd)
{
    if (!m_connected) {
        emit errorOccurred("Cannot send: Not connected.");
        return;
    }

    // TODO: implement BLE write characteristic
    Q_UNUSED(cmd)
    qWarning() << "BleTransporter::sendCommand() not yet implemented";
}

bool BleTransporter::isConnected() const
{
    return m_connected;
}

