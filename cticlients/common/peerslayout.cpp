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

#include <QDebug>
#include <QWidget>

#include "peerslayout.h"

PeersLayout::PeersLayout(QWidget * parent)
        : QLayout(parent), m_nb_rows(0), m_nb_columns(0)
{
	//qDebug() << "PeersLayout::PeersLayout(" << parent << ")";
}

PeersLayout::PeersLayout()
{
	//qDebug() << "PeersLayout::PeersLayout()";
}

/*! \brief same as size()
 */
QSize PeersLayout::sizeHint() const
{
	//qDebug() << "PeersLayout::sizeHints()";
	return size();
}

/*! \brief same as size()
 */
QSize PeersLayout::minimumSize() const
{
	//qDebug() << "PeersLayout::minimumSize()";
	return size();
}

/*! \brief same as size()
 */
QSize PeersLayout::maximumSize() const
{
	//qDebug() << "PeersLayout::maximumSize()";
	return size();
}

/*! \brief return the size in pixels
 */
QSize PeersLayout::size() const
{
	QSize itemSize = maxItemSize();
	return QSize( itemSize.width() * m_nb_columns, 
	              itemSize.height() * m_nb_rows );
}

/*! \brief add a widget at position
 */
void PeersLayout::addWidget(QWidget *w, QPoint pos)
{
	addChildWidget(w);
        QWidgetItem * wi = new QWidgetItem(w);
        // wi->setAlignment(Qt::AlignCenter);
        addItem(wi, pos);
}

/*! \brief add a layout item at position
 */
void PeersLayout::addItem(QLayoutItem * item, QPoint pos)
{
	m_list.append(item);
	if(pos.x() >= 0 && pos.y() >= 0) {
		if(m_listPos.contains(pos))
			pos = freePosition();
		if(pos.x() >= m_nb_columns)
			m_nb_columns = pos.x() + 1;
		if(pos.y() >= m_nb_rows)
			m_nb_rows = pos.y() + 1;
	}
	m_listPos.append(pos);
}

/*! \brief add a layout item at a free position
 */
void PeersLayout::addItem(QLayoutItem * item)
{
	m_list.append(item);
	QPoint pos = freePosition();
	if(pos.x() >= m_nb_columns)
		m_nb_columns = pos.x() + 1;
	if(pos.y() >= m_nb_rows)
		m_nb_rows = pos.y() + 1;
	m_listPos.append(pos);
}

/*! \brief set geometry of contained layout items
 */
void PeersLayout::setGeometry(const QRect &/* r*/)
{
	//qDebug() << "PeersLayout::setGeometry" << r;
	QSize itemSize = maxItemSize();
	int i, x, y;
	for(i = 0; i<m_list.size(); i++) {
		x = m_listPos[i].x();
		y = m_listPos[i].y();
		if((x >= 0) && (y >= 0)) {
			m_list[i]->setGeometry(QRect( x*itemSize.width()/*left*/, 
                                                      y*itemSize.height()/*top*/,
                                                      itemSize.width()/*width*/,
                                                      itemSize.height()/*height*/ ) );
		}
	}
}

/*! \brief return item at index
 */
QLayoutItem* PeersLayout::itemAt(int i) const
{
	//qDebug() << "PeersLayout::itemAt" << i;
	return m_list.value(i);
}

/*! \brief take item at index
 */
QLayoutItem* PeersLayout::takeAt(int i)
{
	//qDebug() << "PeersLayout::takeAt" << i;
	m_listPos.takeAt(i);
	return m_list.takeAt(i);
}

/*! \brief return number of items
 */
int PeersLayout::count() const
{
	//qDebug() << "PeersLayout::count()";
	return m_list.size();
}

/*! \brief return the maximal size of contained items
 */
QSize PeersLayout::maxItemSize() const
{
	int max_w = 150;
	int max_h = 20;
	int i;
	for(i = 0; i < m_list.size(); i++) {
		QSize size = m_list[i]->minimumSize();
		if(size.width() > max_w)
			max_w = size.width();
		if(size.height() > max_h)
			max_h = size.height();
	}
	return QSize(max_w, max_h);
}

/*! \brief find and return a free position in the grid
 */
QPoint PeersLayout::freePosition() const
{
	QPoint pos(0, 0);
	while(m_listPos.contains(pos)) {
		pos.ry()++;
		if(pos.y() > 6) {
			pos.ry() = 0;
			pos.rx()++;
		}
	}
	return pos;
}

/*! \brief return in which case of the cell is the pixel
 */
QPoint PeersLayout::getPosInGrid(QPoint pos) const
{
	QSize itemSize = maxItemSize();
	return QPoint(pos.x() / itemSize.width(), pos.y() / itemSize.height());
}

/*! \brief set item position
 */
void PeersLayout::setItemPosition(int i, QPoint pos)
{
	if(i >= 0 && i < m_listPos.size()) {
		m_listPos[i] = pos;
		if(pos.x() >= m_nb_columns)
			m_nb_columns = pos.x() + 1;
		if(pos.y() >= m_nb_rows)
			m_nb_rows = pos.y() + 1;
		if(pos.x() >= 0)
			m_list[i]->widget()->show();
		else
			m_list[i]->widget()->hide();
		setGeometry(QRect());
	}
}

/*! \brief get item position
 */
QPoint PeersLayout::getItemPosition(int i) const
{
        QPoint reply = QPoint(-1, -1);
	if(i >= 0 && i < m_listPos.size())
                reply = m_listPos[i];
        return reply;
}
