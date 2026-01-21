#include "SettingsView.h"
#include "ui/state/AppState.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFormLayout>

SettingsView::SettingsView(QWidget *parent)
    : QWidget(parent)
    , m_appState(nullptr)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);

    // Mode Settings Group
    QGroupBox* modeGroup = new QGroupBox("Operational Mode", this);
    QFormLayout* modeLayout = new QFormLayout(modeGroup);

    m_drivingModeCheckbox = new QCheckBox("Driving Mode (read-only)", this);
    m_drivingModeCheckbox->setToolTip("When enabled, editing and dangerous actions are disabled");
    connect(m_drivingModeCheckbox, &QCheckBox::toggled, this, &SettingsView::onDrivingModeToggled);
    modeLayout->addRow(m_drivingModeCheckbox);

    mainLayout->addWidget(modeGroup);

    // Expert Settings Group
    QGroupBox* expertGroup = new QGroupBox("Expert Settings", this);
    QFormLayout* expertLayout = new QFormLayout(expertGroup);

    m_expertModeCheckbox = new QCheckBox("Expert Mode", this);
    m_expertModeCheckbox->setToolTip("Show additional technical details and raw data");
    connect(m_expertModeCheckbox, &QCheckBox::toggled, this, &SettingsView::onExpertModeToggled);
    expertLayout->addRow(m_expertModeCheckbox);

    mainLayout->addWidget(expertGroup);

    // Other Settings Placeholder
    QGroupBox* otherGroup = new QGroupBox("Other Settings", this);
    QVBoxLayout* otherLayout = new QVBoxLayout(otherGroup);

    m_otherSettingsLabel = new QLabel("Additional settings will be added in future development phases", this);
    m_otherSettingsLabel->setStyleSheet("color: gray;");
    otherLayout->addWidget(m_otherSettingsLabel);

    mainLayout->addWidget(otherGroup);

    mainLayout->addStretch();
}

void SettingsView::setAppState(AppState* appState)
{
    if (m_appState == appState) {
        return;
    }

    if (m_appState) {
        disconnect(m_appState, nullptr, this, nullptr);
    }

    m_appState = appState;

    if (m_appState) {
        connect(m_appState, &AppState::drivingModeChanged, this, &SettingsView::onDrivingModeChanged);
        connect(m_appState, &AppState::expertModeChanged, this, &SettingsView::onExpertModeChanged);
        onDrivingModeChanged(m_appState->drivingMode());
        onExpertModeChanged(m_appState->expertMode());
    }
}

void SettingsView::onDrivingModeToggled(bool checked)
{
    if (m_appState) {
        m_appState->setDrivingMode(checked);
    }
}

void SettingsView::onExpertModeToggled(bool checked)
{
    if (m_appState) {
        m_appState->setExpertMode(checked);
    }
}

void SettingsView::onDrivingModeChanged(bool driving)
{
    if (m_drivingModeCheckbox) {
        m_drivingModeCheckbox->blockSignals(true);
        m_drivingModeCheckbox->setChecked(driving);
        m_drivingModeCheckbox->blockSignals(false);
    }
}

void SettingsView::onExpertModeChanged(bool expert)
{
    if (m_expertModeCheckbox) {
        m_expertModeCheckbox->blockSignals(true);
        m_expertModeCheckbox->setChecked(expert);
        m_expertModeCheckbox->blockSignals(false);
    }
}
