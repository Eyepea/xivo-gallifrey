/* XIVO CTI clients
 * Copyright (C) 2007-2009  Proformatique
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Linking the Licensed Program statically or dynamically with other
 * modules is making a combined work based on the Licensed Program. Thus,
 * the terms and conditions of the GNU General Public License version 2
 * cover the whole combination.
 *
 * In addition, as a special exception, the copyright holders of the
 * Licensed Program give you permission to combine the Licensed Program
 * with free software programs or libraries that are released under the
 * GNU Library General Public License version 2.0 or GNU Lesser General
 * Public License version 2.1 or any later version of the GNU Lesser
 * General Public License, and with code included in the standard release
 * of OpenSSL under a version of the OpenSSL license (with original SSLeay
 * license) which is identical to the one that was published in year 2003,
 * or modified versions of such code, with unchanged license. You may copy
 * and distribute such a system following the terms of the GNU GPL
 * version 2 for the Licensed Program and the licenses of the other code
 * concerned, provided that you include the source code of that other code
 * when and as the GNU GPL version 2 requires distribution of source code.
*/

/* $Revision$
 * $Date$
 */

#include <QContextMenuEvent>
#include <QDebug>
#include <QDesktopServices>
#include <QHeaderView>
#include <QMenu>
#include <QMouseEvent>
#include <QUrl>

#include "extendedtablewidget.h"
#include "xivoconsts.h"

/*! \brief Constructor
 */
ExtendedTableWidget::ExtendedTableWidget(QWidget * parent)
        : QTableWidget(parent)
{
	setAcceptDrops(true);
        setAlternatingRowColors(true);
        this->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
}

/*! \brief Constructor
 */
ExtendedTableWidget::ExtendedTableWidget(int rows, int columns, QWidget * parent)
        : QTableWidget(rows, columns, parent)
{
	setAcceptDrops(true);
        setAlternatingRowColors(true);
}

/*! \brief display the context Menu
 */
void ExtendedTableWidget::contextMenuEvent(QContextMenuEvent * event)
{
        // qDebug() << "ExtendedTableWidget::contextMenuEvent()" << event;
        ContextMenuEvent(event);
}

void ExtendedTableWidget::mouseMoveEvent(QMouseEvent * event)
{
        // qDebug() << "ExtendedTableWidget::mouseMoveEvent()" << event << event->pos();
	QTableWidgetItem * item = itemAt( event->pos() );
        if(item) {
                QDrag *drag = new QDrag(this);
                QMimeData *mimeData = new QMimeData;
                mimeData->setText(item->text());
                mimeData->setData(NUMBER_MIMETYPE, "");
                drag->setMimeData(mimeData);
                drag->start(Qt::CopyAction | Qt::MoveAction);
        }
}

/*! \brief filter drag events
 */
void ExtendedTableWidget::dragEnterEvent(QDragEnterEvent *event)
{
        // qDebug() << "ExtendedTableWidget::dragEnterEvent" << event->mimeData()->formats() << event->pos();
	if(  event->mimeData()->hasFormat(PEER_MIMETYPE) ||
             event->mimeData()->hasFormat(NUMBER_MIMETYPE) ||
             event->mimeData()->hasFormat(CHANNEL_MIMETYPE) ) {
                event->acceptProposedAction();
        }
}

/*! \brief filter drag move events
 *
 * Detect if the move is over a cell containing a phone number.
 */
void ExtendedTableWidget::dragMoveEvent(QDragMoveEvent *event)
{
        // qDebug() << "ExtendedTableWidget::dragMoveEvent()" << event->pos();
	if(event->proposedAction() & ( Qt::CopyAction | Qt::MoveAction ))
		event->acceptProposedAction();
	QTableWidgetItem * item = itemAt( event->pos() );
	if(item) {
		QRegExp re("\\+?[0-9\\s\\.]+");
		if(re.exactMatch( item->text() ))
			event->accept(visualItemRect(item));
		else
			event->ignore(visualItemRect(item));
	} else
		event->ignore();
}

/*! \brief receive drop event
 */
void ExtendedTableWidget::dropEvent(QDropEvent *event)
{
        // qDebug() << "ExtendedTableWidget::dropEvent()" << event->mimeData()->text() << event->pos();
	QTableWidgetItem * item = itemAt( event->pos() );
	QRegExp re("\\+?[0-9\\s\\.]+");
	if(item && re.exactMatch( item->text() )) {
                QString userid_from = QString::fromAscii(event->mimeData()->data(USERID_MIMETYPE));
                QString channel_from = QString::fromAscii(event->mimeData()->data(CHANNEL_MIMETYPE));
		if(event->mimeData()->hasFormat(CHANNEL_MIMETYPE)) {
			event->acceptProposedAction();
                        this->setProperty("action", "transfer");
			actionCall("chan:" + userid_from + ":" + channel_from, "ext:" + item->text()); // Call
		} else if(event->mimeData()->hasFormat(PEER_MIMETYPE)) {
			event->acceptProposedAction();
                        this->setProperty("action", "originate");
			actionCall("user:" + userid_from, "ext:" + item->text()); // Call
		} else {
			event->ignore();
                }
	}
	else
		event->ignore();
}
