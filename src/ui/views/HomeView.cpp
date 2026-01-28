#include "HomeView.h"
#include "ui/state/AppState.h"
#include "core/ScanService.h"
#include "core/dto/ConnectionState.h"
#include "core/dto/ScanResult.h"
#include "core/dto/DtcEntry.h"
#include "hardware/ObdTransporter.h"
#include <QDebug>

HomeView::HomeView(QWidget *parent)
    : QWidget(parent)
    , m_appState(nullptr)
    , m_scanService(nullptr)
    , m_transporter(nullptr)
{
    setupUI();
}

void HomeView::setAppState(AppState* appState)
{
    if (m_appState) {
        disconnect(m_appState, &AppState::connectionStateChanged, this, &HomeView::onConnectionStateChanged);
        disconnect(m_appState, &AppState::lastScanResultChanged, this, &HomeView::onScanResultChanged);
    }

    m_appState = appState;

    if (m_appState) {
        connect(m_appState, &AppState::connectionStateChanged, this, &HomeView::onConnectionStateChanged);
        connect(m_appState, &AppState::lastScanResultChanged, this, &HomeView::onScanResultChanged);
        onConnectionStateChanged();
        onScanResultChanged();
    }
}

void HomeView::setScanService(ScanService* scanService)
{
    if (m_scanService) {
        disconnect(m_scanService, &ScanService::scanProgress, this, &HomeView::onScanProgress);
    }

    m_scanService = scanService;

    if (m_scanService) {
        connect(m_scanService, &ScanService::scanProgress, this, &HomeView::onScanProgress);
    }
}

void HomeView::setTransporter(ObdTransporter* transporter)
{
    m_transporter = transporter;
}

void HomeView::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Connection controls section
    QGroupBox* connectionGroup = new QGroupBox("Connection", this);
    QVBoxLayout* connectionLayout = new QVBoxLayout(connectionGroup);

    // Address input
    QHBoxLayout* addressLayout = new QHBoxLayout();
    addressLayout->addWidget(new QLabel("Address:", this));
    m_addressInput = new QLineEdit(this);
    m_addressInput->setPlaceholderText("127.0.0.1:35000 (TCP) or MAC address (BLE)");
    m_addressInput->setText("127.0.0.1:35000"); // Default for testing
    addressLayout->addWidget(m_addressInput);
    connectionLayout->addLayout(addressLayout);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_connectButton = new QPushButton("Connect", this);
    m_disconnectButton = new QPushButton("Disconnect", this);
    m_scanButton = new QPushButton("Scan", this);
    
    m_connectButton->setMinimumWidth(100);
    m_disconnectButton->setMinimumWidth(100);
    m_scanButton->setMinimumWidth(100);
    
    buttonLayout->addWidget(m_connectButton);
    buttonLayout->addWidget(m_disconnectButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_scanButton);
    connectionLayout->addLayout(buttonLayout);

    // Connection status display
    QHBoxLayout* statusLayout = new QHBoxLayout();
    m_connectionStatusLabel = new QLabel("Status: Disconnected", this);
    m_protocolLabel = new QLabel("Protocol: -", this);
    statusLayout->addWidget(m_connectionStatusLabel);
    statusLayout->addStretch();
    statusLayout->addWidget(m_protocolLabel);
    connectionLayout->addLayout(statusLayout);

    // Progress label
    m_progressLabel = new QLabel("", this);
    m_progressLabel->setStyleSheet("color: blue; font-style: italic;");
    connectionLayout->addWidget(m_progressLabel);

    mainLayout->addWidget(connectionGroup);

    // Summary tiles section
    m_summaryGroup = new QGroupBox("Scan Summary", this);
    QGridLayout* summaryLayout = new QGridLayout(m_summaryGroup);
    summaryLayout->setSpacing(15);

    // MIL tile
    m_milLabel = new QLabel("MIL Status:", this);
    m_milValueLabel = new QLabel("OFF", this);
    m_milValueLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: green;");
    summaryLayout->addWidget(m_milLabel, 0, 0);
    summaryLayout->addWidget(m_milValueLabel, 0, 1);

    // Codes tile
    m_codesLabel = new QLabel("DTCs:", this);
    m_codesValueLabel = new QLabel("0", this);
    m_codesValueLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    summaryLayout->addWidget(m_codesLabel, 0, 2);
    summaryLayout->addWidget(m_codesValueLabel, 0, 3);

    // Readiness tile
    m_readinessLabel = new QLabel("Readiness:", this);
    m_readinessValueLabel = new QLabel("Unknown", this);
    m_readinessValueLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    summaryLayout->addWidget(m_readinessLabel, 1, 0);
    summaryLayout->addWidget(m_readinessValueLabel, 1, 1);

    // Last scan time
    m_lastScanLabel = new QLabel("Last Scan:", this);
    m_lastScanTimeLabel = new QLabel("Never", this);
    m_lastScanTimeLabel->setStyleSheet("font-style: italic;");
    summaryLayout->addWidget(m_lastScanLabel, 1, 2);
    summaryLayout->addWidget(m_lastScanTimeLabel, 1, 3);

    mainLayout->addWidget(m_summaryGroup);

    // Error display
    m_errorLabel = new QLabel("", this);
    m_errorLabel->setStyleSheet("color: red; font-weight: bold; padding: 5px;");
    m_errorLabel->setWordWrap(true);
    m_errorLabel->hide();
    mainLayout->addWidget(m_errorLabel);

    mainLayout->addStretch();

    // Connect button signals
    connect(m_connectButton, &QPushButton::clicked, this, &HomeView::onConnectClicked);
    connect(m_disconnectButton, &QPushButton::clicked, this, &HomeView::onDisconnectClicked);
    connect(m_scanButton, &QPushButton::clicked, this, &HomeView::onScanClicked);

    updateButtonStates();
}

void HomeView::onConnectClicked()
{
    if (!m_transporter) {
        showError("No transporter available");
        return;
    }

    QString address = m_addressInput->text().trimmed();
    if (address.isEmpty()) {
        showError("Please enter a connection address");
        return;
    }

    clearError();
    m_progressLabel->setText("Connecting...");
    m_transporter->connectToDevice(address);
}

void HomeView::onDisconnectClicked()
{
    if (!m_transporter) {
        return;
    }

    if (m_scanService && m_scanService->isScanning()) {
        m_scanService->cancel();
    }

    m_transporter->disconnectFromDevice();
    m_progressLabel->setText("");
    clearError();
}

void HomeView::onScanClicked()
{
    if (!m_scanService) {
        showError("Scan service not available");
        return;
    }

    if (!m_appState) {
        showError("App state not available");
        return;
    }

    ConnectionStateInfo connState = m_appState->connectionState();
    if (connState.state != ConnectionState::ConnectedEcu) {
        showError("Must be connected to ECU to scan");
        return;
    }

    clearError();
    m_progressLabel->setText("Starting scan...");
    m_scanService->startScan();
}

void HomeView::onConnectionStateChanged()
{
    if (!m_appState) {
        return;
    }

    updateConnectionDisplay();
    updateButtonStates();
}

void HomeView::onScanResultChanged()
{
    if (!m_appState) {
        return;
    }

    updateSummaryTiles();
}

void HomeView::onScanProgress(const QString& message)
{
    if (m_progressLabel) {
        m_progressLabel->setText(message);
    }
}

void HomeView::updateConnectionDisplay()
{
    if (!m_appState) {
        return;
    }

    ConnectionStateInfo connState = m_appState->connectionState();
    
    QString statusText;
    QString statusColor;
    
    switch (connState.state) {
        case ConnectionState::Disconnected:
            statusText = "Disconnected";
            statusColor = "black";
            break;
        case ConnectionState::Connecting:
            statusText = "Connecting...";
            statusColor = "blue";
            break;
        case ConnectionState::ConnectedAdapter:
            statusText = "Adapter Connected (No ECU)";
            statusColor = "orange";
            break;
        case ConnectionState::ConnectedEcu:
            statusText = "Connected to ECU";
            statusColor = "green";
            break;
        case ConnectionState::Error:
            statusText = "Error: " + connState.lastError;
            statusColor = "red";
            break;
    }

    m_connectionStatusLabel->setText(QString("Status: <span style='color: %1; font-weight: bold;'>%2</span>")
                                     .arg(statusColor, statusText));

    if (!connState.protocolName.isEmpty()) {
        m_protocolLabel->setText("Protocol: " + connState.protocolName);
    } else {
        m_protocolLabel->setText("Protocol: -");
    }
}

void HomeView::updateSummaryTiles()
{
    if (!m_appState) {
        return;
    }

    ScanResult result = m_appState->lastScanResult();

    // MIL status
    if (result.milOn) {
        m_milValueLabel->setText("ON");
        m_milValueLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: red;");
    } else {
        m_milValueLabel->setText("OFF");
        m_milValueLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: green;");
    }

    // DTC counts
    int confirmedCount = result.getDtcCount(DtcStatus::Confirmed);
    int pendingCount = result.getDtcCount(DtcStatus::Pending);
    int permanentCount = result.getDtcCount(DtcStatus::Permanent);
    int totalCount = result.dtcs.size();

    QString codesText;
    if (totalCount == 0) {
        codesText = "0";
        m_codesValueLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: green;");
    } else {
        codesText = QString::number(totalCount);
        if (confirmedCount > 0 || permanentCount > 0) {
            codesText += QString(" (%1 confirmed").arg(confirmedCount);
            if (permanentCount > 0) {
                codesText += QString(", %1 permanent").arg(permanentCount);
            }
            codesText += ")";
            m_codesValueLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: red;");
        } else if (pendingCount > 0) {
            codesText += QString(" (%1 pending)").arg(pendingCount);
            m_codesValueLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: orange;");
        } else {
            m_codesValueLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
        }
    }
    m_codesValueLabel->setText(codesText);

    // Readiness status
    if (result.readiness.monitors.isEmpty()) {
        m_readinessValueLabel->setText("Unknown");
        m_readinessValueLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: gray;");
    } else if (result.readiness.overallReady) {
        m_readinessValueLabel->setText("Ready");
        m_readinessValueLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: green;");
    } else {
        m_readinessValueLabel->setText("Not Ready");
        m_readinessValueLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: orange;");
    }

    // Last scan time
    if (result.timestamp.isValid()) {
        m_lastScanTimeLabel->setText(result.timestamp.toString("yyyy-MM-dd hh:mm:ss"));
    } else {
        m_lastScanTimeLabel->setText("Never");
    }
}

void HomeView::updateButtonStates()
{
    if (!m_appState || !m_transporter) {
        m_connectButton->setEnabled(false);
        m_disconnectButton->setEnabled(false);
        m_scanButton->setEnabled(false);
        return;
    }

    ConnectionStateInfo connState = m_appState->connectionState();
    bool isConnected = m_transporter->isConnected();
    bool isConnecting = (connState.state == ConnectionState::Connecting);
    bool isScanning = m_scanService && m_scanService->isScanning();

    m_connectButton->setEnabled(!isConnected && !isConnecting);
    m_disconnectButton->setEnabled(isConnected || isConnecting);
    m_scanButton->setEnabled(connState.state == ConnectionState::ConnectedEcu && !isScanning);
}

void HomeView::showError(const QString& message)
{
    if (m_errorLabel) {
        m_errorLabel->setText("Error: " + message);
        m_errorLabel->show();
    }
}

void HomeView::clearError()
{
    if (m_errorLabel) {
        m_errorLabel->hide();
        m_errorLabel->clear();
    }
}
