#ifndef DTCPARSER_H
#define DTCPARSER_H

#include <QObject>
#include <QString>
#include <QStringList>

/**
 * @brief The DtcParser class
 * Parses Diagnostic Trouble Code (DTC) responses from OBD-II adapters.
 * Mode 03 returns stored DTCs, Mode 07 returns pending DTCs.
 */
class DtcParser : public QObject
{
    Q_OBJECT

public:
    explicit DtcParser(QObject *parent = nullptr);
    ~DtcParser();

    /**
     * @brief Parses raw OBD-II response data for DTCs.
     * @param rawData The raw hex response from the adapter (e.g., "43 01 33 00 00 00 00").
     * @return A list of DTC strings (e.g., "P0133").
     */
    QStringList parseDtcResponse(const QByteArray &rawData);

    /**
     * @brief Converts a single DTC from raw bytes to standard format.
     * @param byte1 First byte of the DTC.
     * @param byte2 Second byte of the DTC.
     * @return The DTC string (e.g., "P0133").
     */
    static QString decodeDtc(quint8 byte1, quint8 byte2);
};

#endif // DTCPARSER_H
