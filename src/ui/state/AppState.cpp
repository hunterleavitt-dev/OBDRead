#include "AppState.h"
#include <QDebug>

AppState::AppState(QObject *parent)
    : QObject(parent)
    , m_connectionState(ConnectionState::Disconnected)
    , m_drivingMode(false)  // Start in Parked Mode
    , m_expertMode(false)
{
}

void AppState::setConnectionState(const ConnectionStateInfo& state)
{
    if (m_connectionState.state != state.state ||
        m_connectionState.lastError != state.lastError ||
        m_connectionState.protocolName != state.protocolName ||
        qAbs(m_connectionState.voltage - state.voltage) > 0.01 ||
        m_connectionState.ignitionState != state.ignitionState) {
        m_connectionState = state;
        emit connectionStateChanged(m_connectionState);
    }
}

void AppState::setConnectionState(ConnectionState state)
{
    if (m_connectionState.state != state) {
        m_connectionState.state = state;
        emit connectionStateChanged(m_connectionState);
    }
}

void AppState::setSelectedVehicleProfile(const VehicleProfile& profile)
{
    if (!(m_selectedVehicleProfile == profile)) {
        m_selectedVehicleProfile = profile;
        emit selectedVehicleProfileChanged(m_selectedVehicleProfile);
    }
}

void AppState::setDrivingMode(bool driving)
{
    if (m_drivingMode != driving) {
        m_drivingMode = driving;
        emit drivingModeChanged(m_drivingMode);
    }
}

void AppState::setLastScanResult(const ScanResult& result)
{
    if (!(m_lastScanResult == result)) {
        m_lastScanResult = result;
        emit lastScanResultChanged(m_lastScanResult);
    }
}

void AppState::setExpertMode(bool expert)
{
    if (m_expertMode != expert) {
        m_expertMode = expert;
        emit expertModeChanged(m_expertMode);
    }
}
