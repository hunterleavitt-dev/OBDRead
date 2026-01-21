#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QWidget>
#include <QLabel>

class AppState;

/**
 * @brief The StatusBar class
 * Persistent status bar component showing connection state and system information.
 */
class StatusBar : public QWidget
{
    Q_OBJECT

public:
    explicit StatusBar(QWidget *parent = nullptr);
    ~StatusBar() = default;

    void setAppState(AppState* appState);

private slots:
    void onConnectionStateChanged();
    void onDrivingModeChanged(bool driving);

private:
    void updateConnectionDisplay();
    void updateStreamIndicator();

    AppState* m_appState = nullptr;
    QLabel* m_connectionLabel = nullptr;
    QLabel* m_protocolLabel = nullptr;
    QLabel* m_voltageLabel = nullptr;
    QLabel* m_streamLabel = nullptr;
    QLabel* m_modeLabel = nullptr;
};

#endif // STATUSBAR_H
