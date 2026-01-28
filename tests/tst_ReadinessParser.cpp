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
    // Mode 01 PID 01 response with all monitors complete
    // Byte 2: 0x00 (MIL off, 0 DTCs)
    // Byte 3: 0xFF (all monitors complete)
    // Byte 4: 0xFF (all monitors complete)
    QByteArray response = "41 01 00 FF FF";
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
    // Mode 01 PID 01 response with all monitors incomplete
    // Byte 2: 0x00 (MIL off, 0 DTCs)
    // Byte 3: 0x00 (all monitors incomplete)
    // Byte 4: 0x00 (all monitors incomplete)
    QByteArray response = "41 01 00 00 00";
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
    // Mode 01 PID 01 response with mixed status
    // Byte 2: 0x00 (MIL off, 0 DTCs)
    // Byte 3: 0x01 (only MIS complete)
    // Byte 4: 0x03 (O2S and O2SH complete)
    QByteArray response = "41 01 00 01 03";
    ReadinessResult result = m_parser->parseReadinessResponse(response);
    
    QVERIFY(!result.monitors.isEmpty());
    QVERIFY(!result.overallReady); // Not all complete
    
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
    QByteArray response = "41 01 00 FF FF >";
    ReadinessResult result = m_parser->parseReadinessResponse(response);
    
    QVERIFY(!result.monitors.isEmpty());
    QVERIFY(result.overallReady);
}

QTEST_MAIN(TestReadinessParser)
#include "tst_ReadinessParser.moc"
