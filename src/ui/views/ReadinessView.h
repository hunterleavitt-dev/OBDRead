#ifndef READINESSVIEW_H
#define READINESSVIEW_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class AppState;

/**
 * @brief The ReadinessView class
 * Readiness screen placeholder (to be implemented in Phase 6).
 */
class ReadinessView : public QWidget
{
    Q_OBJECT

public:
    explicit ReadinessView(QWidget *parent = nullptr);
    void setAppState(AppState* appState);

private:
    AppState* m_appState = nullptr;
    QLabel* m_placeholderLabel = nullptr;
};

#endif // READINESSVIEW_H
