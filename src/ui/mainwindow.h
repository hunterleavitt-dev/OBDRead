#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QQueue>
#include <QByteArray>
#include <QTabWidget>

#include "hardware/ObdTransporter.h"
#include "ui/state/AppState.h"
#include "ui/components/StatusBar.h"
#include "ui/views/HomeView.h"
#include "ui/views/CodesView.h"
#include "ui/views/LiveDataView.h"
#include "ui/views/ReadinessView.h"
#include "ui/views/AdvancedView.h"
#include "ui/views/LogsView.h"
#include "ui/views/SettingsView.h"

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
    void onTransporterConnected();
    void onTransporterDisconnected();
    void onTransporterError(const QString& errorMsg);

private:
    Ui::MainWindow *ui;
    QQueue<QByteArray> m_commandQueue;
    QByteArray m_incomingBuffer;

    // App state and UI components
    AppState* m_appState = nullptr;
    StatusBar* m_statusBar = nullptr;
    QTabWidget* m_tabWidget = nullptr;
    
    // Views
    HomeView* m_homeView = nullptr;
    CodesView* m_codesView = nullptr;
    LiveDataView* m_liveDataView = nullptr;
    ReadinessView* m_readinessView = nullptr;
    AdvancedView* m_advancedView = nullptr;
    LogsView* m_logsView = nullptr;
    SettingsView* m_settingsView = nullptr;

    void processQueue();
    void setupUI();
    void setupConnections();
    void updateConnectionState();
};

#endif // MAINWINDOW_H
