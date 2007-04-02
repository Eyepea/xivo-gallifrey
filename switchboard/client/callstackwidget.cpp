#include <QVBoxLayout>
#include <QDebug>
#include "callstackwidget.h"
#include "callwidget.h"


Call::Call(const QString & ext)
{
	m_ext = ext;
}

Call::Call(const Call & call)
//: QObject(call.parent())
{
	m_ext = call.m_ext;
	m_callwidget = call.m_callwidget;
}




CallStackWidget::CallStackWidget(QWidget * parent)
: QWidget(parent)
{
	m_layout = new QVBoxLayout(this);
	//m_layout->setMargin();
	//m_layout->setSpacing(0);
}

void CallStackWidget::addCall(const QString & channelme,
                              const QString & action,
			      const QString & time,
			      const QString & direction,
			      const QString & channelpeer,
			      const QString & exten)
{
	int found = 0;
	// qDebug() << "CallStackWidget::addCall" << channelme << action << time << direction << channelpeer << exten;

	for(int i = 0; i < m_calllist.count() ; i++) {
		if(channelme == m_calllist[i].ext()) {
			found = 1;
			if(action == QString("Hangup")) {
				m_layout->removeWidget( m_calllist[i].getWidget() );
				delete m_calllist[i].getWidget();
				m_calllist.removeAt(i);
			} else {
				m_calllist[i].getWidget()->updateWidget(action, time,
									direction, channelpeer, exten);
			}
		}
	}

	if(found == 0) {
		Call call(channelme);
		CallWidget * callwidget = new CallWidget(channelme, action, time,
							 direction, channelpeer,
							 exten, this);
		m_layout->addWidget(callwidget, 0, Qt::AlignTop);

		call.setWidget(callwidget);
		m_calllist.append(call);
	}
}

