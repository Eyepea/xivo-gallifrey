/* XIVO CTI clients
Copyright (C) 2007, 2008  Proformatique

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

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

#ifndef __LOGELTWIDGET_H__
#define __LOGELTWIDGET_H__

#include <QWidget>
#include <QDateTime>
#include <QAction>
#include <QContextMenuEvent>

class QMouseEvent;

/*! \brief Log element widget
 */
class LogEltWidget : public QWidget
{
	Q_OBJECT
public:
	//! Call direction (out or in)
	typedef enum { OutCall = 1, InCall = 2 } Direction;
	LogEltWidget( const QString & peer,
	              Direction d,
                      const QDateTime & dt,
                      int duration,
                      QWidget * parent = 0 );
	const QDateTime & dateTime() const { return m_dateTime; };	//! get m_dateTime
	const QString & peer() const { return m_peer; };		//! get m_peer
	Direction direction() const { return m_direction; };		//! get m_direction
protected:
	void contextMenuEvent(QContextMenuEvent *);
        void mouseDoubleClickEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
private slots:
	void callBackPeer();
	void doNotCallBackPeer();
signals:
	void emitDial(const QString &);		//!< signal to dial back.
        void copyNumber(const QString &);
private:
	QDateTime m_dateTime;	//!< date time of the call 
	QString m_peer;			//!< phone number who called/was called
	Direction m_direction;	//!< call direction (In/out)
	QAction * m_dialAction;	//!< dial action
};

#endif
