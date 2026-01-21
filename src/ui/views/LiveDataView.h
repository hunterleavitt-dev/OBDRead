#ifndef LIVEDATAVIEW_H
#define LIVEDATAVIEW_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class AppState;

/**
 * @brief The LiveDataView class
 * Live Data screen placeholder (to be implemented in Phase 4).
 */
class LiveDataView : public QWidget
{
    Q_OBJECT

public:
    explicit LiveDataView(QWidget *parent = nullptr);
    void setAppState(AppState* appState);

private slots:
    void onDrivingModeChanged(bool driving);

private:
    AppState* m_appState = nullptr;
    QLabel* m_placeholderLabel = nullptr;
};

#endif // LIVEDATAVIEW_H
