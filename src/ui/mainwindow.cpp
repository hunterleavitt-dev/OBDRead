#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "hardware/TcpTransporter.h"
#include "core/dto/ConnectionState.h"
#include "core/dto/ScanResult.h"
#include "core/ScanService.h"
#include <QDebug>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // Initialize base UI widgets
    ui->setupUi(this);

    // Create backend objects
    m_appState = new AppState(this); // Create AppState
    m_transporter = new TcpTransporter(this); // Create transporter
    m_scanService = new ScanService(m_transporter, this); // Create scan service

    // Setup UI (tabs, status bar, etc.)
    setupUI();

    // Final wiring
    setupConnections(); // Setup signal connections
    updateConnectionState(); // Initial connection state
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    setWindowTitle("OBD-II Scanner");

    // Create tab widget for navigation
    m_tabWidget = new QTabWidget(this);
    setCentralWidget(m_tabWidget);

    // Create all views
    m_homeView = new HomeView(this);
    m_codesView = new CodesView(this);
    m_liveDataView = new LiveDataView(this);
    m_readinessView = new ReadinessView(this);
    m_advancedView = new AdvancedView(this);
    m_logsView = new LogsView(this);
    m_settingsView = new SettingsView(this);

    // Set AppState on all views
    m_homeView->setAppState(m_appState);
    m_homeView->setScanService(m_scanService);
    m_homeView->setTransporter(m_transporter);
    m_codesView->setAppState(m_appState);
    m_liveDataView->setAppState(m_appState);
    m_readinessView->setAppState(m_appState);
    m_advancedView->setAppState(m_appState);
    m_logsView->setAppState(m_appState);
    m_settingsView->setAppState(m_appState);

    // Add tabs
    m_tabWidget->addTab(m_homeView, "Home/Health");
    m_tabWidget->addTab(m_codesView, "Codes");
    m_tabWidget->addTab(m_liveDataView, "Live Data");
    m_tabWidget->addTab(m_readinessView, "Readiness");
    m_tabWidget->addTab(m_advancedView, "Advanced");
    m_tabWidget->addTab(m_logsView, "Logs");
    m_tabWidget->addTab(m_settingsView, "Settings");

    // Create and add status bar component
    m_statusBar = new StatusBar(this);
    m_statusBar->setAppState(m_appState);
    
    // Add status bar to main window's status bar area
    statusBar()->addPermanentWidget(m_statusBar, 1);
}

void MainWindow::setupConnections()
{
    // Transporter signals
    connect(m_transporter, &ObdTransporter::connected, this, &MainWindow::onTransporterConnected);
    connect(m_transporter, &ObdTransporter::disconnected, this, &MainWindow::onTransporterDisconnected);
    connect(m_transporter, &ObdTransporter::errorOccurred, this, &MainWindow::onTransporterError);

    // ScanService signals
    connect(m_scanService, &ScanService::connectionComplete, this, &MainWindow::onConnectionComplete);
    connect(m_scanService, &ScanService::connectionFailed, this, &MainWindow::onConnectionFailed);
    connect(m_scanService, &ScanService::adapterConnectedNoEcu, this, &MainWindow::onAdapterConnectedNoEcu);
    connect(m_scanService, &ScanService::scanComplete, this, &MainWindow::onScanComplete);
    connect(m_scanService, &ScanService::scanFailed, this, &MainWindow::onScanFailed);
}

void MainWindow::updateConnectionState()
{
    if (!m_appState) {
        return;
    }

    ConnectionStateInfo info = m_appState->connectionState();
    
    if (!m_transporter) {
        info.state = ConnectionState::Disconnected;
    } else if (m_transporter->isConnected()) {
        // State will be updated by scan service connection flow
        // Keep existing protocol name if set
    } else {
        info.state = ConnectionState::Disconnected;
        info.protocolName.clear();
    }

    m_appState->setConnectionState(info);
}

void MainWindow::onTransporterConnected()
{
    qDebug() << "Adapter connected, starting connection sequence...";
    ConnectionStateInfo info;
    info.state = ConnectionState::Connecting;
    m_appState->setConnectionState(info);
    
    // Start connection sequence via ScanService
    if (m_scanService) {
        m_scanService->startConnection();
    }
}

void MainWindow::onTransporterDisconnected()
{
    qDebug() << "Disconnected.";
    ConnectionStateInfo info;
    info.state = ConnectionState::Disconnected;
    info.protocolName.clear();
    m_appState->setConnectionState(info);
}

void MainWindow::onTransporterError(const QString& errorMsg)
{
    qDebug() << "Transporter Error:" << errorMsg;
    ConnectionStateInfo info;
    info.state = ConnectionState::Error;
    info.lastError = errorMsg;
    m_appState->setConnectionState(info);
}

void MainWindow::onConnectionComplete(const QString& protocolName)
{
    qDebug() << "Connection complete, protocol:" << protocolName;
    ConnectionStateInfo info;
    info.state = ConnectionState::ConnectedEcu;
    info.protocolName = protocolName;
    m_appState->setConnectionState(info);
}

void MainWindow::onConnectionFailed(const QString& errorMessage)
{
    qDebug() << "Connection failed:" << errorMessage;
    ConnectionStateInfo info;
    info.state = ConnectionState::Error;
    info.lastError = errorMessage;
    m_appState->setConnectionState(info);
}

void MainWindow::onAdapterConnectedNoEcu()
{
    qDebug() << "Adapter connected but ECU not responding";
    ConnectionStateInfo info;
    info.state = ConnectionState::ConnectedAdapter;
    m_appState->setConnectionState(info);
}

void MainWindow::onScanComplete(const ScanResult& result)
{
    qDebug() << "Scan complete, DTCs found:" << result.dtcs.size();
    m_appState->setLastScanResult(result);
}

void MainWindow::onScanFailed(const QString& errorMessage)
{
    qDebug() << "Scan failed:" << errorMessage;
    // Update connection state to show error, but don't change connection status
    ConnectionStateInfo info = m_appState->connectionState();
    info.lastError = errorMessage;
    m_appState->setConnectionState(info);
}
