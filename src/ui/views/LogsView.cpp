#include "LogsView.h"
#include "ui/state/AppState.h"
#include <QVBoxLayout>

LogsView::LogsView(QWidget *parent)
    : QWidget(parent)
    , m_appState(nullptr)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    m_placeholderLabel = new QLabel("Logs - Coming in Phase 5", this);
    m_placeholderLabel->setAlignment(Qt::AlignCenter);
    m_placeholderLabel->setStyleSheet("font-size: 16px; color: gray;");
    layout->addWidget(m_placeholderLabel);
}

void LogsView::setAppState(AppState* appState)
{
    m_appState = appState;
    // Will be used in Phase 5 for log management and playback
}
