#include "ReadinessView.h"
#include "ui/state/AppState.h"
#include <QVBoxLayout>

ReadinessView::ReadinessView(QWidget *parent)
    : QWidget(parent)
    , m_appState(nullptr)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    m_placeholderLabel = new QLabel("Readiness - Coming in Phase 6", this);
    m_placeholderLabel->setAlignment(Qt::AlignCenter);
    m_placeholderLabel->setStyleSheet("font-size: 16px; color: gray;");
    layout->addWidget(m_placeholderLabel);
}

void ReadinessView::setAppState(AppState* appState)
{
    m_appState = appState;
    // Will be used in Phase 6 to display readiness results
}
