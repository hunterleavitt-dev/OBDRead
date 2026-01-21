#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "hardware/TcpTransporter.h"
#include "core/dto/ConnectionState.h"
#include <QDebug>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create AppState
    m_appState = new AppState(this);

    // Setup UI (tabs, status bar, etc.)
    setupUI();

    // Create transporter
    m_transporter = new TcpTransporter(this);

    // Setup signal connections
    setupConnections();

    // Initial connection state
    updateConnectionState();

    // Connect to emulator (for testing)
    m_transporter->connectToDevice("127.0.0.1:35000");
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
    connect(m_transporter, &ObdTransporter::dataReceived, this, [this](QByteArray data){
        // Append new data to buffer
        m_incomingBuffer.append(data);

        // Check if buffer contains 'prompt' character (>)
        if (m_incomingBuffer.contains('>')) {
            qDebug() << "Full Response:" << m_incomingBuffer;

            int endOfPrompt = m_incomingBuffer.indexOf('>') + 1;
            m_incomingBuffer.remove(0, endOfPrompt);

            // Trigger the next command
            processQueue();
        }
    });
}

void MainWindow::updateConnectionState()
{
    if (!m_appState) {
        return;
    }

    ConnectionStateInfo info;
    
    if (!m_transporter) {
        info.state = ConnectionState::Disconnected;
    } else if (m_transporter->isConnected()) {
        info.state = ConnectionState::ConnectedEcu;  // Will be refined in Phase 2
        info.protocolName = "Auto";  // Placeholder
    } else {
        info.state = ConnectionState::Disconnected;
    }

    m_appState->setConnectionState(info);
}

void MainWindow::onTransporterConnected()
{
    qDebug() << "Connected! System Ready.";
    updateConnectionState();
}

void MainWindow::onTransporterDisconnected()
{
    qDebug() << "Disconnected.";
    updateConnectionState();
}

void MainWindow::onTransporterError(const QString& errorMsg)
{
    qDebug() << "Error:" << errorMsg;
    ConnectionStateInfo info;
    info.state = ConnectionState::Error;
    info.lastError = errorMsg;
    m_appState->setConnectionState(info);
}

void MainWindow::on_initializeButton_clicked()
{
    // This button is kept for backward compatibility but will be replaced in Phase 2
    // Queue up the initialization sequence
    m_commandQueue.enqueue("AT Z\r");
    m_commandQueue.enqueue("AT E0\r"); // echo off
    m_commandQueue.enqueue("AT SP 0\r"); // CRITICAL: tell it to find the protocol
    m_commandQueue.enqueue("01 00\r");   // ping the ECU to ensure connection
    m_commandQueue.enqueue("AT L0\r"); // linefeeds off
    m_commandQueue.enqueue("03\r");    // ask for codes

    // Kickstart the process
    processQueue();
}

void MainWindow::processQueue()
{
    // If there is nothing waiting, stop.
    if (m_commandQueue.isEmpty()) return;

    // Send the next command in line
    QByteArray nextCmd = m_commandQueue.dequeue();
    qDebug() << "Sending:" << nextCmd;
    m_transporter->sendCommand(nextCmd);
}
