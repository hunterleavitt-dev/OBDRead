#ifndef LOGDATA_H
#define LOGDATA_H

#include <QVector>
#include <QMetaType>
#include "LogMeta.h"
#include "PidSample.h"

/**
 * @brief The LogData struct
 * Contains complete log data: metadata and time series samples.
 */
struct LogData {
    LogMeta meta;                           // Log metadata
    QVector<PidSample> samples;             // Time series of PID samples

    LogData() = default;
    
    LogData(const LogMeta& m) : meta(m) {}
    
    bool isEmpty() const {
        return samples.isEmpty();
    }
    
    int getSampleCount() const {
        return samples.size();
    }
    
    bool operator==(const LogData& other) const {
        return meta == other.meta &&
               samples == other.samples;
    }
};

Q_DECLARE_METATYPE(LogData)

#endif // LOGDATA_H
