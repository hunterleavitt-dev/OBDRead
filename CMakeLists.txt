cmake_minimum_required(VERSION 3.16)

project(OBDRead VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(OBDRead
        MANUAL_FINALIZATION
        src/main.cpp
        src/ui/mainwindow.cpp
        src/ui/mainwindow.h
        src/ui/mainwindow.ui
        src/hardware/ObdTransporter.h
        src/hardware/TcpTransporter.h
        src/hardware/TcpTransporter.cpp
        src/core/DtcParser.h
        src/core/DtcParser.cpp
        src/core/ObdCommand.h
        src/hardware/BleTransporter.h
        src/hardware/BleTransporter.cpp
    )
endif()

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

# Create a separate executable for tests
qt_add_executable(OBDReadTests
    tests/tst_DtcParser.cpp
    src/core/DtcParser.cpp   # (Include the file we are testing)
)

# Link against Qt Test and any other modules DtcParser needs (e.g., Core/Network)
target_link_libraries(OBDReadTests PRIVATE
    Qt6::Test
    Qt6::Core
    # Qt6::Network  <-- Uncomment if DtcParser ends up using networking
)

# Include directories for tests
target_include_directories(OBDReadTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware
)

# Register the test so Qt Creator's "Test" panel sees it
add_test(NAME DtcParserTest COMMAND OBDReadTests)
