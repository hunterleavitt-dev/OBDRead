#ifndef PIDSAMPLE_H
#define PIDSAMPLE_H

#include <QDateTime>
#include <QString>
#include <QMetaType>

/**
 * @brief The PidSample struct
 * Represents a single sample of PID data.
 */
struct PidSample {
    QDateTime timestamp;    // When the sample was taken
    QString pidId;          // PID identifier
    double value = 0.0;     // Sample value
    QString unit;           // Unit of measurement

    PidSample() {
        timestamp = QDateTime::currentDateTime();
    }
    
    PidSample(const QString& id, double val, const QString& u)
        : pidId(id), value(val), unit(u) {
        timestamp = QDateTime::currentDateTime();
    }
    
    bool isValid() const {
        return !pidId.isEmpty();
    }
    
    bool operator==(const PidSample& other) const {
        return timestamp == other.timestamp &&
               pidId == other.pidId &&
               qAbs(value - other.value) < 0.0001 &&
               unit == other.unit;
    }
};

Q_DECLARE_METATYPE(PidSample)

#endif // PIDSAMPLE_H
