#include "CodesView.h"
#include "ui/state/AppState.h"
#include <QVBoxLayout>

CodesView::CodesView(QWidget *parent)
    : QWidget(parent)
    , m_appState(nullptr)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    m_placeholderLabel = new QLabel("Codes - Coming in Phase 3", this);
    m_placeholderLabel->setAlignment(Qt::AlignCenter);
    m_placeholderLabel->setStyleSheet("font-size: 16px; color: gray;");
    layout->addWidget(m_placeholderLabel);
}

void CodesView::setAppState(AppState* appState)
{
    m_appState = appState;
    // Will be used in Phase 3 to display DTC list
}
