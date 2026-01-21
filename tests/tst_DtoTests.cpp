#include <QtTest/QtTest>
#include "core/dto/ConnectionState.h"
#include "core/dto/VehicleProfile.h"
#include "core/dto/DtcEntry.h"
#include "core/dto/FreezeFrame.h"
#include "core/dto/ScanResult.h"
#include "core/dto/ReadinessResult.h"
#include "core/dto/PidMeta.h"
#include "core/dto/PidSample.h"
#include "core/dto/LogMeta.h"
#include "core/dto/LogData.h"

class TestDtos : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // ConnectionState tests
    void testConnectionStateEnum();
    void testConnectionStateInfo();

    // VehicleProfile tests
    void testVehicleProfileCreation();
    void testVehicleProfileValidation();

    // DtcEntry tests
    void testDtcEntryCreation();
    void testDtcEntryStatusTypes();
    void testDtcEntryCategoryDerivation();

    // FreezeFrame tests
    void testFreezeFrame();

    // ScanResult tests
    void testScanResultCreation();
    void testScanResultDtcCount();

    // ReadinessResult tests
    void testReadinessResult();
    void testReadinessResultOverallReady();

    // PidMeta tests
    void testPidMeta();

    // PidSample tests
    void testPidSample();

    // LogMeta tests
    void testLogMeta();

    // LogData tests
    void testLogData();
};

void TestDtos::initTestCase()
{
}

void TestDtos::cleanupTestCase()
{
}

// ConnectionState tests
void TestDtos::testConnectionStateEnum()
{
    ConnectionState state1 = ConnectionState::Disconnected;
    ConnectionState state2 = ConnectionState::ConnectedEcu;
    
    QVERIFY(state1 != state2);
    QVERIFY(state1 == ConnectionState::Disconnected);
}

void TestDtos::testConnectionStateInfo()
{
    ConnectionStateInfo info1;
    info1.state = ConnectionState::ConnectedEcu;
    info1.protocolName = "ISO 9141-2";
    info1.voltage = 12.6;
    info1.ignitionState = true;

    ConnectionStateInfo info2 = info1;
    QVERIFY(info1 == info2);

    info2.voltage = 12.7;
    QVERIFY(info1 != info2);
}

// VehicleProfile tests
void TestDtos::testVehicleProfileCreation()
{
    VehicleProfile profile1;
    QVERIFY(!profile1.isValid());

    VehicleProfile profile2(2020, "Toyota", "Camry");
    QVERIFY(profile2.isValid());
    QCOMPARE(profile2.year, 2020);
    QCOMPARE(profile2.make, QString("Toyota"));
    QCOMPARE(profile2.model, QString("Camry"));
}

void TestDtos::testVehicleProfileValidation()
{
    VehicleProfile profile;
    profile.year = 2020;
    QVERIFY(!profile.isValid());  // Missing make/model

    profile.make = "Toyota";
    QVERIFY(!profile.isValid());  // Missing model

    profile.model = "Camry";
    QVERIFY(profile.isValid());
}

// DtcEntry tests
void TestDtos::testDtcEntryCreation()
{
    DtcEntry entry("P0420");
    QVERIFY(entry.isValid());
    QCOMPARE(entry.code, QString("P0420"));
    QCOMPARE(entry.status, DtcStatus::Confirmed);
    QCOMPARE(entry.category, DtcCategory::P);
}

void TestDtos::testDtcEntryStatusTypes()
{
    DtcEntry entry1("P0420", DtcStatus::Confirmed);
    QCOMPARE(entry1.status, DtcStatus::Confirmed);

    DtcEntry entry2("P0420", DtcStatus::Pending);
    QCOMPARE(entry2.status, DtcStatus::Pending);

    DtcEntry entry3("P0420", DtcStatus::Permanent);
    QCOMPARE(entry3.status, DtcStatus::Permanent);
}

void TestDtos::testDtcEntryCategoryDerivation()
{
    DtcEntry entryP("P0420");
    QCOMPARE(entryP.category, DtcCategory::P);

    DtcEntry entryB("B1234");
    QCOMPARE(entryB.category, DtcCategory::B);

    DtcEntry entryC("C0500");
    QCOMPARE(entryC.category, DtcCategory::C);

    DtcEntry entryU("U0100");
    QCOMPARE(entryU.category, DtcCategory::U);
}

// FreezeFrame tests
void TestDtos::testFreezeFrame()
{
    FreezeFrame frame(1);
    QVERIFY(frame.isEmpty());

    frame.addParameter("RPM", "2500 rpm");
    frame.addParameter("Speed", "65 mph");
    QVERIFY(!frame.isEmpty());
    QCOMPARE(frame.parameters.size(), 2);
    QCOMPARE(frame.parameters["RPM"], QString("2500 rpm"));
}

// ScanResult tests
void TestDtos::testScanResultCreation()
{
    ScanResult result;
    QVERIFY(result.isEmpty());
    QVERIFY(result.timestamp.isValid());
    QVERIFY(!result.milOn);
    QVERIFY(result.dtcs.isEmpty());
}

void TestDtos::testScanResultDtcCount()
{
    ScanResult result;
    result.milOn = true;
    
    result.dtcs.append(DtcEntry("P0420", DtcStatus::Confirmed));
    result.dtcs.append(DtcEntry("P0133", DtcStatus::Confirmed));
    result.dtcs.append(DtcEntry("P0171", DtcStatus::Pending));

    QCOMPARE(result.getDtcCount(DtcStatus::Confirmed), 2);
    QCOMPARE(result.getDtcCount(DtcStatus::Pending), 1);
    QCOMPARE(result.getDtcCount(DtcStatus::Permanent), 0);
}

// ReadinessResult tests
void TestDtos::testReadinessResult()
{
    ReadinessResult result;
    QVERIFY(!result.overallReady);

    result.setMonitorStatus("MISFIRE", MonitorStatus::Complete);
    result.setMonitorStatus("FUEL", MonitorStatus::Complete);
    QVERIFY(result.overallReady);

    result.setMonitorStatus("CATALYST", MonitorStatus::Incomplete);
    QVERIFY(!result.overallReady);
}

void TestDtos::testReadinessResultOverallReady()
{
    ReadinessResult result;
    
    result.setMonitorStatus("MISFIRE", MonitorStatus::Complete);
    result.setMonitorStatus("FUEL", MonitorStatus::Complete);
    result.setMonitorStatus("CATALYST", MonitorStatus::Unsupported);
    
    QVERIFY(result.overallReady);  // Unsupported monitors don't affect readiness
}

// PidMeta tests
void TestDtos::testPidMeta()
{
    PidMeta meta("0100", "PIDs supported", "");
    QVERIFY(meta.isValid());
    QCOMPARE(meta.pidId, QString("0100"));
    QCOMPARE(meta.name, QString("PIDs supported"));
    QVERIFY(!meta.supported);
}

// PidSample tests
void TestDtos::testPidSample()
{
    PidSample sample("0105", 2000.0, "rpm");
    QVERIFY(sample.isValid());
    QCOMPARE(sample.pidId, QString("0105"));
    QCOMPARE(sample.value, 2000.0);
    QCOMPARE(sample.unit, QString("rpm"));
    QVERIFY(sample.timestamp.isValid());
}

// LogMeta tests
void TestDtos::testLogMeta()
{
    LogMeta meta;
    QVERIFY(!meta.id.isEmpty());
    QVERIFY(meta.timestamp.isValid());
    QVERIFY(!meta.isValid());  // duration and pidCount not set

    meta.duration = 60;
    meta.pidCount = 5;
    meta.sampleRate = 10.0;
    QVERIFY(meta.isValid());
}

// LogData tests
void TestDtos::testLogData()
{
    LogMeta meta;
    meta.duration = 60;
    meta.pidCount = 2;
    
    LogData log(meta);
    QVERIFY(log.isEmpty());
    QCOMPARE(log.getSampleCount(), 0);

    log.samples.append(PidSample("0105", 2000.0, "rpm"));
    log.samples.append(PidSample("010C", 65.0, "rpm"));
    QVERIFY(!log.isEmpty());
    QCOMPARE(log.getSampleCount(), 2);
}

QTEST_MAIN(TestDtos)
#include "tst_DtoTests.moc"
