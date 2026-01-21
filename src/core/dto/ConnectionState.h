#ifndef CONNECTIONSTATE_H
#define CONNECTIONSTATE_H

#include <QString>
#include <QMetaType>

/**
 * @brief The ConnectionState enum
 * Represents the current connection state of the OBD adapter.
 */
enum class ConnectionState {
    Disconnected,      // No connection
    Connecting,        // Attempting to connect
    ConnectedAdapter,  // Connected to adapter but ECU not responding
    ConnectedEcu,      // Connected and ECU responding
    Error              // Connection error occurred
};

Q_DECLARE_METATYPE(ConnectionState)

/**
 * @brief The ConnectionStateInfo struct
 * Contains metadata about the connection state.
 */
struct ConnectionStateInfo {
    ConnectionState state = ConnectionState::Disconnected;
    QString lastError;           // Error message if state is Error
    QString protocolName;        // OBD protocol name (e.g., "ISO 9141-2", "CAN")
    double voltage = 0.0;        // Battery voltage (if available)
    bool ignitionState = false;  // Ignition on/off (if available)

    ConnectionStateInfo() = default;
    ConnectionStateInfo(ConnectionState s) : state(s) {}
    
    bool operator==(const ConnectionStateInfo& other) const {
        return state == other.state &&
               lastError == other.lastError &&
               protocolName == other.protocolName &&
               qAbs(voltage - other.voltage) < 0.01 &&
               ignitionState == other.ignitionState;
    }

    bool operator!=(const ConnectionStateInfo& other) const {
        return state != other.state ||
               lastError != other.lastError ||
               protocolName != other.protocolName ||
               qAbs(voltage - other.voltage) >= 0.01 ||
               ignitionState != other.ignitionState;
    }
};

Q_DECLARE_METATYPE(ConnectionStateInfo)

#endif // CONNECTIONSTATE_H
