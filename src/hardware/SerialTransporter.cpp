#include "SerialTransporter.h"
#include <QDebug>

SerialTransporter::SerialTransporter(QObject *parent)
    : ObdTransporter(parent)
    , m_serial(new QSerialPort(this))
{
    connect(m_serial, &QSerialPort::readyRead, this, &SerialTransporter::onSerialReadyRead);
    connect(m_serial, &QSerialPort::errorOccurred, this, &SerialTransporter::onSerialErrorOccurred);
}

SerialTransporter::~SerialTransporter()
{
    disconnectFromDevice();
}

void SerialTransporter::connectToDevice(const QString &identifier)
{
    QString path = identifier.trimmed();
    if (path.isEmpty()) {
        emit errorOccurred("No device path specified.");
        return;
    }

    if (m_serial->isOpen()) {
        m_serial->close();
    }

    qDebug() << "Attempting to connect to serial device:" << path;

    m_serial->setPortName(path);
    m_serial->setBaudRate(QSerialPort::Baud38400);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setStopBits(QSerialPort::OneStop);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);

    if (m_serial->open(QIODevice::ReadWrite)) {
        qDebug() << "Serial connected!";
        emit connected();
    } else {
        QString err = m_serial->errorString();
        qDebug() << "Serial open failed:" << err;
        emit errorOccurred(err);
    }
}

void SerialTransporter::disconnectFromDevice()
{
    if (m_serial->isOpen()) {
        m_serial->close();
        qDebug() << "Serial disconnected!";
        emit disconnected();
    }
}

void SerialTransporter::sendCommand(const QByteArray &cmd)
{
    if (!m_serial->isOpen()) {
        emit errorOccurred("Cannot send: Not connected.");
        return;
    }

    m_serial->write(cmd);
    m_serial->flush();
}

bool SerialTransporter::isConnected() const
{
    return m_serial->isOpen();
}

void SerialTransporter::onSerialReadyRead()
{
    QByteArray data = m_serial->readAll();
    emit dataReceived(data);
}

void SerialTransporter::onSerialErrorOccurred(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError) {
        return;
    }

    QString msg = m_serial->errorString();
    if (m_serial->isOpen()) {
        m_serial->close();
        emit disconnected();
    }
    emit errorOccurred(msg);
}
