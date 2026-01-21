#include "SafetyGate.h"
#include "ui/state/AppState.h"
#include <QMessageBox>

bool SafetyGate::RequireParked(QWidget* parent, AppState* appState)
{
    if (!appState) {
        return false;
    }

    if (appState->drivingMode()) {
        QMessageBox::warning(
            parent,
            "Action Not Available",
            "This action is not available while driving. Please switch to Parked Mode."
        );
        return false;
    }

    return true;
}

bool SafetyGate::ConfirmDangerousAction(QWidget* parent, const QString& actionName, const QString& warning)
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        parent,
        QString("Confirm %1").arg(actionName),
        QString("%1\n\n%2\n\nDo you want to continue?").arg(actionName, warning),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );

    return reply == QMessageBox::Yes;
}
