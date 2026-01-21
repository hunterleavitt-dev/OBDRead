#include <QtTest/QtTest>
#include "DtcParser.h"

class TestDtcParser : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testDecodeDtc_Powertrain();
    void testDecodeDtc_Chassis();
    void testDecodeDtc_Body();
    void testDecodeDtc_Network();

    void testParseDtc_Powertrain();
    void testParseDtc_Chassis();
    void testParseDtc_Body();
    void testParseDtc_Network();

private:
    DtcParser *m_parser;
};

void TestDtcParser::initTestCase()
{
    m_parser = new DtcParser(this);
}

void TestDtcParser::cleanupTestCase()
{
    // m_parser is deleted automatically as child of this
}

// DECODER TESTS
void TestDtcParser::testDecodeDtc_Powertrain()
{
    // P0133 = 01 33 in hex (Powertrain, 0133)
    QString result = DtcParser::decodeDtc(0x01, 0x33);
    QCOMPARE(result, QString("P0133"));
}

void TestDtcParser::testDecodeDtc_Chassis()
{
    // C0500 = 45 00 in hex (Chassis = 01 in upper 2 bits)
    QString result = DtcParser::decodeDtc(0x45, 0x00);
    QCOMPARE(result, QString("C0500"));
}

void TestDtcParser::testDecodeDtc_Body()
{
    // B1234 = 92 34 in hex (Body = 10 in upper 2 bits)
    QString result = DtcParser::decodeDtc(0x92, 0x34);
    QCOMPARE(result, QString("B1234"));
}

void TestDtcParser::testDecodeDtc_Network()
{
    // U0100 = C1 00 in hex (Network = 11 in upper 2 bits)
    QString result = DtcParser::decodeDtc(0xC1, 0x00);
    QCOMPARE(result, QString("U0100"));
}

// PARSER TESTS
void TestDtcParser::testParseDtc_Powertrain()
{
    // TODO
    // P0133 = 01 33 in hex (Powertrain, 0133)
}

void TestDtcParser::testParseDtc_Chassis()
{
    // TODO
    // C0500 = 45 00 in hex (Chassis = 01 in upper 2 bits)
}

void TestDtcParser::testParseDtc_Body()
{
    // TODO
    // B1234 = 92 34 in hex (Body = 10 in upper 2 bits)
}

void TestDtcParser::testParseDtc_Network()
{
    // TODO
    // U0100 = C1 00 in hex (Network = 11 in upper 2 bits)
}


QTEST_MAIN(TestDtcParser)
#include "tst_DtcParser.moc"

