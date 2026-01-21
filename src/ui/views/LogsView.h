#ifndef LOGSVIEW_H
#define LOGSVIEW_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class AppState;

/**
 * @brief The LogsView class
 * Logs screen placeholder (to be implemented in Phase 5).
 */
class LogsView : public QWidget
{
    Q_OBJECT

public:
    explicit LogsView(QWidget *parent = nullptr);
    void setAppState(AppState* appState);

private:
    AppState* m_appState = nullptr;
    QLabel* m_placeholderLabel = nullptr;
};

#endif // LOGSVIEW_H
