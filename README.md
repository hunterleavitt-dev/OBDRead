# OBDRead

A cross-platform desktop application for reading OBD-II diagnostic data from automobiles via ELM327 hardware adapters. Built with Qt6 and C++17.

## Table of Contents
- [Features](#features)
- [Architecture](#architecture)
  - [Architecture Layers](#architecture-layers)
  - [DTC Code Types](#dtc-code-types)
- [Prerequisites](#prerequisites)
- [Building](#building)
  - [Configure and Build](#configure-and-build)
  - [Build with a Specific Generator (Windows)](#build-with-a-specific-generator-windows)
  - [Build with Qt Creator](#build-with-qt-creator)
- [Usage](#usage)
  - [Connecting to an OBD-II Adapter](#connecting-to-an-obd-ii-adapter)
  - [Running a Diagnostic Scan](#running-a-diagnostic-scan)
  - [Connection Troubleshooting](#connection-troubleshooting)
- [Testing](#testing)
  - [Run Tests](#run-tests)
  - [Test Coverage](#test-coverage)
- [Project Status](#project-status)
  - [Phase 1 - Architecture + UI Skeleton (Completed)](#phase-1---architecture--ui-skeleton-completed)
  - [Phase 2 - Connection + Health Check + Scan Pipeline (Completed)](#phase-2---connection--health-check--scan-pipeline-completed)
  - [Backend (Existing)](#backend-existing)
  - [Future Phases (Planned)](#future-phases-planned)
- [License](#license)

## Features

- **Diagnostic Trouble Code (DTC) Reading**: Parse and decode DTCs from OBD-II Mode 03 (stored codes) and Mode 07 (pending codes)
- **Multiple Connection Methods**: Support for Serial/PTY (ELM327 adapters), TCP/IP (emulators), and Bluetooth Low Energy (planned)
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
│   │   ├── FreezeFrame.h
│   │   ├── DtcEntry.h
│   │   ├── ScanResult.h
│   │   ├── ReadinessResult.h
│   │   ├── PidMeta.h
│   │   ├── PidSample.h
│   │   ├── LogMeta.h
│   │   └── LogData.h
│   ├── DtcParser       # Parses DTC responses into human-readable codes (P/C/B/U)
│   ├── ReadinessParser # Parses Mode 01 PID 01 readiness monitor responses
│   ├── ScanService     # Manages scan pipeline and command sequencing
│   └── ObdCommand      # OBD-II command definitions
├── hardware/
│   ├── ObdTransporter      # Abstract interface for OBD communication
│   ├── SerialTransporter   # Serial/PTY implementation (primary transport)
│   ├── TcpTransporter      # TCP/IP implementation (for emulators)
│   └── BleTransporter      # Bluetooth LE implementation (stub, planned)
└── ui/
    ├── state/
    │   └── AppState    # Central application state management
    ├── components/
    │   ├── StatusBar   # Persistent status bar component
    │   └── SafetyGate  # Safety gating utilities
    ├── views/          # Tab views
    │   ├── HomeView    # Connection controls and scan summary (Phase 2)
    │   ├── CodesView   # DTC list and filters (Phase 3)
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
- **Service Layer**: Scan pipeline management and OBD command sequencing (`core/ScanService`)
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
  - Qt6::SerialPort
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

### Connecting to an OBD-II Adapter

1. **Launch the application** - The main window opens with the Home/Health tab active.

2. **Enter connection address:**
   - **Serial/PTY**: Enter the device path (e.g., `/dev/pts/3` or `/dev/ttyUSB0`)
   - **TCP Emulator**: Enter IP address and port (e.g., `127.0.0.1:35000`)

3. **Click "Connect"** - The application will:
   - Connect to the adapter
   - Initialize the adapter (reset, echo off, protocol detection)
   - Ping the ECU to verify communication
   - Detect and display the OBD protocol

4. **Connection States:**
   - **Disconnected**: No connection
   - **Connecting**: Establishing connection
   - **Adapter Connected (No ECU)**: Adapter connected but ECU not responding
   - **Connected to ECU**: Ready for scanning

### Running a Diagnostic Scan

1. **Ensure connected to ECU** - The "Scan" button is only enabled when connected to ECU.

2. **Click "Scan"** - The application will:
   - Read MIL (Malfunction Indicator Lamp) status
   - Retrieve stored DTCs (Mode 03)
   - Retrieve pending DTCs (Mode 07)
   - Read readiness monitor status (Mode 01 PID 01)

3. **View Results** - The Home/Health tab displays:
   - **MIL Status**: ON (red) or OFF (green)
   - **DTC Count**: Total codes with breakdown by status
   - **Readiness Status**: Ready (green) or Not Ready (orange)
   - **Last Scan Time**: Timestamp of the most recent scan

### Connection Troubleshooting

- **"Adapter Connected (No ECU)"**: The adapter is connected but the ECU is not responding. Check:
  - Vehicle ignition is ON
  - OBD-II adapter is properly plugged into the vehicle's OBD port
  - Vehicle supports OBD-II protocol

- **Connection Timeout**: The adapter did not respond. Check:
  - Serial device path exists and is accessible (for Serial/PTY)
  - Network connectivity (for TCP)
  - Adapter power and status

- **Scan Errors**: Some scan operations may fail while others succeed. The application will display partial results when available.

## Testing

The project uses Qt Test for unit testing.

### Run Tests

```bash
cd build
ctest --output-on-failure
```

Or run individual test executables directly:

```bash
./tst_DtcParser
./tst_ReadinessParser
./tst_ScanService
./tst_DtoTests
./tst_AppStateTests
```

### Test Coverage

Current tests cover:
- DTC decoding for all code types (Powertrain, Chassis, Body, Network)
- DTC parsing for Mode 03 (stored) and Mode 07 (pending)
- Byte-to-code conversion accuracy
- Readiness monitor parsing (Mode 01 PID 01)
- Data Transfer Objects (DTOs) - all core DTO types and their operations
- AppState management - state transitions and signal emissions
- ScanService - scan pipeline and state management

## Project Status

This project is in early development (v0.2). The implementation follows a multi-phase plan for incremental feature development.

### Phase 1 - Architecture + UI Skeleton (Completed)

- [x] Shared Data Contracts (DTOs) - UI-facing data structures
- [x] AppState management - centralized state with signals
- [x] Navigation shell - tab-based UI with 7 main sections
- [x] Status bar component - connection state and system info
- [x] Safety gating utilities - Driving/Parked mode support
- [x] Placeholder views - all tabs render with explicit placeholders
- [x] Unit tests - DTOs and AppState coverage

### Phase 2 - Connection + Health Check + Scan Pipeline (Completed)

- [x] Connection flow with state transitions (Disconnected → Connecting → ConnectedAdapter → ConnectedEcu)
- [x] Protocol detection and display
- [x] ScanService - manages scan pipeline and command sequencing
- [x] ReadinessParser - parses Mode 01 PID 01 responses
- [x] Enhanced DtcParser - supports Mode 03 (stored) and Mode 07 (pending) DTCs
- [x] Home/Health screen - Connect/Disconnect/Scan controls
- [x] Summary tiles - MIL status, DTC counts, Readiness status, Last scan time
- [x] Error handling - graceful handling of connection and scan failures
- [x] Unit tests - ReadinessParser and ScanService coverage

### Backend (Existing)

- [x] Hardware abstraction layer (ObdTransporter interface)
- [x] Serial/PTY transport (primary, for ELM327 adapters)
- [x] TCP transport for emulator testing
- [x] DTC parsing and decoding (Mode 03 and Mode 07)
- [x] Readiness monitor parsing
- [ ] Full BLE transport implementation (currently stub)

### Future Phases (Planned)

- [ ] Phase 3: Codes Tab - List + Filters + Detail
- [ ] Phase 4: Live Data - Workbench + Dashboard
- [ ] Phase 5: Logging + Playback + Export
- [ ] Phase 6: Readiness Detail + Clear Codes
- [ ] Phase 7: Reporting + Advanced + Polish

## License

This project is licensed under the **GNU General Public License v3.0** - see the [LICENSE](LICENSE) file for details.

## Acknowledgements

This project uses the [ELM327-emulator](https://github.com/Ircama/ELM327-emulator) by Ircama to simulate ECU responses during development.

**License Notice:** The emulator is used as an external testing tool and is licensed under [CC BY-NC-SA 4.0](https://creativecommons.org/licenses/by-nc-sa/4.0/).


