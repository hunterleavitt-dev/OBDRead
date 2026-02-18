#include <QtTest/QtTest>
#include "core/ScanService.h"
#include "hardware/ObdTransporter.h"
#include "core/dto/ScanResult.h"
#include "core/dto/DtcEntry.h"
#include <QSignalSpy>
#include <QTimer>
#include <qtestcase.h>

// Mock transporter for testing
class MockTransporter : public ObdTransporter
{
    Q_OBJECT

public:
    MockTransporter(QObject* parent = nullptr) : ObdTransporter(parent), m_connected(false) {}
    
    void connectToDevice(const QString &identifier) override {
        Q_UNUSED(identifier);
        m_connected = true;
        emit connected();
    }
    
    void disconnectFromDevice() override {
        m_connected = false;
        emit disconnected();
    }
    
    void sendCommand(const QByteArray &cmd) override {
        m_lastCommand = cmd;
        // Determine response based on command
        QByteArray response;
        if (cmd.contains("01 01")) {
            response = "41 01 80 00 00 >"; // MIL on, no readiness
        } else if (cmd.contains("03")) {
            response = "43 01 33 00 00 >"; // P0133
        } else if (cmd.contains("07")) {
            response = "47 00 00 00 00 >"; // No pending DTCs
        } else if (cmd.contains("AT DP")) {
            response = "ISO 9141-2 >";
        } else if (cmd.contains("01 00")) {
            response = "41 00 BE 1F A8 13 >"; // ECU responds
        } else {
            response = "OK >";
        }
        // Defer emission to the next event-loop iteration so that the
        // caller (ScanService::processNextCommand) returns before the
        // response arrives, matching real async hardware behaviour.
        QTimer::singleShot(0, this, [this, response]() {
            emit dataReceived(response);
        });
    }
    
    bool isConnected() const override {
        return m_connected;
    }
    
    QByteArray lastCommand() const { return m_lastCommand; }

private:
    bool m_connected;
    QByteArray m_lastCommand;
};

class TestScanService : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testInitialState();
    void testConnectionSequence();
    void testScanSequence();
    void testCancel();

private:
    MockTransporter* m_transporter = nullptr;
    ScanService* m_scanService = nullptr;
};

void TestScanService::initTestCase()
{
    m_transporter = new MockTransporter(this);
    m_scanService = new ScanService(m_transporter, this);
}

void TestScanService::cleanupTestCase()
{
    // Objects are deleted automatically as children
}

void TestScanService::testInitialState()
{
    QVERIFY(m_scanService != nullptr);
    QVERIFY(!m_scanService->isScanning());
    QVERIFY(!m_scanService->isConnecting());
}

void TestScanService::testConnectionSequence()
{
    QSignalSpy connectionCompleteSpy(m_scanService, &ScanService::connectionComplete);
    QSignalSpy connectionFailedSpy(m_scanService, &ScanService::connectionFailed);
    
    // Connect transporter first
    m_transporter->connectToDevice("127.0.0.1:35000");
    
    // Start connection sequence
    m_scanService->startConnection();
    
    // Mock responses are delivered asynchronously, so the state must still
    // be Connecting at this point.
    QVERIFY(m_scanService->isConnecting());
    
    // Spin the event loop until the full command-response chain completes
    QTRY_VERIFY_WITH_TIMEOUT(!m_scanService->isConnecting(), 1000);
    
    // Verify the connection completed successfully
    QCOMPARE(connectionCompleteSpy.count(), 1);
    QCOMPARE(connectionFailedSpy.count(), 0);
}

void TestScanService::testScanSequence()
{
    // Ensure transporter is connected
    m_transporter->connectToDevice("127.0.0.1:35000");
    
    QSignalSpy scanCompleteSpy(m_scanService, &ScanService::scanComplete);
    QSignalSpy scanFailedSpy(m_scanService, &ScanService::scanFailed);
    
    // Start scan
    m_scanService->startScan();
    
    QVERIFY(m_scanService->isScanning());
    
    // Spin the event loop until the scan finishes
    QTRY_VERIFY_WITH_TIMEOUT(!m_scanService->isScanning(), 1000);
    
    // Verify the scan completed successfully
    QCOMPARE(scanCompleteSpy.count(), 1);
    QCOMPARE(scanFailedSpy.count(), 0);
}

void TestScanService::testCancel()
{
    m_scanService->startScan();
    QVERIFY(m_scanService->isScanning());
    
    m_scanService->cancel();
    QVERIFY(!m_scanService->isScanning());
}

QTEST_MAIN(TestScanService)
#include "tst_ScanService.moc"
