#ifndef DTCENTRY_H
#define DTCENTRY_H

#include <QString>
#include <QMetaType>
#include "FreezeFrame.h"

/**
 * @brief The DtcStatus enum
 * Represents the status of a Diagnostic Trouble Code.
 */
enum class DtcStatus {
    Confirmed,   // Confirmed/stored DTC
    Pending,     // Pending DTC (not yet confirmed)
    Permanent    // Permanent DTC (cannot be cleared)
};

Q_DECLARE_METATYPE(DtcStatus)

/**
 * @brief The DtcCategory enum
 * Represents the category of a DTC (derived from code prefix).
 */
enum class DtcCategory {
    P,  // Powertrain
    B,  // Body
    C,  // Chassis
    U   // Network
};

Q_DECLARE_METATYPE(DtcCategory)

/**
 * @brief The DtcEntry struct
 * Represents a single Diagnostic Trouble Code with metadata.
 */
struct DtcEntry {
    QString code;                    // DTC code (e.g., "P0420")
    QString shortText;               // Short description (optional)
    DtcStatus status = DtcStatus::Confirmed;
    DtcCategory category;            // Derived from code prefix
    QString module;                  // Module that reported the code (optional)
    FreezeFrame freezeFrame;         // Freeze frame data (optional)

    DtcEntry() = default;
    
    DtcEntry(const QString& c, DtcStatus s = DtcStatus::Confirmed)
        : code(c), status(s) {
        // Derive category from code prefix
        if (c.startsWith("P")) category = DtcCategory::P;
        else if (c.startsWith("B")) category = DtcCategory::B;
        else if (c.startsWith("C")) category = DtcCategory::C;
        else if (c.startsWith("U")) category = DtcCategory::U;
        else category = DtcCategory::P; // Default
    }
    
    bool isValid() const {
        return !code.isEmpty();
    }
    
    bool operator==(const DtcEntry& other) const {
        return code == other.code &&
               shortText == other.shortText &&
               status == other.status &&
               category == other.category &&
               module == other.module &&
               freezeFrame == other.freezeFrame;
    }
};

Q_DECLARE_METATYPE(DtcEntry)

#endif // DTCENTRY_H
