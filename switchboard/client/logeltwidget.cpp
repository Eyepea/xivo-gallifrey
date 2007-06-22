/* XIVO switchboard
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

#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QDebug>
#include "logeltwidget.h"

/*! \brief Constructor
 *
 * init everything and construct the sub widgets.
 */
LogEltWidget::LogEltWidget( const QString & peer, Direction d,
                            const QDateTime & dt, int duration,
							QWidget * parent )
: QWidget(parent), m_dateTime(dt), m_peer(peer), m_direction(d)
{
//	qDebug() << "  LogEltWidget::LogEltWidget()" << peer << d << dt << duration << parent;
	QHBoxLayout * layout = new QHBoxLayout( this );
	layout->setMargin(0);

	QLabel * lbldt = new QLabel( dt.toString(Qt::SystemLocaleDate), this );
	layout->addWidget(lbldt);

	QLabel * lblduration = new QLabel( this );
	int min = duration / 60;
	int sec = duration % 60;
	lblduration->setText((min?(QString::number(min) + "m"):"") + QString::number(sec) + "s");
	layout->addWidget(lblduration);

	QLabel * lblpeer = new QLabel( peer, this );
	layout->addWidget(lblpeer);

	/*	QLabel * lbldir = new QLabel( this );
	  lbldir->setText((d == OutCall)?"<=":"=>");
	  layout->addWidget(lbldir);
	*/

	layout->addStretch(1);

	m_dialAction = new QAction( tr("&Call back"), this );
	m_dialAction->setStatusTip( tr("Call back the correspondent") );
	connect( m_dialAction, SIGNAL(triggered()),
	         this, SLOT(callBackPeer()) );
}

/*! \brief display context menu
 */
void LogEltWidget::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu contextMenu(this);
	contextMenu.addAction( m_dialAction );
	contextMenu.exec(event->globalPos());
}

/*! \brief call the guy
 */
void LogEltWidget::callBackPeer()
{
	emitDial(m_peer);
}

