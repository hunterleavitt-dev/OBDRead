#ifndef READINESSRESULT_H
#define READINESSRESULT_H

#include <QString>
#include <QMap>
#include <QMetaType>

/**
 * @brief The MonitorStatus enum
 * Represents the status of an emissions monitor.
 */
enum class MonitorStatus {
    Complete,     // Monitor test complete
    Incomplete,   // Monitor test incomplete
    Unsupported   // Monitor not supported by vehicle
};

Q_DECLARE_METATYPE(MonitorStatus)

/**
 * @brief The ReadinessResult struct
 * Contains readiness test results for emissions monitors.
 */
struct ReadinessResult {
    bool overallReady = false;  // Overall readiness status
    QMap<QString, MonitorStatus> monitors;  // Monitor name -> status

    ReadinessResult() = default;
    
    void setMonitorStatus(const QString& monitorName, MonitorStatus status) {
        monitors[monitorName] = status;
        updateOverallReady();
    }
    
    MonitorStatus getMonitorStatus(const QString& monitorName) const {
        return monitors.value(monitorName, MonitorStatus::Unsupported);
    }
    
    void updateOverallReady() {
        if (monitors.isEmpty()) {
            overallReady = false;
            return;
        }
        
        // Overall ready if all supported monitors are complete
        overallReady = true;
        for (auto it = monitors.begin(); it != monitors.end(); ++it) {
            if (it.value() == MonitorStatus::Unsupported) {
                continue; // Skip unsupported monitors
            }
            if (it.value() != MonitorStatus::Complete) {
                overallReady = false;
                break;
            }
        }
    }
    
    bool operator==(const ReadinessResult& other) const {
        return overallReady == other.overallReady &&
               monitors == other.monitors;
    }
};

Q_DECLARE_METATYPE(ReadinessResult)

#endif // READINESSRESULT_H
