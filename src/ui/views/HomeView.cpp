#include "HomeView.h"
#include "ui/state/AppState.h"
#include <QVBoxLayout>

HomeView::HomeView(QWidget *parent)
    : QWidget(parent)
    , m_appState(nullptr)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    m_placeholderLabel = new QLabel("Home/Health - Coming in Phase 2", this);
    m_placeholderLabel->setAlignment(Qt::AlignCenter);
    m_placeholderLabel->setStyleSheet("font-size: 16px; color: gray;");
    layout->addWidget(m_placeholderLabel);
}

void HomeView::setAppState(AppState* appState)
{
    m_appState = appState;
    // Will be used in Phase 2 to display scan results
}
