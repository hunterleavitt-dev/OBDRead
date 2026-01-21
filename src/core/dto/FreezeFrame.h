#ifndef FREEZEFRAME_H
#define FREEZEFRAME_H

#include <QString>
#include <QMap>
#include <QMetaType>

/**
 * @brief The FreezeFrame struct
 * Contains freeze frame data (key-value pairs with units).
 * Freeze frames capture vehicle conditions when a DTC was set.
 */
struct FreezeFrame {
    QMap<QString, QString> parameters;  // Parameter name -> value with unit
    int frameNumber = 0;                // Freeze frame number

    FreezeFrame() = default;
    
    FreezeFrame(int frameNum) : frameNumber(frameNum) {}
    
    void addParameter(const QString& name, const QString& valueWithUnit) {
        parameters[name] = valueWithUnit;
    }
    
    bool isEmpty() const {
        return parameters.isEmpty();
    }
    
    bool operator==(const FreezeFrame& other) const {
        return frameNumber == other.frameNumber &&
               parameters == other.parameters;
    }
};

Q_DECLARE_METATYPE(FreezeFrame)

#endif // FREEZEFRAME_H
