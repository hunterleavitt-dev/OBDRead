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

    // Validate Mode 01 PID 01 Response (should start with 0x41 0x01 and have 6 data bytes)
    if (bytes.size() < 6 || bytes.at(0) != 0x41 || bytes.at(1) != 0x01) {
        qDebug() << "ReadinessParser: Not a valid Mode 01 PID 01 response:" << clean;
        return result;
    }

    // Mode 01 PID 01 response format (per SAE J1979 / ISO 15031-5):
    // Byte 0: 0x41 (Mode 01 response)
    // Byte 1: 0x01 (PID 01)
    // Byte 2: A - MIL status (bit 7) and DTC count (bits 0-6)
    // Byte 3: B - Common monitors: availability (bits 0-2), engine type (bit 3), completeness (bits 4-6)
    // Byte 4: C - Engine-specific monitors availability
    // Byte 5: D - Engine-specific monitors completeness
    //
    // IMPORTANT: For availability bits, 1 = available. For completeness bits, 0 = complete, 1 = incomplete.

    quint8 byteB = (quint8)bytes.at(3);
    quint8 byteC = (quint8)bytes.at(4);
    quint8 byteD = (quint8)bytes.at(5);

    // Helper lambda to parse a monitor given availability and completeness bits
    // availBit: 1 = monitor is available/supported
    // compBit: 0 = complete, 1 = incomplete
    auto parseMonitor = [&result](const QString& name, bool available, bool incompleteFlag) {
        if (!available) {
            result.setMonitorStatus(name, MonitorStatus::Unsupported);
        } else if (incompleteFlag) {
            result.setMonitorStatus(name, MonitorStatus::Incomplete);
        } else {
            result.setMonitorStatus(name, MonitorStatus::Complete);
        }
    };

    // Parse common monitors from Byte B
    // Availability: B0=MIS, B1=FUEL, B2=CCM
    // Completeness: B4=MIS, B5=FUEL, B6=CCM (0 = complete, 1 = incomplete)
    parseMonitor("MIS",  (byteB & 0x01) != 0, (byteB & 0x10) != 0);  // Misfire
    parseMonitor("FUEL", (byteB & 0x02) != 0, (byteB & 0x20) != 0);  // Fuel System
    parseMonitor("CCM",  (byteB & 0x04) != 0, (byteB & 0x40) != 0);  // Comprehensive Component

    // Parse engine-specific monitors from Bytes C (availability) and D (completeness)
    // For spark ignition engines (assuming spark ignition for now):
    // C0/D0: Catalyst (CAT)
    // C1/D1: Heated Catalyst (HCAT)
    // C2/D2: Evaporative System (EVAP)
    // C3/D3: Secondary Air System (SAS)
    // C4/D4: Gasoline Particulate Filter (GPF) - formerly reserved/A/C
    // C5/D5: Oxygen Sensor (O2S)
    // C6/D6: Oxygen Sensor Heater (O2SH)
    // C7/D7: EGR and/or VVT System (EGR)

    parseMonitor("CAT",  (byteC & 0x01) != 0, (byteD & 0x01) != 0);  // Catalyst
    parseMonitor("HCAT", (byteC & 0x02) != 0, (byteD & 0x02) != 0);  // Heated Catalyst
    parseMonitor("EVAP", (byteC & 0x04) != 0, (byteD & 0x04) != 0);  // Evaporative System
    parseMonitor("SAS",  (byteC & 0x08) != 0, (byteD & 0x08) != 0);  // Secondary Air System
    parseMonitor("O2S",  (byteC & 0x20) != 0, (byteD & 0x20) != 0);  // Oxygen Sensor
    parseMonitor("O2SH", (byteC & 0x40) != 0, (byteD & 0x40) != 0);  // Oxygen Sensor Heater
    parseMonitor("EGR",  (byteC & 0x80) != 0, (byteD & 0x80) != 0);  // EGR/VVT System

    return result;
}
