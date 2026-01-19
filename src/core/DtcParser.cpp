#include <QDebug>
#include "DtcParser.h"

DtcParser::DtcParser(QObject *parent)
    : QObject(parent)
{
}

DtcParser::~DtcParser()
{
}

// main function: converts raw ELM327 response to code list
QStringList DtcParser::parseDtcResponse(const QByteArray &rawData)
{
    QStringList dtcList;

    // TODO: implement DTC parsing logic
    // expected format: "43 XX YY XX YY ..." where 43 is mode response
    // each DTC is 2 bytes (XX YY)
    // Q_UNUSED(rawData)

    // 1. clean the input (remove whitespace, newlines, and the prompt '>')
    QByteArray clean = rawData.simplified();
    clean.replace(">", "");
    clean.replace(" ", ""); // Remove spaces so we have continuous hex: "430133..."

    // 2. handle "Empty" or "Error" responses
    if (clean.contains("NODATA")) return dtcList;
    if (clean.contains("ERROR")) return dtcList;

    // 3. convert Hex String to Byte Array
    QByteArray bytes = QByteArray::fromHex(clean);

    // 4. validate Mode 03 Response (first byte should be 0x43)
    if (bytes.isEmpty() || (quint8)bytes.at(0) != 0x43) {
        qDebug() << "Parser: Not a valid Mode 03 response:" << clean;
        return dtcList;
    }

    // 5. iterate through the byte pairs (skipping the first byte '43'); each code is 2 bytes.
    for (int i = 1; i < bytes.size(); i += 2) {
        if (i + 1 >= bytes.size()) break; // Safety check

        quint8 A = bytes.at(i);
        quint8 B = bytes.at(i+1);

        // If A and B are both 0, it's just padding at the end
        if (A == 0 && B == 0) continue;

        QString code = decodeDtc(A, B);
        dtcList.append(code);
    }

    return dtcList;
}

// helper: turns 2 bytes into "P0101"
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

