cmake_minimum_required(VERSION 3.16)

project(OBDRead VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network Test SerialPort)

qt_add_executable(OBDRead
    MANUAL_FINALIZATION
        src/main.cpp
        # Core DTOs
        src/core/dto/ConnectionState.h
        src/core/dto/VehicleProfile.h
        src/core/dto/FreezeFrame.h
        src/core/dto/DtcEntry.h
        src/core/dto/ReadinessResult.h
        src/core/dto/ScanResult.h
        src/core/dto/PidMeta.h
        src/core/dto/PidSample.h
        src/core/dto/LogMeta.h
        src/core/dto/LogData.h
        # Core
        src/core/DtcParser.h
        src/core/DtcParser.cpp
        src/core/ReadinessParser.h
        src/core/ReadinessParser.cpp
        src/core/ScanService.h
        src/core/ScanService.cpp
        src/core/ObdCommand.h
        # Hardware
        src/hardware/ObdTransporter.h
        src/hardware/TcpTransporter.h
        src/hardware/TcpTransporter.cpp
        src/hardware/BleTransporter.h
        src/hardware/BleTransporter.cpp
        # UI State
        src/ui/state/AppState.h
        src/ui/state/AppState.cpp
        # UI Components
        src/ui/components/StatusBar.h
        src/ui/components/StatusBar.cpp
        src/ui/components/SafetyGate.h
        src/ui/components/SafetyGate.cpp
        # UI Views
        src/ui/views/HomeView.h
        src/ui/views/HomeView.cpp
        src/ui/views/CodesView.h
        src/ui/views/CodesView.cpp
        src/ui/views/LiveDataView.h
        src/ui/views/LiveDataView.cpp
        src/ui/views/ReadinessView.h
        src/ui/views/ReadinessView.cpp
        src/ui/views/AdvancedView.h
        src/ui/views/AdvancedView.cpp
        src/ui/views/LogsView.h
        src/ui/views/LogsView.cpp
        src/ui/views/SettingsView.h
        src/ui/views/SettingsView.cpp
        # UI MainWindow
        src/ui/mainwindow.cpp
        src/ui/mainwindow.h
        src/ui/mainwindow.ui
    )

# --- Link Libraries ---
target_link_libraries(OBDRead PRIVATE Qt6::Widgets Qt6::Network)

# --- Include Directories ---
target_include_directories(OBDRead PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
)

# --- Target Properties (Icons, Bundle IDs, Windows Console) ---
set_target_properties(OBDRead PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER dev.hunterleavitt.OBDRead
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# --- Installation Logic ---
include(GNUInstallDirs)

install(TARGETS OBDRead
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# --- Finalization (REQUIRED for Qt 6 "MANUAL_FINALIZATION") ---
qt_finalize_executable(OBDRead)

# --- Testing Configuration ---
enable_testing()

# Find the Qt Test module
find_package(Qt6 REQUIRED COMPONENTS Test)

# Define the implementation files (.cpp) that tests rely on
set(TEST_IMPL_SOURCES
    src/core/DtcParser.cpp
    src/core/ReadinessParser.cpp
    src/core/ScanService.cpp
    src/ui/state/AppState.cpp
    # Add other .cpp files here for future tests

    src/hardware/ObdTransporter.h # TODO: define core logic as a Library that both the main app and tests link against
)

# Define a macro to streamline creating test targets
macro(create_obd_test test_name source_file)
    qt_add_executable(${test_name}
        ${source_file}        # The specific test file (e.g., tst_DtcParser.cpp)
        ${TEST_IMPL_SOURCES}  # The app logic needed to run the test
    )

    target_link_libraries(${test_name} PRIVATE
        Qt6::Test
        Qt6::Core
        Qt6::Widgets
        Qt6::Network
    )

    target_include_directories(${test_name} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware
    )

    # Register the test so ctest / Qt Creator sees it
    add_test(NAME ${test_name} COMMAND ${test_name})
endmacro()

# Generate separate executables
create_obd_test(tst_DtcParser   tests/tst_DtcParser.cpp)
create_obd_test(tst_DtoTests    tests/tst_DtoTests.cpp)
create_obd_test(tst_AppStateTests tests/tst_AppStateTests.cpp)
create_obd_test(tst_ReadinessParser tests/tst_ReadinessParser.cpp)
create_obd_test(tst_ScanService tests/tst_ScanService.cpp)
