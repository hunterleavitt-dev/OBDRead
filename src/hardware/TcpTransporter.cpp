#include "TcpTransporter.h"
#include <QStringList>
#include <QDebug>

TcpTransporter::TcpTransporter(QObject *parent)
    : ObdTransporter(parent)
    , m_socket(new QTcpSocket(this))
{
    // wire up the socket signals to internal slots
    connect(m_socket, &QTcpSocket::connected, this, &TcpTransporter::onSocketConnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpTransporter::onSocketReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &TcpTransporter::onSocketError);
}

TcpTransporter::~TcpTransporter() {
    disconnectFromDevice();
}

void TcpTransporter::connectToDevice(const QString &identifier)
{
    // format expected: "IP:PORT" (e.g., "127.0.0.1:35000")
    QStringList parts = identifier.split(':');
    QString ip = parts.first();
    quint16 port = (parts.size() > 1) ? parts[1].toUShort() : 35000; // default to 35000 if no port

    qDebug() << "Attempting to connect to Emulator at" << ip << "on port" << port;
    m_socket->connectToHost(ip, port);
}

void TcpTransporter::disconnectFromDevice()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->disconnectFromHost();
    }
}

void TcpTransporter::sendCommand(const QByteArray &cmd)
{
    if (m_socket->state() != QAbstractSocket::ConnectedState) {
        emit errorOccurred("Cannot send: Not connected.");
        return;
    }

    m_socket->write(cmd);
    m_socket->flush(); // ensure data is sent immediately
}

bool TcpTransporter::isConnected() const
{
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

// --- Internal Slots ---

void TcpTransporter::onSocketConnected()
{
    qDebug() << "TCP Connected!";
    emit connected();
}

void TcpTransporter::onSocketReadyRead()
{
    QByteArray data = m_socket->readAll();
    // forward the raw data to the main app/parser
    emit dataReceived(data);
}

void TcpTransporter::onSocketError(QAbstractSocket::SocketError)
{
    emit errorOccurred(m_socket->errorString());
}
