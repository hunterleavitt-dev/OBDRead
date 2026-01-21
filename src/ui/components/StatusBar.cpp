#include "StatusBar.h"
#include "ui/state/AppState.h"
#include "core/dto/ConnectionState.h"
#include <QHBoxLayout>
#include <QFrame>

StatusBar::StatusBar(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 2, 5, 2);
    layout->setSpacing(10);

    // Connection status
    m_connectionLabel = new QLabel("Disconnected", this);
    m_connectionLabel->setStyleSheet("font-weight: bold; color: gray;");
    layout->addWidget(m_connectionLabel);

    // Separator
    QFrame* separator1 = new QFrame(this);
    separator1->setFrameShape(QFrame::VLine);
    separator1->setFrameShadow(QFrame::Sunken);
    layout->addWidget(separator1);

    // Protocol
    m_protocolLabel = new QLabel("", this);
    layout->addWidget(m_protocolLabel);

    // Voltage
    m_voltageLabel = new QLabel("", this);
    layout->addWidget(m_voltageLabel);

    // Separator
    QFrame* separator2 = new QFrame(this);
    separator2->setFrameShape(QFrame::VLine);
    separator2->setFrameShadow(QFrame::Sunken);
    layout->addWidget(separator2);

    // Stream indicator (stub)
    m_streamLabel = new QLabel("", this);
    layout->addWidget(m_streamLabel);

    // Separator
    QFrame* separator3 = new QFrame(this);
    separator3->setFrameShape(QFrame::VLine);
    separator3->setFrameShadow(QFrame::Sunken);
    layout->addWidget(separator3);

    // Mode indicator
    m_modeLabel = new QLabel("Parked Mode", this);
    m_modeLabel->setStyleSheet("color: green;");
    layout->addWidget(m_modeLabel);

    layout->addStretch();
}

void StatusBar::setAppState(AppState* appState)
{
    if (m_appState == appState) {
        return;
    }

    // Disconnect old signals
    if (m_appState) {
        disconnect(m_appState, nullptr, this, nullptr);
    }

    m_appState = appState;

    // Connect new signals
    if (m_appState) {
        connect(m_appState, &AppState::connectionStateChanged, this, &StatusBar::onConnectionStateChanged);
        connect(m_appState, &AppState::drivingModeChanged, this, &StatusBar::onDrivingModeChanged);
        onConnectionStateChanged();
        onDrivingModeChanged(m_appState->drivingMode());
    }
}

void StatusBar::onConnectionStateChanged()
{
    if (!m_appState) {
        return;
    }

    updateConnectionDisplay();
}

void StatusBar::onDrivingModeChanged(bool driving)
{
    Q_UNUSED(driving);
    if (!m_appState) {
        return;
    }

    if (m_appState->drivingMode()) {
        m_modeLabel->setText("Driving Mode");
        m_modeLabel->setStyleSheet("color: orange; font-weight: bold;");
    } else {
        m_modeLabel->setText("Parked Mode");
        m_modeLabel->setStyleSheet("color: green;");
    }
}

void StatusBar::updateConnectionDisplay()
{
    if (!m_appState) {
        m_connectionLabel->setText("Disconnected");
        m_connectionLabel->setStyleSheet("font-weight: bold; color: gray;");
        m_protocolLabel->setText("");
        m_voltageLabel->setText("");
        return;
    }

    const ConnectionStateInfo& info = m_appState->connectionState();

    switch (info.state) {
    case ConnectionState::Disconnected:
        m_connectionLabel->setText("Disconnected");
        m_connectionLabel->setStyleSheet("font-weight: bold; color: gray;");
        m_protocolLabel->setText("");
        m_voltageLabel->setText("");
        break;

    case ConnectionState::Connecting:
        m_connectionLabel->setText("Connecting...");
        m_connectionLabel->setStyleSheet("font-weight: bold; color: blue;");
        m_protocolLabel->setText("");
        m_voltageLabel->setText("");
        break;

    case ConnectionState::ConnectedAdapter:
        m_connectionLabel->setText("Adapter Connected");
        m_connectionLabel->setStyleSheet("font-weight: bold; color: orange;");
        m_protocolLabel->setText("");
        m_voltageLabel->setText("");
        break;

    case ConnectionState::ConnectedEcu:
        m_connectionLabel->setText("ECU Connected");
        m_connectionLabel->setStyleSheet("font-weight: bold; color: green;");
        if (!info.protocolName.isEmpty()) {
            m_protocolLabel->setText(QString("Protocol: %1").arg(info.protocolName));
        } else {
            m_protocolLabel->setText("");
        }
        if (info.voltage > 0.0) {
            m_voltageLabel->setText(QString("Voltage: %1V").arg(info.voltage, 0, 'f', 1));
        } else {
            m_voltageLabel->setText("");
        }
        break;

    case ConnectionState::Error:
        m_connectionLabel->setText(QString("Error: %1").arg(info.lastError.isEmpty() ? "Unknown" : info.lastError));
        m_connectionLabel->setStyleSheet("font-weight: bold; color: red;");
        m_protocolLabel->setText("");
        m_voltageLabel->setText("");
        break;
    }

    updateStreamIndicator();
}

void StatusBar::updateStreamIndicator()
{
    // Stub implementation - will be implemented in Phase 4
    m_streamLabel->setText("");
}
