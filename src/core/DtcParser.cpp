#include "DtcParser.h"

DtcParser::DtcParser(QObject *parent)
    : QObject(parent)
{
}

DtcParser::~DtcParser()
{
}

QStringList DtcParser::parseDtcResponse(const QByteArray &rawData)
{
    QStringList dtcList;
    // TODO: implement DTC parsing logic
    // expected format: "43 XX YY XX YY ..." where 43 is mode response
    // each DTC is 2 bytes (XX YY)
    Q_UNUSED(rawData)
    return dtcList;
}

QString DtcParser::decodeDtc(quint8 byte1, quint8 byte2)
{
    // first two bits of byte1 determine the DTC type:
    // 00 = P (Powertrain), 01 = C (Chassis), 10 = B (Body), 11 = U (Network)
    static const char dtcTypes[] = {'P', 'C', 'B', 'U'};

    quint8 typeIndex = (byte1 >> 6) & 0x03;
    quint8 digit1 = (byte1 >> 4) & 0x03;
    quint8 digit2 = byte1 & 0x0F;
    quint8 digit3 = (byte2 >> 4) & 0x0F;
    quint8 digit4 = byte2 & 0x0F;

    return QString("%1%2%3%4%5")
        .arg(dtcTypes[typeIndex])
        .arg(digit1)
        .arg(digit2, 1, 16, QChar('0'))
        .arg(digit3, 1, 16, QChar('0'))
        .arg(digit4, 1, 16, QChar('0'))
        .toUpper();
}

