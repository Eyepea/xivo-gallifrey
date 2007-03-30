#include <QVBoxLayout>
#include <QDebug>
#include "callstackwidget.h"
#include "callwidget.h"

CallStackWidget::CallStackWidget(QWidget * parent)
: QWidget(parent)
{
	m_layout = new QVBoxLayout(this);
	//m_layout->setMargin();
	//m_layout->setSpacing(0);
}

void CallStackWidget::addCall(const QString & callerId,
                              const QString & callerIdName,
							  const QString & channel,
							  const QString & ext)
{
	qDebug() << "CallStackWidget::addCall" << callerId << channel;
	CallWidget * call = new CallWidget(callerId, callerIdName,
	                                   channel, this);
	m_layout->addWidget(call);
	//call->show();
}


