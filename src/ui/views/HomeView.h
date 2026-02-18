#ifndef HOMEVIEW_H
#define HOMEVIEW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLineEdit>

class AppState;
class ScanService;
class ObdTransporter;
class ConnectionStateInfo;
class ScanResult;

/**
 * @brief The HomeView class
 * Home/Health screen with connection controls and scan summary.
 */
class HomeView : public QWidget
{
    Q_OBJECT

public:
    explicit HomeView(QWidget *parent = nullptr);
    void setAppState(AppState* appState);
    void setScanService(ScanService* scanService);
    void setTransporter(ObdTransporter* transporter);

private slots:
    void onConnectClicked();
    void onDisconnectClicked();
    void onScanClicked();
    void onConnectionStateChanged();
    void onScanResultChanged();
    void onScanProgress(const QString& message);

private:
    void setupUI();
    void updateConnectionDisplay();
    void updateSummaryTiles();
    void updateButtonStates();
    void showError(const QString& message);
    void clearError();

    AppState* m_appState = nullptr;
    ScanService* m_scanService = nullptr;
    ObdTransporter* m_transporter = nullptr;

    // UI Components
    QPushButton* m_connectButton = nullptr;
    QPushButton* m_disconnectButton = nullptr;
    QPushButton* m_scanButton = nullptr;
    QLineEdit* m_addressInput = nullptr;
    
    QLabel* m_connectionStatusLabel = nullptr;
    QLabel* m_protocolLabel = nullptr;
    
    // Summary tiles
    QGroupBox* m_summaryGroup = nullptr;
    QLabel* m_milLabel = nullptr;
    QLabel* m_milValueLabel = nullptr;
    QLabel* m_codesLabel = nullptr;
    QLabel* m_codesValueLabel = nullptr;
    QLabel* m_readinessLabel = nullptr;
    QLabel* m_readinessValueLabel = nullptr;
    QLabel* m_lastScanLabel = nullptr;
    QLabel* m_lastScanTimeLabel = nullptr;
    
    // Error display
    QLabel* m_errorLabel = nullptr;
    QLabel* m_progressLabel = nullptr;
};

#endif // HOMEVIEW_H
