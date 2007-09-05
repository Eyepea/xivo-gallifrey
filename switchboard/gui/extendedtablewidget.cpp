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

/* $Id$ */
#include <QContextMenuEvent>
#include <QMenu>
#include <QDebug>
#include "extendedtablewidget.h"
#include "xivoconsts.h"

/*! \brief Constructor
 */
ExtendedTableWidget::ExtendedTableWidget(QWidget * parent)
        : QTableWidget(parent)
{
	setAcceptDrops(true);
        setAlternatingRowColors(true);
        setStyleSheet("* {selection-background-color : #05aefd}\n"
                      "QScrollBar:vertical   {width: 10px; border: 0px solid black}\n"
                      "QScrollBar:horizontal {height: 10px; border: 0px solid black}\n"
                      "QScrollBar::handle:vertical   {background: qlineargradient(x1: 0.45, x2: 0.55, stop: 0 #3bc0ff, stop: 1.0 #05aefd)}\n"
                      "QScrollBar::handle:horizontal {background: qlineargradient(y1: 0.45, y2: 0.55, stop: 0 #3bc0ff, stop: 1.0 #05aefd)}\n");
}

/*! \brief Constructor
 */
ExtendedTableWidget::ExtendedTableWidget(int rows, int columns, QWidget * parent)
: QTableWidget(rows, columns, parent)
{
	setAcceptDrops(true);
}

/*! \brief display the context Menu
 */
void ExtendedTableWidget::contextMenuEvent(QContextMenuEvent * event)
{
	qDebug() << "ExtendedTableWidget::contextMenuEvent()";
	qDebug() << event->pos();
	QTableWidgetItem * item = itemAt( event->pos() );
	QRegExp re("\\+?[0-9\\s\\.]+");
	if(item && re.exactMatch( item->text() )) {
                QString menuQSS("QMenu {border: 3px solid #ffa030 ; border-radius: 4px} ; QMenu::item {background-color: transparent}");
		m_numberToDial = item->text();
		qDebug() << "preparing to dial :" << m_numberToDial;
		QMenu contextMenu(this);
		contextMenu.addAction( tr("&Dial"), this, SLOT(dialNumber()) );
                contextMenu.setStyleSheet(menuQSS);
		if(!m_mychannels.empty())
		{
			QMenu * transferMenu = new QMenu(tr("&Transfer"), &contextMenu);
                        transferMenu->setStyleSheet(menuQSS);
			QListIterator<PeerChannel *> i(m_mychannels);
			while(i.hasNext())
			{
				const PeerChannel * channel = i.next();
				transferMenu->addAction(channel->otherPeer(),
				                        channel, SLOT(transfer()));
			}
			contextMenu.addMenu(transferMenu);
		}
		contextMenu.exec( event->globalPos() );
	}
}

/*! \brief dial the number (when context menu item is toggled
 */
void ExtendedTableWidget::dialNumber()
{
	if(m_numberToDial.length() > 0)
	{
		emitDial( m_numberToDial );
	}
}

/*! \brief update call list for transfer
 */
void ExtendedTableWidget::updateMyCalls(const QStringList & chanIds,
                               const QStringList & chanStates,
							   const QStringList & chanOthers)
{
	while(!m_mychannels.isEmpty())
		delete m_mychannels.takeFirst();
	for(int i = 0; i<chanIds.count(); i++)
	{
		PeerChannel * ch = new PeerChannel(chanIds[i], chanStates[i], chanOthers[i]);
		connect(ch, SIGNAL(transferChan(const QString &)),
		        this, SLOT(transferChan(const QString &)) );
		m_mychannels << ch;
	}
}

/*! \brief transfer channel to the number
 */
void ExtendedTableWidget::transferChan(const QString & chan)
{
	transferCall(chan, m_numberToDial);
}

/*! \brief filter drag events
 */
void ExtendedTableWidget::dragEnterEvent(QDragEnterEvent *event)
{
	//qDebug() << "ExtendedTableWidget::dragEnterEvent" << event->mimeData()->formats() << event->pos();
	if(  event->mimeData()->hasFormat(PEER_MIMETYPE)
	  || event->mimeData()->hasFormat(CHANNEL_MIMETYPE) )
	{
		event->acceptProposedAction();
	}
}

/*! \brief filter drag move events
 *
 * Detect if the move is over a cell containing a phone number.
 */
void ExtendedTableWidget::dragMoveEvent(QDragMoveEvent *event)
{
	//qDebug() << "ExtendedTableWidget::dragMoveEvent()" << event->pos();
	if(event->proposedAction() & (Qt::CopyAction|Qt::MoveAction))
		event->acceptProposedAction();
	QTableWidgetItem * item = itemAt( event->pos() );
	if(item)
	{
		QRegExp re("\\+?[0-9\\s\\.]+");
		if(re.exactMatch( item->text() ))
			event->accept(visualItemRect(item));
		else
			event->ignore(visualItemRect(item));
	}
	else
		event->ignore();
}

/*! \brief receive drop event
 */
void ExtendedTableWidget::dropEvent(QDropEvent *event)
{
	qDebug() << "ExtendedTableWidget::dropEvent()" << event->mimeData()->text() << event->pos();
	QTableWidgetItem * item = itemAt( event->pos() );
	QRegExp re("\\+?[0-9\\s\\.]+");
	if(item && re.exactMatch( item->text() ))
	{
		qDebug() << item->text();
		QString from = event->mimeData()->text();
		if(event->mimeData()->hasFormat(CHANNEL_MIMETYPE))
		{
			event->acceptProposedAction();
			transferCall(from, item->text());
		}
		else if(event->mimeData()->hasFormat(PEER_MIMETYPE))
		{
			event->acceptProposedAction();
			originateCall(from, item->text());
		}
		else
			event->ignore();
	}
	else
		event->ignore();
}

