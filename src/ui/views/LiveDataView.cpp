#include "LiveDataView.h"
#include "ui/state/AppState.h"
#include <QVBoxLayout>

LiveDataView::LiveDataView(QWidget *parent)
    : QWidget(parent)
    , m_appState(nullptr)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    m_placeholderLabel = new QLabel("Live Data - Coming in Phase 4", this);
    m_placeholderLabel->setAlignment(Qt::AlignCenter);
    m_placeholderLabel->setStyleSheet("font-size: 16px; color: gray;");
    layout->addWidget(m_placeholderLabel);
}

void LiveDataView::setAppState(AppState* appState)
{
    if (m_appState == appState) {
        return;
    }

    if (m_appState) {
        disconnect(m_appState, nullptr, this, nullptr);
    }

    m_appState = appState;

    if (m_appState) {
        connect(m_appState, &AppState::drivingModeChanged, this, &LiveDataView::onDrivingModeChanged);
        onDrivingModeChanged(m_appState->drivingMode());
    }
}

void LiveDataView::onDrivingModeChanged(bool driving)
{
    Q_UNUSED(driving);
    // In Phase 4, this will disable editing controls when in Driving Mode
}
