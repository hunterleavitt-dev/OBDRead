#include <QtTest/QtTest>
#include "core/ReadinessParser.h"
#include "core/dto/ReadinessResult.h"

class TestReadinessParser : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testEmptyResponse();
    void testErrorResponse();
    void testInvalidResponse();
    void testValidResponseAllComplete();
    void testValidResponseAllIncomplete();
    void testValidResponseMixed();
    void testPartialResponse();

private:
    ReadinessParser* m_parser = nullptr;
};

void TestReadinessParser::initTestCase()
{
    m_parser = new ReadinessParser(this);
}

void TestReadinessParser::cleanupTestCase()
{
    // m_parser is deleted automatically as child of this
}

void TestReadinessParser::testEmptyResponse()
{
    QByteArray emptyResponse = "NODATA";
    ReadinessResult result = m_parser->parseReadinessResponse(emptyResponse);
    
    QVERIFY(result.monitors.isEmpty());
    QVERIFY(!result.overallReady);
}

void TestReadinessParser::testErrorResponse()
{
    QByteArray errorResponse = "ERROR";
    ReadinessResult result = m_parser->parseReadinessResponse(errorResponse);
    
    QVERIFY(result.monitors.isEmpty());
    QVERIFY(!result.overallReady);
}

void TestReadinessParser::testInvalidResponse()
{
    QByteArray invalidResponse = "43 01 33 00"; // Mode 03 response, not Mode 01
    ReadinessResult result = m_parser->parseReadinessResponse(invalidResponse);
    
    QVERIFY(result.monitors.isEmpty());
    QVERIFY(!result.overallReady);
}

void TestReadinessParser::testValidResponseAllComplete()
{
    // Response with all monitors available and complete
    // Per SAE J1979: availability bit = 1 means supported, completeness bit = 0 means complete
    //
    // 41: Service Mode 01 response
    // 01: PID 01 (Monitor status since DTCs cleared)
    // 00 (A): A7=0 (MIL off), A6-A0=0 (0 DTCs)
    // 07 (B): 0000 0111
    //         B0-B2 = 111: Common monitors available (MIS, FUEL, CCM)
    //         B3 = 0: Spark ignition engine
    //         B4-B6 = 000: Common monitors complete
    // 67 (C): 0110 0111
    //         C0=1 (CAT avail), C1=1 (HCAT avail), C2=1 (EVAP avail)
    //         C5=1 (O2S avail), C6=1 (O2SH avail)
    // 00 (D): 0000 0000
    //         All engine-specific monitors complete (0 = complete)
    
    QByteArray response = "41 01 00 07 67 00";
    ReadinessResult result = m_parser->parseReadinessResponse(response);
    
    QVERIFY(!result.monitors.isEmpty());
    QVERIFY(result.overallReady);
    
    // Check that all monitors are Complete
    QCOMPARE(result.getMonitorStatus("MIS"), MonitorStatus::Complete);
    QCOMPARE(result.getMonitorStatus("FUEL"), MonitorStatus::Complete);
    QCOMPARE(result.getMonitorStatus("CCM"), MonitorStatus::Complete);
    QCOMPARE(result.getMonitorStatus("CAT"), MonitorStatus::Complete);
    QCOMPARE(result.getMonitorStatus("HCAT"), MonitorStatus::Complete);
    QCOMPARE(result.getMonitorStatus("EVAP"), MonitorStatus::Complete);
    QCOMPARE(result.getMonitorStatus("O2S"), MonitorStatus::Complete);
    QCOMPARE(result.getMonitorStatus("O2SH"), MonitorStatus::Complete);
}

void TestReadinessParser::testValidResponseAllIncomplete()
{
    // Response with all monitors available but incomplete
    // Per SAE J1979: availability bit = 1 means supported, completeness bit = 1 means incomplete
    //
    // 41: Service Mode 01 response
    // 01: PID 01
    // 00 (A): MIL off, 0 DTCs
    // 77 (B): 0111 0111
    //         B0-B2 = 111: Common monitors available
    //         B3 = 0: Spark ignition
    //         B4-B6 = 111: Common monitors incomplete (1 = incomplete)
    // 67 (C): 0110 0111
    //         Engine-specific monitors available (CAT, HCAT, EVAP, O2S, O2SH)
    // 67 (D): 0110 0111
    //         Engine-specific monitors incomplete (1 = incomplete)
    
    QByteArray response = "41 01 00 77 67 67";
    ReadinessResult result = m_parser->parseReadinessResponse(response);
    
    QVERIFY(!result.monitors.isEmpty());
    QVERIFY(!result.overallReady);
    
    // Check that all monitors are Incomplete
    QCOMPARE(result.getMonitorStatus("MIS"), MonitorStatus::Incomplete);
    QCOMPARE(result.getMonitorStatus("FUEL"), MonitorStatus::Incomplete);
    QCOMPARE(result.getMonitorStatus("CCM"), MonitorStatus::Incomplete);
    QCOMPARE(result.getMonitorStatus("CAT"), MonitorStatus::Incomplete);
    QCOMPARE(result.getMonitorStatus("HCAT"), MonitorStatus::Incomplete);
    QCOMPARE(result.getMonitorStatus("EVAP"), MonitorStatus::Incomplete);
    QCOMPARE(result.getMonitorStatus("O2S"), MonitorStatus::Incomplete);
    QCOMPARE(result.getMonitorStatus("O2SH"), MonitorStatus::Incomplete);
}

void TestReadinessParser::testValidResponseMixed()
{
    // Response with mixed monitor status
    // Per SAE J1979: availability bit = 1 means supported, completeness bit = 0 means complete
    //
    // 41: Service Mode 01 response
    // 01: PID 01
    // 00 (A): MIL off, 0 DTCs
    // 67 (B): 0110 0111
    //         B0=1 (MIS avail), B1=1 (FUEL avail), B2=1 (CCM avail)
    //         B4=0 (MIS complete), B5=1 (FUEL incomplete), B6=1 (CCM incomplete)
    // 60 (C): 0110 0000
    //         C5=1 (O2S avail), C6=1 (O2SH avail)
    //         C0-C2=0 (CAT, HCAT, EVAP not available)
    // 00 (D): 0000 0000
    //         D5=0 (O2S complete), D6=0 (O2SH complete)
    
    QByteArray response = "41 01 00 67 60 00";
    ReadinessResult result = m_parser->parseReadinessResponse(response);
    
    QVERIFY(!result.monitors.isEmpty());
    QVERIFY(!result.overallReady); // Not all complete (FUEL and CCM are incomplete)
    
    // Check specific monitors
    QCOMPARE(result.getMonitorStatus("MIS"), MonitorStatus::Complete);
    QCOMPARE(result.getMonitorStatus("FUEL"), MonitorStatus::Incomplete);
    QCOMPARE(result.getMonitorStatus("CCM"), MonitorStatus::Incomplete);
    QCOMPARE(result.getMonitorStatus("O2S"), MonitorStatus::Complete);
    QCOMPARE(result.getMonitorStatus("O2SH"), MonitorStatus::Complete);
}

void TestReadinessParser::testPartialResponse()
{
    // Response with prompt character (as would come from ELM327)
    // All monitors available and complete, with trailing '>' prompt
    //
    // 07 (B): Common monitors available (B0-B2=111), complete (B4-B6=000)
    // 67 (C): Engine-specific monitors available (CAT, HCAT, EVAP, O2S, O2SH)
    // 00 (D): All engine-specific monitors complete
    
    QByteArray response = "41 01 00 07 67 00 >";
    ReadinessResult result = m_parser->parseReadinessResponse(response);
    
    QVERIFY(!result.monitors.isEmpty());
    QVERIFY(result.overallReady);
}

QTEST_MAIN(TestReadinessParser)
#include "tst_ReadinessParser.moc"
