# PLAN.md - Strategic Roadmap & Status

## Project State
**Current Phase:** Phase 3 (Codes Tab)
**Active Branch:** `phase-2-implementation`
**Build Status:** Phase 2 tests passing (`tst_ReadinessParser`, `tst_ScanService`).

---

## Roadmap Overview

| Phase | Milestone | Status | Deliverables |
| :--- | :--- | :--- | :--- |
| **1** | Architecture & UI Skeleton | **Complete** | AppShell, AppState, DTOs, SafetyGate |
| **2** | Connection & Scan Pipeline | **Complete** | ScanService, SerialTransporter, HomeView |
| **3** | **Codes Tab (DTCs)** | **Active** | List/Filter, Freeze Frame, DTC Detail |
| **4** | Live Data Workbench | Pending | PidStreamService, Graphing, Dashboard |
| **5** | Logging & Playback | Pending | LogRecorder, CSV Export, Playback UI |
| **6** | Readiness & Clear Codes | Pending | Mode 04 Implementation, Safety Gating |
| **7** | Reporting & Advanced | Pending | JSON/HTML Reports, Mode $06 Stubs |

---

## Data Contract Registry (DTOs)
Located in `src/core/dto/`. These header-only structs define the communication boundary between `ScanService` and `UI`.

| DTO | Description | Status |
| :--- | :--- | :--- |
| `ConnectionStateInfo` | Adapter/ECU link status, voltage, protocol. | Stable |
| `ScanResult` | Snapshot of MIL, DTCs, and Readiness. | Stable |
| `DtcEntry` | Code, category (P/B/C/U), status, and Freeze Frame. | Stable |
| `ReadinessResult` | Per-monitor completion status. | Stable |
| `PidSample` | Timestamped PID value and units. | Phase 4 |
| `LogMeta` / `LogData` | Session metadata and sample series. | Phase 5 |

---

## Active Phase: Phase 3 — Codes Tab
**Objective:** Replace the placeholder `CodesView` with a functional DTC browser.

### 1. Functional Requirements
- **Data Source:** Observe `AppState::lastScanResultChanged`.
- **Filtering:** - Status: Confirmed, Pending, Permanent.
  - Category: Powertrain (P), Body (B), Chassis (C), Network (U).
- **Detail View:** Display `FreezeFrame` data (key-value pairs) when available.
- **Safety:** The "Clear Codes" button must be present but **disabled** (Logic follows in Phase 6).

### 2. Implementation Checklist
- [ ] Implement `QSortFilterProxyModel` for DTC filtering.
- [ ] Create `DtcDetailPanel` widget for the right-hand/bottom detail view.
- [ ] Map `DtcEntry::category` to human-readable strings (e.g., "P" -> "Powertrain").
- [ ] Ensure empty state (no codes) is handled gracefully.

---

## Future Backlog

### Phase 4: Live Data
- Implement `PidStreamService` for async polling.
- Integrate time-series graphing (Workbench) and Gauges (Dashboard).
- Apply Driving Mode restrictions to PID selection.

### Phase 5: Logging
- Disk-buffered recording to prevent UI lag.
- CSV/JSON export capability.

### Phase 6: Write Operations
- Implement Mode 04 (Clear Codes).
- **Hard Constraint:** Require `SafetyGate::RequireParked()` and triple-check confirmation.

### Phase 7: Reporting
- Generate human-readable summaries (JSON/HTML).
- Mode $06 diagnostic stubs in `AdvancedView`.

---

## On Phase Implementation
When completing a phase:
1. Move the phase status to **Complete** in the Roadmap Overview.
2. Update the **Project State** header.
3. Move the completed phase requirements to **Implementation History**.
4. Detail the requirements for the next **Active Phase**.