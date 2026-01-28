#include "ScanService.h"
#include "DtcParser.h"
#include "ReadinessParser.h"
#include "core/dto/ScanResult.h"
#include "core/dto/DtcEntry.h"
#include <QDebug>

ScanService::ScanService(ObdTransporter* transporter, QObject *parent)
    : QObject(parent)
    , m_transporter(transporter)
    , m_dtcParser(new DtcParser(this))
    , m_readinessParser(new ReadinessParser(this))
    , m_state(Idle)
    , m_currentOperation(CmdConnection)
    , m_milStatusReceived(false)
    , m_storedDtcReceived(false)
    , m_pendingDtcReceived(false)
    , m_readinessReceived(false)
    , m_ecuResponded(false)
    , m_timeoutTimer(new QTimer(this))
{
    m_timeoutTimer->setSingleShot(true);
    connect(m_timeoutTimer, &QTimer::timeout, this, &ScanService::onTimeout);
    
    if (m_transporter) {
        connect(m_transporter, &ObdTransporter::dataReceived, this, &ScanService::onDataReceived);
    }
}

ScanService::~ScanService()
{
}

void ScanService::startConnection()
{
    if (m_state != Idle) {
        qDebug() << "ScanService: Cannot start connection, already busy";
        return;
    }

    reset();
    m_state = Connecting;
    m_currentOperation = CmdConnection;
    m_ecuResponded = false;
    m_protocolName.clear();

    // Build connection sequence
    m_commandQueue.enqueue({QByteArray("AT Z\r"), "Reset adapter", CmdConnection});
    m_commandQueue.enqueue({QByteArray("AT E0\r"), "Echo off", CmdConnection});
    m_commandQueue.enqueue({QByteArray("AT SP 0\r"), "Auto-detect protocol", CmdConnection});
    m_commandQueue.enqueue({QByteArray("01 00\r"), "Ping ECU", CmdConnection});
    m_commandQueue.enqueue({QByteArray("AT DP\r"), "Get protocol name", CmdConnection});

    emit scanProgress("Connecting to adapter...");
    processNextCommand();
}

void ScanService::startScan()
{
    if (m_state != Idle) {
        qDebug() << "ScanService: Cannot start scan, already busy";
        return;
    }

    reset();
    m_state = Scanning;
    m_currentOperation = CmdScan;
    m_currentScanResult = ScanResult();

    // Build scan sequence
    m_commandQueue.enqueue({QByteArray("01 01\r"), "Get MIL status", CmdScan});
    m_commandQueue.enqueue({QByteArray("03\r"), "Get stored DTCs", CmdScan});
    m_commandQueue.enqueue({QByteArray("07\r"), "Get pending DTCs", CmdScan});
    m_commandQueue.enqueue({QByteArray("01 01\r"), "Get readiness monitors", CmdScan});

    emit scanProgress("Starting scan...");
    processNextCommand();
}

void ScanService::cancel()
{
    if (m_state == Idle) {
        return;
    }

    m_timeoutTimer->stop();
    m_commandQueue.clear();
    m_responseBuffer.clear();
    m_state = Idle;
    emit scanProgress("Cancelled");
}

void ScanService::onDataReceived(const QByteArray& data)
{
    if (m_state == Idle) {
        return; // Not expecting data
    }

    m_responseBuffer.append(data);

    // Check if we have a complete response (ends with '>')
    if (m_responseBuffer.contains('>')) {
        // Extract response up to and including the prompt
        int promptIndex = m_responseBuffer.indexOf('>');
        QByteArray response = m_responseBuffer.left(promptIndex);
        m_responseBuffer.remove(0, promptIndex + 1);

        qDebug() << "ScanService: Received response:" << response;

        // Process the response
        if (m_currentOperation == CmdConnection) {
            handleConnectionResponse(response);
        } else if (m_currentOperation == CmdScan) {
            handleScanResponse(response);
        }

        // Stop timeout timer
        m_timeoutTimer->stop();

        // Process next command
        processNextCommand();
    }
}

void ScanService::onTimeout()
{
    qDebug() << "ScanService: Timeout waiting for response";
    
    if (m_currentOperation == CmdConnection) {
        // Check if we got adapter connection but no ECU response
        if (m_transporter && m_transporter->isConnected() && !m_ecuResponded) {
            m_state = Idle;
            emit adapterConnectedNoEcu();
        } else {
            m_state = Idle;
            emit connectionFailed("Connection timeout");
        }
    } else if (m_currentOperation == CmdScan) {
        // Partial scan results are acceptable
        qDebug() << "ScanService: Scan timeout, finishing with partial results";
        finishScan();
    }

    reset();
}

void ScanService::processNextCommand()
{
    if (m_commandQueue.isEmpty()) {
        // All commands processed
        if (m_currentOperation == CmdConnection) {
            if (m_ecuResponded) {
                m_state = Idle;
                emit connectionComplete(m_protocolName.isEmpty() ? "Auto" : m_protocolName);
            } else {
                m_state = Idle;
                emit adapterConnectedNoEcu();
            }
        } else if (m_currentOperation == CmdScan) {
            finishScan();
        }
        return;
    }

    Command cmd = m_commandQueue.dequeue();
    m_responseBuffer.clear();

    if (m_transporter && m_transporter->isConnected()) {
        qDebug() << "ScanService: Sending" << cmd.description << ":" << cmd.data;
        m_transporter->sendCommand(cmd.data);
        
        // Start timeout timer
        m_timeoutTimer->start(TIMEOUT_MS);
    } else {
        qDebug() << "ScanService: Cannot send command, not connected";
        if (m_currentOperation == CmdConnection) {
            m_state = Idle;
            emit connectionFailed("Not connected to adapter");
        } else {
            m_state = Idle;
            emit scanFailed("Not connected to adapter");
        }
        reset();
    }
}

void ScanService::handleConnectionResponse(const QByteArray& response)
{
    QByteArray clean = response.simplified();
    
    // Check for ECU ping response (01 00 response should be "41 00 XX ...")
    if (clean.contains("41 00") || clean.contains("4100")) {
        m_ecuResponded = true;
        emit scanProgress("ECU responding");
    }
    
    // Check for protocol name (AT DP response)
    if (clean.contains("AT DP") || clean.toUpper().contains("PROTOCOL")) {
        parseProtocolName(response);
    }
}

void ScanService::handleScanResponse(const QByteArray& response)
{
    QByteArray clean = response.simplified();
    
    // Determine which command this response is for based on response content
    // This is a bit fragile, but we process in order
    
    if (clean.contains("41 01") || clean.contains("4101")) {
        // MIL status response
        if (!m_milStatusReceived) {
            parseMilStatus(response);
            m_milStatusReceived = true;
            emit scanProgress("MIL status received");
        }
    } else if (clean.contains("43") && !m_storedDtcReceived) {
        // Mode 03 response (stored DTCs)
        parseDtcResponse(response, DtcStatus::Confirmed);
        m_storedDtcReceived = true;
        emit scanProgress("Stored DTCs received");
    } else if (clean.contains("47") && !m_pendingDtcReceived) {
        // Mode 07 response (pending DTCs)
        parseDtcResponse(response, DtcStatus::Pending);
        m_pendingDtcReceived = true;
        emit scanProgress("Pending DTCs received");
    } else if (clean.contains("41 01") && m_milStatusReceived && !m_readinessReceived) {
        // Second 01 01 response is for readiness
        parseReadinessResponse(response);
        m_readinessReceived = true;
        emit scanProgress("Readiness monitors received");
    }
}

void ScanService::parseProtocolName(const QByteArray& response)
{
    // AT DP response format varies, but typically contains protocol name
    // Examples: "AUTO, ISO 9141-2" or "ISO 9141-2" or "CAN 11/500"
    QString responseStr = QString::fromLatin1(response).simplified();
    
    // Try to extract protocol name
    // Look for common protocol identifiers
    if (responseStr.contains("ISO 9141", Qt::CaseInsensitive)) {
        m_protocolName = "ISO 9141-2";
    } else if (responseStr.contains("ISO 14230", Qt::CaseInsensitive)) {
        m_protocolName = "ISO 14230-4";
    } else if (responseStr.contains("CAN", Qt::CaseInsensitive)) {
        if (responseStr.contains("11", Qt::CaseInsensitive)) {
            m_protocolName = "CAN 11/500";
        } else if (responseStr.contains("29", Qt::CaseInsensitive)) {
            m_protocolName = "CAN 29/500";
        } else {
            m_protocolName = "CAN";
        }
    } else if (responseStr.contains("J1850", Qt::CaseInsensitive)) {
        m_protocolName = "J1850";
    } else if (!responseStr.isEmpty() && !responseStr.contains("OK") && !responseStr.contains("ELM")) {
        // Try to use the response as-is if it looks like a protocol name
        m_protocolName = responseStr;
    }
    
    if (!m_protocolName.isEmpty()) {
        qDebug() << "ScanService: Detected protocol:" << m_protocolName;
    }
}

void ScanService::parseMilStatus(const QByteArray& response)
{
    // Mode 01 PID 01 response: "41 01 A B C D"
    // Byte A: bit 7 = MIL status, bits 0-6 = DTC count
    
    QByteArray clean = response.simplified();
    clean.replace(">", "");
    clean.replace(" ", "");
    
    QByteArray bytes = QByteArray::fromHex(clean);
    
    if (bytes.size() >= 3 && bytes.at(0) == 0x41 && bytes.at(1) == 0x01) {
        quint8 byteA = bytes.at(2);
        m_currentScanResult.milOn = (byteA & 0x80) != 0; // Bit 7
        // DTC count is in bits 0-6, but we'll get actual DTCs from Mode 03/07
    }
}

void ScanService::parseDtcResponse(const QByteArray& response, DtcStatus status)
{
    QStringList dtcCodes;
    
    // Determine mode from response
    int mode = (status == DtcStatus::Pending) ? 7 : 3;
    dtcCodes = m_dtcParser->parseDtcResponse(response, mode);
    
    // Convert to DtcEntry objects
    for (const QString& code : dtcCodes) {
        DtcEntry entry(code, status);
        m_currentScanResult.dtcs.append(entry);
    }
}

void ScanService::parseReadinessResponse(const QByteArray& response)
{
    ReadinessResult readiness = m_readinessParser->parseReadinessResponse(response);
    m_currentScanResult.readiness = readiness;
}

void ScanService::finishScan()
{
    m_state = Idle;
    m_currentScanResult.timestamp = QDateTime::currentDateTime();
    emit scanComplete(m_currentScanResult);
    reset();
}

void ScanService::reset()
{
    m_commandQueue.clear();
    m_responseBuffer.clear();
    m_milStatusReceived = false;
    m_storedDtcReceived = false;
    m_pendingDtcReceived = false;
    m_readinessReceived = false;
    m_currentScanResult = ScanResult();
}
