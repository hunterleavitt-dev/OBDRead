#ifndef PIDMETA_H
#define PIDMETA_H

#include <QString>
#include <QMetaType>

/**
 * @brief The PidCategory enum
 * Represents the category of a PID.
 */
enum class PidCategory {
    Engine,
    Transmission,
    Fuel,
    Emissions,
    Vehicle,
    Other
};

Q_DECLARE_METATYPE(PidCategory)

/**
 * @brief The PidMeta struct
 * Contains metadata about a PID (Parameter ID).
 */
struct PidMeta {
    QString pidId;                  // PID identifier (e.g., "0100", "0105")
    QString name;                   // Human-readable name
    QString unit;                   // Unit of measurement (e.g., "rpm", "Centigrade", "kPa")
    PidCategory category = PidCategory::Other;
    bool supported = false;         // Whether PID is supported by vehicle

    PidMeta() = default;
    
    PidMeta(const QString& id, const QString& n, const QString& u)
        : pidId(id), name(n), unit(u) {}
    
    bool isValid() const {
        return !pidId.isEmpty() && !name.isEmpty();
    }
    
    bool operator==(const PidMeta& other) const {
        return pidId == other.pidId &&
               name == other.name &&
               unit == other.unit &&
               category == other.category &&
               supported == other.supported;
    }
};

Q_DECLARE_METATYPE(PidMeta)

#endif // PIDMETA_H
