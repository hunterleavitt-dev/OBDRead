#include <QtTest/QtTest>
#include "ui/state/AppState.h"
#include "core/dto/ConnectionState.h"
#include "core/dto/VehicleProfile.h"
#include "core/dto/ScanResult.h"

class TestAppState : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testInitialState();
    void testConnectionStateChanges();
    void testDrivingModeToggle();
    void testVehicleProfile();
    void testScanResult();
    void testExpertMode();
    void testSignalEmission();

private:
    AppState* m_appState = nullptr;
    bool m_connectionStateChanged = false;
    bool m_drivingModeChanged = false;
    bool m_expertModeChanged = false;
    bool m_scanResultChanged = false;
    bool m_vehicleProfileChanged = false;
};

void TestAppState::initTestCase()
{
    m_appState = new AppState(this);
    
    // Connect signals to track emissions
    connect(m_appState, &AppState::connectionStateChanged, this, [this](const ConnectionStateInfo&) {
        m_connectionStateChanged = true;
    });
    connect(m_appState, &AppState::drivingModeChanged, this, [this](bool) {
        m_drivingModeChanged = true;
    });
    connect(m_appState, &AppState::expertModeChanged, this, [this](bool) {
        m_expertModeChanged = true;
    });
    connect(m_appState, &AppState::lastScanResultChanged, this, [this](const ScanResult&) {
        m_scanResultChanged = true;
    });
    connect(m_appState, &AppState::selectedVehicleProfileChanged, this, [this](const VehicleProfile&) {
        m_vehicleProfileChanged = true;
    });
}

void TestAppState::cleanupTestCase()
{
    // m_appState is deleted automatically as child of this
}

void TestAppState::testInitialState()
{
    QVERIFY(m_appState != nullptr);
    QCOMPARE(m_appState->connectionState().state, ConnectionState::Disconnected);
    QVERIFY(!m_appState->drivingMode());  // Should start in Parked Mode
    QVERIFY(!m_appState->expertMode());
    QVERIFY(m_appState->lastScanResult().isEmpty());
    QVERIFY(!m_appState->selectedVehicleProfile().isValid());
}

void TestAppState::testConnectionStateChanges()
{
    m_connectionStateChanged = false;

    ConnectionStateInfo info;
    info.state = ConnectionState::Connecting;
    m_appState->setConnectionState(info);

    QVERIFY(m_connectionStateChanged);
    QCOMPARE(m_appState->connectionState().state, ConnectionState::Connecting);

    m_connectionStateChanged = false;
    m_appState->setConnectionState(ConnectionState::ConnectedEcu);
    QVERIFY(m_connectionStateChanged);
    QCOMPARE(m_appState->connectionState().state, ConnectionState::ConnectedEcu);
}

void TestAppState::testDrivingModeToggle()
{
    m_drivingModeChanged = false;

    QVERIFY(!m_appState->drivingMode());  // Start in Parked Mode

    m_appState->setDrivingMode(true);
    QVERIFY(m_drivingModeChanged);
    QVERIFY(m_appState->drivingMode());

    m_drivingModeChanged = false;
    m_appState->setDrivingMode(false);
    QVERIFY(m_drivingModeChanged);
    QVERIFY(!m_appState->drivingMode());
}

void TestAppState::testVehicleProfile()
{
    m_vehicleProfileChanged = false;

    VehicleProfile profile(2020, "Toyota", "Camry");
    profile.vin = "1HGBH41JXMN109186";
    
    m_appState->setSelectedVehicleProfile(profile);
    QVERIFY(m_vehicleProfileChanged);
    
    VehicleProfile retrieved = m_appState->selectedVehicleProfile();
    QCOMPARE(retrieved.year, 2020);
    QCOMPARE(retrieved.make, QString("Toyota"));
    QCOMPARE(retrieved.model, QString("Camry"));
    QCOMPARE(retrieved.vin, QString("1HGBH41JXMN109186"));
}

void TestAppState::testScanResult()
{
    m_scanResultChanged = false;

    ScanResult result;
    result.milOn = true;
    result.dtcs.append(DtcEntry("P0420", DtcStatus::Confirmed));

    m_appState->setLastScanResult(result);
    QVERIFY(m_scanResultChanged);

    ScanResult retrieved = m_appState->lastScanResult();
    QVERIFY(retrieved.milOn);
    QCOMPARE(retrieved.dtcs.size(), 1);
    QCOMPARE(retrieved.dtcs[0].code, QString("P0420"));
}

void TestAppState::testExpertMode()
{
    m_expertModeChanged = false;

    QVERIFY(!m_appState->expertMode());

    m_appState->setExpertMode(true);
    QVERIFY(m_expertModeChanged);
    QVERIFY(m_appState->expertMode());

    m_expertModeChanged = false;
    m_appState->setExpertMode(false);
    QVERIFY(m_expertModeChanged);
    QVERIFY(!m_appState->expertMode());
}

void TestAppState::testSignalEmission()
{
    // Test that signals are not emitted when value doesn't change
    m_connectionStateChanged = false;
    m_drivingModeChanged = false;
    m_expertModeChanged = false;

    // Set to same values
    ConnectionStateInfo currentState = m_appState->connectionState();
    m_appState->setConnectionState(currentState);
    QVERIFY(!m_connectionStateChanged);  // Should not emit if unchanged

    bool currentDrivingMode = m_appState->drivingMode();
    m_appState->setDrivingMode(currentDrivingMode);
    QVERIFY(!m_drivingModeChanged);  // Should not emit if unchanged

    bool currentExpertMode = m_appState->expertMode();
    m_appState->setExpertMode(currentExpertMode);
    QVERIFY(!m_expertModeChanged);  // Should not emit if unchanged
}

QTEST_MAIN(TestAppState)
#include "tst_AppStateTests.moc"
