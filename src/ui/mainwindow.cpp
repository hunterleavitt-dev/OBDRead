#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "TcpTransporter.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // when testing:
    m_transporter = new TcpTransporter(this);

    // when using hardware:
    // m_transporter = new BleTransporter(this);

    // -------------------------------------------

    // 1. connect signals to see what happens
    connect(m_transporter, &ObdTransporter::connected, [](){
        qDebug() << "SYSTEM STATUS: Active";
    });
    connect(m_transporter, &ObdTransporter::dataReceived, [](QByteArray data){
        qDebug() << "RESPONSE FROM EMULATOR:" << data;
    });

    // 2. connect to localhost
    m_transporter->connectToDevice("127.0.0.1:35000");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_testButton_clicked()
{
    // 3. check if connected first
    if (!m_transporter->isConnected()) {
        qDebug() << "Connecting to emulator...";
        m_transporter->connectToDevice("127.0.0.1:35000");
        return; // wait for connection before sending
    }

    // 4. send the command
    qDebug() << "Sending AT Z...";
    m_transporter->sendCommand("AT Z\r");
}

