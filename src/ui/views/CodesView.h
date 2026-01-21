#ifndef CODESVIEW_H
#define CODESVIEW_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class AppState;

/**
 * @brief The CodesView class
 * Codes list screen placeholder (to be implemented in Phase 3).
 */
class CodesView : public QWidget
{
    Q_OBJECT

public:
    explicit CodesView(QWidget *parent = nullptr);
    void setAppState(AppState* appState);

private:
    AppState* m_appState = nullptr;
    QLabel* m_placeholderLabel = nullptr;
};

#endif // CODESVIEW_H
