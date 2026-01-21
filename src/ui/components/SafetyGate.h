#ifndef SAFETYGATE_H
#define SAFETYGATE_H

#include <QString>
#include <QWidget>

class AppState;

/**
 * @brief The SafetyGate class
 * Utility class for safety gating of dangerous actions.
 * Provides methods to check if actions are allowed based on vehicle state.
 */
class SafetyGate
{
public:
    /**
     * @brief RequireParked
     * Checks if vehicle is in Parked Mode. Shows message if in Driving Mode.
     * @param parent Parent widget for message box
     * @param appState Application state to check
     * @return true if in Parked Mode, false if in Driving Mode
     */
    static bool RequireParked(QWidget* parent, AppState* appState);

    /**
     * @brief ConfirmDangerousAction
     * Shows a confirmation dialog for dangerous actions.
     * @param parent Parent widget for dialog
     * @param actionName Name of the action (e.g., "Clear Codes")
     * @param warning Warning message to display
     * @return true if user confirmed, false if cancelled
     */
    static bool ConfirmDangerousAction(QWidget* parent, const QString& actionName, const QString& warning);

private:
    SafetyGate() = delete;  // Static class, prevent instantiation
};

#endif // SAFETYGATE_H
