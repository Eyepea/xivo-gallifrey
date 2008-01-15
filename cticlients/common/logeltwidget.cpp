/* XIVO CTI clients
Copyright (C) 2007, 2008  Proformatique

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

/* $Revision$
 * $Date$
 */

#include <QDebug>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>

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
	QGridLayout * glayout = new QGridLayout( this );
	//	QHBoxLayout * layout1 = new QHBoxLayout( this );
	//	layout1->setMargin(0);

	QLabel * lblpeer = new QLabel( peer, this );
	lblpeer->setObjectName("logpeername");
	lblpeer->setFont(QFont("helvetica", 10, QFont::Bold));
	lblpeer->setMargin(0);
	glayout->addWidget(lblpeer, 0, 0);

	QLabel * lbldt = new QLabel( dt.toString(Qt::SystemLocaleDate) + "   ", this );
	lbldt->setFont(QFont("helvetica", 10, QFont::Light));
	lbldt->setMargin(0);
	glayout->addWidget(lbldt, 1, 0);

	QLabel * lblduration = new QLabel( this );
	int min = duration / 60;
	int sec = duration % 60;
	lblduration->setText((min?(QString::number(min) + "min "):"") + QString::number(sec) + "s");
	lblduration->setFont(QFont("helvetica", 10, QFont::Light));
	lblduration->setMargin(0);
	glayout->addWidget(lblduration, 1, 1);

	QLabel * lbldummy = new QLabel( "", this );
	lbldummy->setMargin(0);
	glayout->addWidget(lbldummy, 1, 2);

	/*	QLabel * lbldir = new QLabel( this );
	  lbldir->setText((d == OutCall)?"<=":"=>");
	  glayout->addWidget(lbldir);
	*/

	glayout->setSpacing(0);
	glayout->setColumnStretch(0, 0);
	glayout->setColumnStretch(1, 0);
	glayout->setColumnStretch(2, 1);

	m_dialAction = new QAction( tr("&Call back"), this );
	m_dialAction->setStatusTip( tr("Call back the correspondent") );
	connect( m_dialAction, SIGNAL(triggered()),
	         this, SLOT(callBackPeer()) );
}

void LogEltWidget::mouseDoubleClickEvent(QMouseEvent */* event*/)
{
        callBackPeer();
}

void LogEltWidget::mouseReleaseEvent(QMouseEvent */* event*/)
{
        doNotCallBackPeer();
}

/*! \brief display context menu
 */
void LogEltWidget::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu contextMenu(this);
	contextMenu.addAction( m_dialAction );
	contextMenu.exec(event->globalPos());
}

/*! \brief call the guy if dials is true, otherwise transmits the number in order to paste it
 *     somewhere, for instance in the dial input widget
 */
void LogEltWidget::callBackPeer()
{
	QStringList qsl1 = m_peer.split("<");
        QString number = m_peer;
	if (qsl1.size() > 1) {
		QStringList qsl2 = qsl1[1].split(">");
                number = qsl2[0];
        }

        emitDial(number);
}

void LogEltWidget::doNotCallBackPeer()
{
	QStringList qsl1 = m_peer.split("<");
        QString number = m_peer;
	if (qsl1.size() > 1) {
		QStringList qsl2 = qsl1[1].split(">");
                number = qsl2[0];
        }

        copyNumber(number);
}

