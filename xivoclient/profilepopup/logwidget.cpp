/*
XIVO switchboard : 
Copyright (C) 2007  Proformatique

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/

/* $Id: logwidget.cpp 920 2007-06-15 16:45:30Z nanard $
 * $Revision: 920 $
   $Date: 2007-06-15 18:45:30 +0200 (ven, 15 jun 2007) $
*/

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLayoutItem>
#include <QLabel>
#include <QGroupBox>
#include <QRadioButton>
#include <QScrollArea>
#include <QDebug>
#include "logwidget.h"
#include "logeltwidget.h"
#include "engine.h"

/*! \brief Constructor
 *
 * build layout and child widgets.
 */
LogWidget::LogWidget(Engine * engine, QWidget * parent)
: QWidget(parent), m_engine(engine), m_timer(-1)
{
	QVBoxLayout * layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(2);
	QGroupBox * groupBox = new QGroupBox( tr("Call history"), this );
	groupBox->setAlignment( Qt::AlignHCenter );
	QHBoxLayout * vbox = new QHBoxLayout( groupBox );

	m_radioNone = new QRadioButton( tr("&None"), groupBox );
	m_radioNone->setChecked( true );
	connect( m_radioNone, SIGNAL(toggled(bool)),
	         this, SLOT(modeChanged(bool)) );
	vbox->addWidget( m_radioNone );

	m_radioOut = new QRadioButton( tr("&Outgoing"), groupBox );
	//m_radioOut->setChecked( true );
	connect( m_radioOut, SIGNAL(toggled(bool)),
	         this, SLOT(modeChanged(bool)) );
	vbox->addWidget( m_radioOut );

	m_radioIn = new QRadioButton( tr("&Incoming"), groupBox );
	connect( m_radioIn, SIGNAL(toggled(bool)),
	         this, SLOT(modeChanged(bool)) );
	vbox->addWidget( m_radioIn );

	m_radioMissed = new QRadioButton( tr("&Missed"), groupBox );
	connect( m_radioMissed, SIGNAL(toggled(bool)),
	         this, SLOT(modeChanged(bool)) );
	vbox->addWidget( m_radioMissed );

	layout->addWidget( groupBox );
	QScrollArea * scrollArea = new QScrollArea( this );
	scrollArea->setWidgetResizable( true );
	QWidget * widget = new QWidget( this );
	scrollArea->setWidget( widget );
	m_layout = new QVBoxLayout( widget );
	m_layout->addStretch(1);
	layout->addWidget( scrollArea );

	m_peer = "p/" + m_engine->serverast() + "/" + "contexte" +  "/" + m_engine->protocol() + "/" + m_engine->userid() + "/" + m_engine->userid();
}

/*! \brief add an entry
 *
 * \sa addLogEntry
 */
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
				else if(dt > logelt->dateTime())
					break;
				index = i + 1;
			}
		}
	}
	LogEltWidget * e = new LogEltWidget(peer, d, dt, duration, this);
	connect( e, SIGNAL(emitDial(const QString &)),
	         m_engine, SLOT(dialExtension(const QString &)) );
	m_layout->insertWidget(index, e);
}

/*! \brief remove all child widgets
 */
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

/*! \brief add an entry
 */
void LogWidget::addLogEntry(const QDateTime & dt, int duration,
                            const QString & peer, int d)
{
	// TODO: manage the list !
	addElement(peer, (LogEltWidget::Direction)d, dt, duration);
}

/*! \brief change the monitored peer
 */
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

/*! \brief timer event : ask for update
 */
void LogWidget::timerEvent(QTimerEvent * event)
{
	//qDebug() << "LogWidget::timerEvent() id=" << event->timerId();
	if(m_peer.size() > 0)
		askHistory(m_peer, mode());
}

/*! \brief return the mode (out/in or missed)
 */
int LogWidget::mode()
{
	int r = -1;
	if(m_radioOut->isChecked())
		r = 0;
	else if(m_radioIn->isChecked())
		r = 1;
	else if(m_radioMissed->isChecked())
		r = 2;
	return r;
}

/*! \brief triggered when mode is changed.
 *
 * clear the list and ask an update.
 */
void LogWidget::modeChanged(bool b)
{
	qDebug() << "LogWidget::modeChanged()" << b << mode();
	if(b && m_peer.size() > 0)
	{
		clear();
		askHistory( m_peer, mode() );
	}
}

