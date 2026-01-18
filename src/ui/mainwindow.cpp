#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "TcpTransporter.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1. start with button disabled so user can't click too early
    ui->initializeButton->setEnabled(false);

    // when testing:
    m_transporter = new TcpTransporter(this);

    // when using hardware:
    // m_transporter = new BleTransporter(this);

    // ------------------------------------------- \\

    // 2. enable button only when connection succeeds
    connect(m_transporter, &ObdTransporter::connected, this, [this](){
        qDebug() << "Connected! System Ready.";
        ui->initializeButton->setEnabled(true);
    });

    connect(m_transporter, &ObdTransporter::dataReceived, this, [this](QByteArray data){
        // 1. append new data to buffer (!TCP packets can be fragmented!)
        m_incomingBuffer.append(data);

        // 2. check if buffer contains 'prompt' character (>)
        if (m_incomingBuffer.contains('>')) {

            // found a prompt, the previous command is finished.
            qDebug() << "Full Response:" << m_incomingBuffer;

            // clear buffer for the next transaction
            // m_incomingBuffer.clear(); // TODO: DEPRECATE

            int endOfPrompt = m_incomingBuffer.indexOf('>') + 1;
            m_incomingBuffer.remove(0, endOfPrompt);

            // trigger the next command
            processQueue();
        }
    });

    m_transporter->connectToDevice("127.0.0.1:35000");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_initializeButton_clicked()
{
    // queue up the initialization sequence
    m_commandQueue.enqueue("AT Z\r");
    m_commandQueue.enqueue("AT E0\r"); // echo off
    m_commandQueue.enqueue("AT SP 0\r"); // CRITICAL: tell it to find the protocol
    m_commandQueue.enqueue("01 00\r");   // ping the ECU to ensure connection
    m_commandQueue.enqueue("AT L0\r"); // linefeeds off
    m_commandQueue.enqueue("03\r");    // ask for codes

    // kickstart the process
    processQueue();
}

void MainWindow::processQueue()
{
    // if there is nothing waiting, stop.
    if (m_commandQueue.isEmpty()) return;

    // send the next command in line
    QByteArray nextCmd = m_commandQueue.dequeue();
    qDebug() << "Sending:" << nextCmd;
    m_transporter->sendCommand(nextCmd);
}

