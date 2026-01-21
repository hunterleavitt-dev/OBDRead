# OBDRead

A cross-platform desktop application for reading OBD-II diagnostic data from automobiles via hardware adapters. Built with Qt6 and C++17.

## Features

- **Diagnostic Trouble Code (DTC) Reading**: Parse and decode DTCs from OBD-II Mode 03 (stored codes) and Mode 07 (pending codes)
- **Multiple Connection Methods**: Support for TCP/IP (emulators) and Bluetooth Low Energy (BLE) adapters
- **Hardware Abstraction Layer**: Clean interface allowing easy addition of new transport mechanisms
- **Cross-Platform**: Runs on Windows, macOS, and Linux
- **Modern UI Architecture**: Tab-based navigation with centralized state management
- **Safety Features**: Driving/Parked mode with safety gating for dangerous actions
- **Extensible Design**: Multi-Phase implementation plan for incremental feature development

## Architecture

```
src/
├── core/
│   ├── dto/            # Data Transfer Objects (UI-facing contracts)
│   │   ├── ConnectionState.h
│   │   ├── VehicleProfile.h
│   │   ├── DtcEntry.h
│   │   ├── ScanResult.h
│   │   ├── ReadinessResult.h
│   │   ├── PidMeta.h
│   │   ├── PidSample.h
│   │   ├── LogMeta.h
│   │   └── LogData.h
│   ├── DtcParser       # Parses DTC responses into human-readable codes (P/C/B/U)
│   └── ObdCommand      # OBD-II command definitions
├── hardware/
│   ├── ObdTransporter  # Abstract interface for OBD communication
│   ├── TcpTransporter  # TCP/IP implementation (for emulators)
│   └── BleTransporter  # Bluetooth LE implementation (for Veepeak adapters)
└── ui/
    ├── state/
    │   └── AppState    # Central application state management
    ├── components/
    │   ├── StatusBar   # Persistent status bar component
    │   └── SafetyGate  # Safety gating utilities
    ├── views/          # Tab views (placeholders for multi-phase implementation)
    │   ├── HomeView
    │   ├── CodesView
    │   ├── LiveDataView
    │   ├── ReadinessView
    │   ├── AdvancedView
    │   ├── LogsView
    │   └── SettingsView
    └── MainWindow      # Main application window with navigation
```

### Architecture Layers

The application follows a clean separation of concerns:

- **UI Layer**: Widgets, views, and user interaction (`ui/views/`, `ui/components/`)
- **App State**: Single source of truth for UI-relevant state (`ui/state/AppState`)
- **Service/Adapters**: Thin layer calling existing backend (to be implemented in future phases)
- **Existing OBD Backend**: Transport/protocol/decoding (`hardware/`, `core/DtcParser`)
- **Data Contracts**: UI-facing DTOs for type-safe data exchange (`core/dto/`)

### DTC Code Types

The parser supports all four DTC categories:
- **P** (Powertrain): Engine, transmission, and drivetrain
- **C** (Chassis): ABS, steering, and suspension
- **B** (Body): Airbags, A/C, and lighting
- **U** (Network): CAN bus and communication modules

## Prerequisites

- **CMake** 3.16 or higher
- **Qt6** with the following modules:
  - Qt6::Widgets
  - Qt6::Network
  - Qt6::Test (for running tests)
- **C++17** compatible compiler

## Building

### Configure and Build

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### Build with a Specific Generator (Windows)

```bash
cmake -G "Visual Studio 17 2022" ..
cmake --build . --config Release
```

### Build with Qt Creator

1. Open `CMakeLists.txt` in Qt Creator
2. Configure the project with your Qt6 kit
3. Build and run

## Usage

### With an OBD-II Emulator (TCP)

For development and testing, connect to an ELM327 emulator over TCP:

```cpp
m_transporter = new TcpTransporter(this);
m_transporter->connectToDevice("192.168.0.10:35000");
```

### With a BLE Adapter (Hardware)

For real vehicle diagnostics using a Veepeak OBDCheck BLE+ or similar:

```cpp
m_transporter = new BleTransporter(this);
m_transporter->connectToDevice("AA:BB:CC:DD:EE:FF");
```

> **Note**: BLE support is currently a stub implementation. Full BLE functionality requires the Qt Bluetooth module and platform-specific configuration.

### Sending Commands

```cpp
// Initialize the adapter
m_transporter->sendCommand("AT Z\r");      // Reset
m_transporter->sendCommand("AT E0\r");     // Echo off
m_transporter->sendCommand("AT SP 0\r");   // Auto-detect protocol

// Read DTCs
m_transporter->sendCommand("03\r");        // Mode 03: Stored DTCs
m_transporter->sendCommand("07\r");        // Mode 07: Pending DTCs
```

## Testing

The project uses Qt Test for unit testing.

### Run Tests

```bash
cd build
ctest --output-on-failure
```

Or run the test executable directly:

```bash
./OBDReadTests
```

### Test Coverage

Current tests cover:
- DTC decoding for all code types (Powertrain, Chassis, Body, Network)
- Byte-to-code conversion accuracy
- Data Transfer Objects (DTOs) - all core DTO types and their operations
- AppState management - state transitions and signal emissions

## Project Status

This project is in early development (v0.1). The implementation follows a multi-phase plan for incremental feature development.

### Phase 1 - Architecture + UI Skeleton (Completed)

- [x] Shared Data Contracts (DTOs) - UI-facing data structures
- [x] AppState management - centralized state with signals
- [x] Navigation shell - tab-based UI with 7 main sections
- [x] Status bar component - connection state and system info
- [x] Safety gating utilities - Driving/Parked mode support
- [x] Placeholder views - all tabs render with explicit placeholders
- [x] Unit tests - DTOs and AppState coverage

### Backend (Existing)

- [x] Hardware abstraction layer (ObdTransporter interface)
- [x] TCP transport for emulator testing
- [x] DTC parsing and decoding
- [ ] Full BLE transport implementation

### Future Phases (Planned)

- [ ] Phase 2: Connection + Health Check + Scan Pipeline
- [ ] Phase 3: Codes Tab - List + Filters + Detail
- [ ] Phase 4: Live Data - Workbench + Dashboard
- [ ] Phase 5: Logging + Playback + Export
- [ ] Phase 6: Readiness Detail + Clear Codes
- [ ] Phase 7: Reporting + Advanced + Polish

## License

This project is licensed under the **GNU General Public License v3.0** - see the [LICENSE](LICENSE) file for details.
