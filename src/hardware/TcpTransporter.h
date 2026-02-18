#ifndef TCPTRANSPORTER_H
#define TCPTRANSPORTER_H

#include "ObdTransporter.h"
#include <QTcpSocket>

class TcpTransporter : public ObdTransporter
{
    Q_OBJECT

public:
    explicit TcpTransporter(QObject *parent = nullptr);
    ~TcpTransporter() override;

    // implement the interface
    void connectToDevice(const QString &identifier) override; // identifier = "IP:PORT"
    void disconnectFromDevice() override;
    void sendCommand(const QByteArray &cmd) override;
    bool isConnected() const override;

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketReadyRead();
    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *m_socket;
};

#endif // TCPTRANSPORTER_H
