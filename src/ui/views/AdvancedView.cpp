#include "AdvancedView.h"
#include "ui/state/AppState.h"
#include <QVBoxLayout>

AdvancedView::AdvancedView(QWidget *parent)
    : QWidget(parent)
    , m_appState(nullptr)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    m_placeholderLabel = new QLabel("Advanced - Coming in Phase 7", this);
    m_placeholderLabel->setAlignment(Qt::AlignCenter);
    m_placeholderLabel->setStyleSheet("font-size: 16px; color: gray;");
    layout->addWidget(m_placeholderLabel);
}

void AdvancedView::setAppState(AppState* appState)
{
    m_appState = appState;
    // Will be used in Phase 7 for Mode $06 and expert features
}
