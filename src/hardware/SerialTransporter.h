#ifndef SERIALTRANSPORTER_H
#define SERIALTRANSPORTER_H

#include "ObdTransporter.h"
#include <QSerialPort>

/**
 * @brief The SerialTransporter class
 * Serial/PTY implementation for OBD-II communication.
 * Used with ELM327 emulator on WSL2 (PTY) or physical serial adapters.
 */
class SerialTransporter : public ObdTransporter
{
    Q_OBJECT

public:
    explicit SerialTransporter(QObject *parent = nullptr);
    ~SerialTransporter() override;

    void connectToDevice(const QString &identifier) override; // identifier = device path (e.g. /dev/pts/10)
    void disconnectFromDevice() override;
    void sendCommand(const QByteArray &cmd) override;
    bool isConnected() const override;

private slots:
    void onSerialReadyRead();
    void onSerialErrorOccurred(QSerialPort::SerialPortError error);

private:
    QSerialPort *m_serial;
};

#endif // SERIALTRANSPORTER_H
