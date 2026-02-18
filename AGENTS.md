# AGENTS.md - Development Constraints

## Architectural Pattern: Signal-Driven Flow
- **Data flows downward:** UI -> Service -> Hardware (via method calls).
- **State flows upward:** Hardware -> Service -> AppState -> UI (via Qt Signals).
- **Single Source of Truth:** `AppState` owns all UI-relevant state. Views must observe `AppState`, never cache local copies of shared data.
- **Abstraction:** All hardware logic must stay behind the `ObdTransporter` interface.

## Implementation Patterns
- **Views:** Programmatic layouts only. **Strictly no `.ui` files** for view classes, excepting the existing `mainwindow`. Use `setAppState(AppState*)` for wiring.
- **DTOs:** Located in `src/core/dto/`. Must remain header-only, standard C++ structs (e.g., no `Q_OBJECT`).
- **Tests:** Use the `create_obd_test` CMake macro. Each class requires a corresponding `tst_ClassName.cpp`.

## Safety Guardrails
- **SafetyGate:** Methods like `RequireParked()` are mandatory for any write operations (e.g., clearing codes). Do **not** bypass.
- **Driving Mode:** Must remain read-only.
- **Interface Integrity:** Do not modify `ObdTransporter` virtual signatures without updating all three backends (Serial, TCP, BLE).

## Project Context
- **Primary Roadmap:** Refer to `design/PLAN.md` for current phase goals and acceptance criteria.
- **Standards:** C++17, Qt6 (Widgets/Network/SerialPort).