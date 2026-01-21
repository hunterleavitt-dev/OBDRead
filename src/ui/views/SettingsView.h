#ifndef SETTINGSVIEW_H
#define SETTINGSVIEW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLabel>

class AppState;

/**
 * @brief The SettingsView class
 * Settings screen with mode toggles and other settings.
 */
class SettingsView : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsView(QWidget *parent = nullptr);
    void setAppState(AppState* appState);

private slots:
    void onDrivingModeToggled(bool checked);
    void onExpertModeToggled(bool checked);
    void onDrivingModeChanged(bool driving);
    void onExpertModeChanged(bool expert);

private:
    AppState* m_appState = nullptr;
    QCheckBox* m_drivingModeCheckbox = nullptr;
    QCheckBox* m_expertModeCheckbox = nullptr;
    QLabel* m_otherSettingsLabel = nullptr;
};

#endif // SETTINGSVIEW_H
