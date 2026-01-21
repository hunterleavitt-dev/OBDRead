#ifndef APPSTATE_H
#define APPSTATE_H

#include <QObject>
#include <QString>
#include "core/dto/ConnectionState.h"
#include "core/dto/VehicleProfile.h"
#include "core/dto/ScanResult.h"

// Forward declarations to avoid circular includes
class AppState;

/**
 * @brief The AppState class
 * Central state management for the application.
 * Single source of truth for UI-relevant state.
 */
class AppState : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ConnectionStateInfo connectionState READ connectionState NOTIFY connectionStateChanged)
    Q_PROPERTY(VehicleProfile selectedVehicleProfile READ selectedVehicleProfile NOTIFY selectedVehicleProfileChanged)
    Q_PROPERTY(bool drivingMode READ drivingMode NOTIFY drivingModeChanged)
    Q_PROPERTY(ScanResult lastScanResult READ lastScanResult NOTIFY lastScanResultChanged)
    Q_PROPERTY(bool expertMode READ expertMode NOTIFY expertModeChanged)

public:
    explicit AppState(QObject *parent = nullptr);
    ~AppState() = default;

    // Getters
    ConnectionStateInfo connectionState() const { return m_connectionState; }
    VehicleProfile selectedVehicleProfile() const { return m_selectedVehicleProfile; }
    bool drivingMode() const { return m_drivingMode; }
    ScanResult lastScanResult() const { return m_lastScanResult; }
    bool expertMode() const { return m_expertMode; }

    // Setters
    void setConnectionState(const ConnectionStateInfo& state);
    void setConnectionState(ConnectionState state);
    void setSelectedVehicleProfile(const VehicleProfile& profile);
    void setDrivingMode(bool driving);
    void setLastScanResult(const ScanResult& result);
    void setExpertMode(bool expert);

signals:
    void connectionStateChanged(const ConnectionStateInfo& state);
    void selectedVehicleProfileChanged(const VehicleProfile& profile);
    void drivingModeChanged(bool driving);
    void lastScanResultChanged(const ScanResult& result);
    void expertModeChanged(bool expert);

private:
    ConnectionStateInfo m_connectionState;
    VehicleProfile m_selectedVehicleProfile;
    bool m_drivingMode = false;  // false = Parked Mode, true = Driving Mode
    ScanResult m_lastScanResult;
    bool m_expertMode = false;
};

#endif // APPSTATE_H
