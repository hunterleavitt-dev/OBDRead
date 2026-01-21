#ifndef ADVANCEDVIEW_H
#define ADVANCEDVIEW_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class AppState;

/**
 * @brief The AdvancedView class
 * Advanced screen placeholder (to be implemented in Phase 7).
 */
class AdvancedView : public QWidget
{
    Q_OBJECT

public:
    explicit AdvancedView(QWidget *parent = nullptr);
    void setAppState(AppState* appState);

private:
    AppState* m_appState = nullptr;
    QLabel* m_placeholderLabel = nullptr;
};

#endif // ADVANCEDVIEW_H
