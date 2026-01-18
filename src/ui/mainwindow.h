#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QQueue>
#include <QByteArray>

#include "ObdTransporter.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    ObdTransporter* m_transporter;

private slots:
    void on_initializeButton_clicked();

private:
    Ui::MainWindow *ui;
    QQueue<QByteArray> m_commandQueue; // line of cars waiting
    QByteArray m_incomingBuffer;       // holding pen for data

    void processQueue();               // helper to send the next guy
};
#endif // MAINWINDOW_H
