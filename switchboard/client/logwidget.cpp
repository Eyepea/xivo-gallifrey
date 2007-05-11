/* $Id$ */
#include <QVBoxLayout>
#include <QLayoutItem>
#include <QLabel>
#include <QGroupBox>
#include <QRadioButton>
#include <QDebug>
#include "logwidget.h"
#include "logeltwidget.h"
#include "switchboardengine.h"

LogWidget::LogWidget(SwitchBoardEngine * engine, QWidget * parent)
: QWidget(parent), m_engine(engine), m_timer(-1)
{
	m_layout = new QVBoxLayout(this);
	m_layout->setSpacing(2);
	QGroupBox * groupBox = new QGroupBox( tr("Call history"), this );
	QVBoxLayout * vbox = new QVBoxLayout( groupBox );
	m_radioOut = new QRadioButton( tr("&Out calls"), groupBox );
	m_radioOut->setChecked( true );
	connect( m_radioOut, SIGNAL(toggled(bool)),
	         this, SLOT(modeChanged(bool)) );
	vbox->addWidget( m_radioOut );
	m_radioIn = new QRadioButton( tr("&In calls"), groupBox );
	connect( m_radioIn, SIGNAL(toggled(bool)),
	         this, SLOT(modeChanged(bool)) );
	vbox->addWidget( m_radioIn );
	m_radioMissed = new QRadioButton( tr("&Missed calls"), groupBox );
	connect( m_radioMissed, SIGNAL(toggled(bool)),
	         this, SLOT(modeChanged(bool)) );
	vbox->addWidget( m_radioMissed );
	m_layout->addWidget( groupBox );
	m_layout->addStretch(1);
}

void LogWidget::addElement(const QString & peer, LogEltWidget::Direction d,
                           const QDateTime & dt, int duration)
{
	//qDebug() << "LogWidget::addElement()" << peer << d << dt << duration;
	int i, index = 1;
	for(i = 1; i<m_layout->count(); i++)
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
	while ((child = m_layout->itemAt(1)) != 0)
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
	if(m_peer.size() > 0)
	{
		askHistory(m_peer, mode());
		if(m_timer<0)
			m_timer = startTimer(3000);
	}
	//qDebug() << "  zz " << mode();
}

void LogWidget::timerEvent(QTimerEvent * event)
{
	//qDebug() << "LogWidget::timerEvent() id=" << event->timerId();
	askHistory(m_peer, mode());
}

int LogWidget::mode()
{
	int r = 0;
	if(m_radioOut->isChecked())
		r = 0;
	else if(m_radioIn->isChecked())
		r = 1;
	else if(m_radioMissed->isChecked())
		r = 2;
	return r;
}

void LogWidget::modeChanged(bool b)
{
	qDebug() << "LogWidget::modeChanger()" << b << mode();
	if(b && m_peer.size() > 0)
	{
		clear();
		askHistory( m_peer, mode() );
	}
}

