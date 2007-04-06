#include <QVBoxLayout>
#include <QDragEnterEvent>
#include <QDebug>
#include "callstackwidget.h"
#include "callwidget.h"


Call::Call(const QString & channelme)
{
	m_channelme = channelme;
}

Call::Call(const QString & channelme,
	   const QString & action,
	   const int & time,
	   const QString & direction,
	   const QString & channelpeer,
	   const QString & exten,
	   const QString & phonen)
{
	m_channelme   = channelme;
	m_action      = action;
	m_time        = time;
	m_direction   = direction;
	m_channelpeer = channelpeer;
	m_exten       = exten;
	m_phonen      = phonen;
}

Call::Call(const Call & call)
//: QObject(call.parent())
{
	m_channelme   = call.m_channelme;
	m_action      = call.m_action;
	m_time        = call.m_time;
	m_direction   = call.m_direction;
	m_channelpeer = call.m_channelpeer;
	m_exten       = call.m_exten;
	m_phonen      = call.m_phonen;
}

void Call::updateCall(const QString & action,
		      const int & time,
		      const QString & direction,
		      const QString & channelpeer,
		      const QString & exten)
{
	m_action      = action;
	m_time        = time;
	m_direction   = direction;
	m_channelpeer = channelpeer;
	m_exten       = exten;
}

int Call::updateTime()
{
	m_time ++;
	return 0;
}




CallStackWidget::CallStackWidget(QWidget * parent)
: QWidget(parent)
{
	m_layout = new QVBoxLayout(this);
	//m_layout->setMargin();
	//m_layout->setSpacing(0);
	setAcceptDrops(true);
	m_layout->addStretch(1);
}

void CallStackWidget::addCall(const QString & channelme,
                              const QString & action,
			      const int & time,
			      const QString & direction,
			      const QString & channelpeer,
			      const QString & exten,
			      const QString & phonen)
{
	int found = 0;
	// qDebug() << "CallStackWidget::addCall" << channelme << action << time << direction << channelpeer << exten;

	for(int i = 0; i < m_calllist.count() ; i++) {
		if(channelme == m_calllist[i].getChannelMe()) {
			found = 1;
			if(action == QString("Hangup")) {
				m_calllist.removeAt(i);
			} else {
				m_calllist[i].updateCall(action, time,
							 direction, channelpeer, exten);
			}
		}
	}

	if((found == 0) && (action != QString("Hangup"))) {
		Call call(channelme, action, time, direction, channelpeer, exten, phonen);
		m_calllist.append(call);
	}
}

void CallStackWidget::hupchan(const QString & hangupchan)
{
	hangUp(hangupchan);
}

void CallStackWidget::showCalls(const QString & tomonitor, const QString & callerid)
{
	int i, j;
	// cleaning the calling list displayed
	/*
	for(int i = 0; i < m_afflist.count() ; i++) {
		m_layout->removeWidget(m_afflist[i]);
		delete m_afflist[i];
	}
	m_afflist.clear();
	*/

	CallWidget * callwidget = NULL;
	// building the new calling list
	//CallWidget * callwidget = new CallWidget(callerid, this);
	//m_layout->addWidget(callwidget, 0, Qt::AlignTop);
	//m_afflist.append(callwidget);
	
	for(j = m_afflist.count() - 1; j>= 0; j--)
	{
		for(i = 0; i < m_calllist.count(); i++)
		{
			if(m_afflist[j]->channel() == m_calllist[i].getChannelMe())
				break;
		}
		if(i == m_calllist.count())
		{
			m_layout->removeWidget(m_afflist[j]);
			delete m_afflist.takeAt(j);
		}
	}

	for(i = 0; i < m_calllist.count() ; i++) {
		if(tomonitor == m_calllist[i].getPhone()) {
			Call c = m_calllist[i];
			for(j = 0; j < m_afflist.count(); j++)
			{
				qDebug() << j << m_afflist[j]->channel();
				if(m_afflist[j]->channel() == c.getChannelMe())
				{
					m_afflist[j]->updateWidget( c.getAction(),
					                            c.getTime(),
											    c.getDirection(),
											    c.getChannelPeer(),
											    c.getExten() );
					break;
				}
			}
			if(j == m_afflist.count())
			{
			callwidget = new CallWidget(c.getChannelMe(),
								 c.getAction(),
								 c.getTime(),
								 c.getDirection(),
								 c.getChannelPeer(),
								 c.getExten(),
								 this);
			m_afflist.append(callwidget);
			//m_layout->addWidget(callwidget, 0, Qt::AlignTop);
			m_layout->insertWidget(m_layout->count() - 1, callwidget,
			                       0, Qt::AlignTop);
			}
		}
	}
/*	
	callwidget = new CallWidget(this);
	m_layout->addWidget(callwidget, 1, Qt::AlignTop);
	m_afflist.append(callwidget);
*/
}

int CallStackWidget::updateTime()
{
	int n = m_calllist.count();
	for(int i = 0; i < n ; i++) {
		m_calllist[i].updateTime();
	}
	return n;
}

void CallStackWidget::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("text/plain"))
		event->acceptProposedAction();
}

void CallStackWidget::dropEvent(QDropEvent *event)
{
	qDebug() << "CallStackWidget::dropEvent()";
	qDebug() << "   " << event->mimeData()->text();
	selectForMonitoring(event->mimeData()->text());
}

