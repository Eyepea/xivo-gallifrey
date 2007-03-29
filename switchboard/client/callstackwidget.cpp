#include <QVBoxLayout>
#include "callstackwidget.h"
#include "callwidget.h"

CallStackWidget::CallStackWidget(QWidget * parent)
: QWidget(parent)
{
	m_layout = new QVBoxLayout(this);
	//m_layout->setMargin();
	m_layout->setSpacing(0);
}

void CallStackWidget::addCall(const QString & test)
{
	CallWidget * call = new CallWidget(this);
	m_layout->addWidget(call);
}


