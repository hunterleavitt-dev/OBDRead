#ifndef LOGMETA_H
#define LOGMETA_H

#include <QDateTime>
#include <QString>
#include <QMetaType>
#include "VehicleProfile.h"

/**
 * @brief The LogMeta struct
 * Contains metadata about a data log session.
 */
struct LogMeta {
    QString id;                     // Unique log identifier
    QDateTime timestamp;             // When logging started
    qint64 duration = 0;             // Duration in seconds
    int pidCount = 0;                // Number of PIDs being logged
    double sampleRate = 0.0;         // Samples per second
    VehicleProfile vehicleProfile;   // Vehicle profile at time of logging

    LogMeta() {
        timestamp = QDateTime::currentDateTime();
        id = QString::number(timestamp.toSecsSinceEpoch());
    }
    
    bool isValid() const {
        return !id.isEmpty() && duration > 0 && pidCount > 0;
    }
    
    bool operator==(const LogMeta& other) const {
        return id == other.id &&
               timestamp == other.timestamp &&
               duration == other.duration &&
               pidCount == other.pidCount &&
               qAbs(sampleRate - other.sampleRate) < 0.01 &&
               vehicleProfile == other.vehicleProfile;
    }
};

Q_DECLARE_METATYPE(LogMeta)

#endif // LOGMETA_H
