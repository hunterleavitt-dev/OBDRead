#ifndef HOMEVIEW_H
#define HOMEVIEW_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class AppState;

/**
 * @brief The HomeView class
 * Home/Health screen placeholder (to be implemented in Phase 2).
 */
class HomeView : public QWidget
{
    Q_OBJECT

public:
    explicit HomeView(QWidget *parent = nullptr);
    void setAppState(AppState* appState);

private:
    AppState* m_appState = nullptr;
    QLabel* m_placeholderLabel = nullptr;
};

#endif // HOMEVIEW_H
