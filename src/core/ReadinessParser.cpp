#include "ReadinessParser.h"
#include <QDebug>
#include <QByteArray>

ReadinessParser::ReadinessParser(QObject *parent)
    : QObject(parent)
{
}

ReadinessParser::~ReadinessParser()
{
}

ReadinessResult ReadinessParser::parseReadinessResponse(const QByteArray &rawData)
{
    ReadinessResult result;

    // Clean the input (remove whitespace, newlines, and the prompt '>')
    QByteArray clean = rawData.simplified();
    clean.replace(">", "");
    clean.replace(" ", "");

    // Handle "Empty" or "Error" responses
    if (clean.contains("NODATA") || clean.contains("NO DATA") || clean.contains("ERROR")) {
        return result; // Return empty result
    }

    // Convert Hex String to Byte Array
    QByteArray bytes = QByteArray::fromHex(clean);

    // Validate Mode 01 PID 01 Response (should start with 0x41 0x01)
    if (bytes.size() < 6 || bytes.at(0) != 0x41 || bytes.at(1) != 0x01) {
        qDebug() << "ReadinessParser: Not a valid Mode 01 PID 01 response:" << clean;
        return result;
    }

    // Mode 01 PID 01 response format:
    // Byte 0: 0x41 (Mode 01 response)
    // Byte 1: 0x01 (PID 01)
    // Byte 2: A - MIL status and DTC count (bit 7 = MIL, bits 0-6 = DTC count)
    // Byte 3: B - Monitor status (bits 0-7)
    // Byte 4: C - Monitor status (bits 0-7)
    // Byte 5+: Additional data (if present)

    // Parse monitor status bytes (B and C)
    quint8 byteB = bytes.size() > 3 ? (quint8)bytes.at(3) : 0;
    quint8 byteC = bytes.size() > 4 ? (quint8)bytes.at(4) : 0;

    // Monitor bit mappings (from OBD-II specification):
    // Byte B (bits 0-7):
    // Bit 0: Misfire Monitor (MIS)
    // Bit 1: Fuel System Monitor (FUEL)
    // Bit 2: Comprehensive Component Monitor (CCM)
    // Bit 3: Reserved
    // Bit 4: Catalyst Monitor (CAT)
    // Bit 5: Heated Catalyst Monitor (HCAT)
    // Bit 6: Evaporative System Monitor (EVAP)
    // Bit 7: Secondary Air System Monitor (SAS) - not commonly used

    // Byte C (bits 0-7):
    // Bit 0: Oxygen Sensor Monitor (O2S)
    // Bit 1: Oxygen Sensor Heater Monitor (O2SH)
    // Bit 2: EGR/VVT System Monitor (EGR)
    // Bit 3: PM Filter Monitor (PM)
    // Bit 4-7: Reserved

    // Parse Byte B monitors
    if (byteB & 0x01) {
        result.setMonitorStatus("MIS", MonitorStatus::Complete);
    } else {
        result.setMonitorStatus("MIS", MonitorStatus::Incomplete);
    }

    if (byteB & 0x02) {
        result.setMonitorStatus("FUEL", MonitorStatus::Complete);
    } else {
        result.setMonitorStatus("FUEL", MonitorStatus::Incomplete);
    }

    if (byteB & 0x04) {
        result.setMonitorStatus("CCM", MonitorStatus::Complete);
    } else {
        result.setMonitorStatus("CCM", MonitorStatus::Incomplete);
    }

    if (byteB & 0x10) {
        result.setMonitorStatus("CAT", MonitorStatus::Complete);
    } else {
        result.setMonitorStatus("CAT", MonitorStatus::Incomplete);
    }

    if (byteB & 0x20) {
        result.setMonitorStatus("HCAT", MonitorStatus::Complete);
    } else {
        result.setMonitorStatus("HCAT", MonitorStatus::Incomplete);
    }

    if (byteB & 0x40) {
        result.setMonitorStatus("EVAP", MonitorStatus::Complete);
    } else {
        result.setMonitorStatus("EVAP", MonitorStatus::Incomplete);
    }

    // Parse Byte C monitors
    if (byteC & 0x01) {
        result.setMonitorStatus("O2S", MonitorStatus::Complete);
    } else {
        result.setMonitorStatus("O2S", MonitorStatus::Incomplete);
    }

    if (byteC & 0x02) {
        result.setMonitorStatus("O2SH", MonitorStatus::Complete);
    } else {
        result.setMonitorStatus("O2SH", MonitorStatus::Incomplete);
    }

    // Note: We mark monitors as Incomplete if the bit is not set, but we don't know
    // if they're truly unsupported. For now, we'll assume all monitors are supported
    // if they appear in the response. Truly unsupported monitors would need additional
    // information from Mode 01 PID 01 extended data or other sources.

    return result;
}
