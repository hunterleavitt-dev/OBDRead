#ifndef VEHICLEPROFILE_H
#define VEHICLEPROFILE_H

#include <QString>
#include <QMetaType>

/**
 * @brief The VehicleProfile struct
 * Contains vehicle identification information.
 */
struct VehicleProfile {
    int year = 0;              // Vehicle year (e.g., 2020)
    QString make;              // Vehicle make (e.g., "Toyota")
    QString model;             // Vehicle model (e.g., "Camry")
    QString vin;               // Vehicle Identification Number (optional)
    QString notes;             // User notes about the vehicle

    VehicleProfile() = default;
    
    VehicleProfile(int y, const QString& m, const QString& mod)
        : year(y), make(m), model(mod) {}
    
    bool isValid() const {
        return year > 0 && !make.isEmpty() && !model.isEmpty();
    }
    
    bool operator==(const VehicleProfile& other) const {
        return year == other.year &&
               make == other.make &&
               model == other.model &&
               vin == other.vin &&
               notes == other.notes;
    }
};

Q_DECLARE_METATYPE(VehicleProfile)

#endif // VEHICLEPROFILE_H
