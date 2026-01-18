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
}

MainWindow::~MainWindow()
{
    delete ui;
}
