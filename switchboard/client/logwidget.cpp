/* $Id$ */
#include <QVBoxLayout>
#include <QLayoutItem>
#include <QLabel>
#include <QDebug>
#include "logwidget.h"
#include "logeltwidget.h"
#include "switchboardengine.h"

LogWidget::LogWidget(SwitchBoardEngine * engine, QWidget * parent)
: QWidget(parent), m_engine(engine), m_timer(-1)
{
	m_layout = new QVBoxLayout(this);
	m_layout->setSpacing(2);
	m_layout->addStretch(1);
}

void LogWidget::addElement(const QString & peer, LogEltWidget::Direction d,
                           const QDateTime & dt, int duration)
{
	//qDebug() << "LogWidget::addElement()" << peer << d << dt << duration;
	int i, index = 0;
	for(i = 0; i<m_layout->count(); i++)
	{
		QWidget * widget = m_layout->itemAt( i )->widget();
		if(widget)
		{
			LogEltWidget * logelt = qobject_cast<LogEltWidget *>(widget);
			if(logelt)
			{
				if(dt == logelt->dateTime()
				   && peer == logelt->peer()
				   && d == logelt->direction())
					return;
				else if(dt >= logelt->dateTime())
					break;
				index = i + 1;
			}
		}
	}
	LogEltWidget * e = new LogEltWidget(peer, d, dt, duration, this);
	connect( e, SIGNAL(emitDial(const QString &)),
	         m_engine, SLOT(dial(const QString &)) );
	m_layout->insertWidget(index, e);
}

void LogWidget::clear()
{
	QLayoutItem * child;
	while ((child = m_layout->itemAt(0)) != 0)
	{
		if(child->widget())
		{
			m_layout->removeItem(child);
			delete child->widget();
			delete child;
		}
		else
			break;
	}
	//m_layout->addStretch(1);
}

void LogWidget::addLogEntry(const QDateTime & dt, int duration,
                            const QString & peer, int d)
{
	// TODO: manage the list !
	addElement(peer, (LogEltWidget::Direction)d, dt, duration);
}

void LogWidget::setPeerToDisplay(const QString & peer)
{
	clear();
	m_peer = peer;
	askHistory(m_peer);
	if(m_timer<0)
		m_timer = startTimer(3000);
}

void LogWidget::timerEvent(QTimerEvent * event)
{
	//qDebug() << "LogWidget::timerEvent() id=" << event->timerId();
	askHistory(m_peer);
}

