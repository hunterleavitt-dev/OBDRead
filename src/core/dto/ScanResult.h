#ifndef SCANRESULT_H
#define SCANRESULT_H

#include <QDateTime>
#include <QVector>
#include <QString>
#include <QMetaType>
#include "core/dto/DtcEntry.h"
#include "core/dto/ReadinessResult.h"

/**
 * @brief The ModuleInfo struct
 * Represents information about an ECU module.
 */
struct ModuleInfo {
    QString name;           // Module name/identifier
    QString address;        // Module address (hex)
    bool responding = false; // Whether module responds

    ModuleInfo() = default;
    
    ModuleInfo(const QString& n, const QString& addr)
        : name(n), address(addr), responding(true) {}
    
    bool operator==(const ModuleInfo& other) const {
        return name == other.name &&
               address == other.address &&
               responding == other.responding;
    }
};

Q_DECLARE_METATYPE(ModuleInfo)

/**
 * @brief The ScanResult struct
 * Contains the results of a diagnostic scan.
 */
struct ScanResult {
    QDateTime timestamp;                    // When the scan was performed
    bool milOn = false;                     // MIL (Malfunction Indicator Lamp) status
    QVector<DtcEntry> dtcs;                 // List of DTCs found
    ReadinessResult readiness;              // Readiness test results
    QVector<ModuleInfo> modules;            // Detected modules (optional)

    ScanResult() {
        timestamp = QDateTime::currentDateTime();
    }
    
    int getDtcCount(DtcStatus status) const {
        int count = 0;
        for (const auto& dtc : dtcs) {
            if (dtc.status == status) {
                count++;
            }
        }
        return count;
    }
    
    bool isEmpty() const {
        return dtcs.isEmpty() && !milOn;
    }
    
    bool operator==(const ScanResult& other) const {
        return timestamp == other.timestamp &&
               milOn == other.milOn &&
               dtcs == other.dtcs &&
               readiness == other.readiness &&
               modules == other.modules;
    }
};

Q_DECLARE_METATYPE(ScanResult)

#endif // SCANRESULT_H
