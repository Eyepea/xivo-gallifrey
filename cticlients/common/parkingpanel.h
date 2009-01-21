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

#ifndef __PARKINGPANEL_H__
#define __PARKINGPANEL_H__

#include <QList>
#include <QVariant>
#include <QWidget>

class QContextMenuEvent;
class QTableWidget;
class QTableWidgetItem;

class BaseEngine;
class ExtendedTableWidget;
class PeerChannel;
class UserInfo;

/*! \brief Widget to display a list of messages
 *
 * dispays the messages in a table with timestamps.
 */
class ParkingPanel : public QWidget
{
	Q_OBJECT
public:
	ParkingPanel(QWidget * parent = 0);
        ~ParkingPanel();
protected:
	void timerEvent(QTimerEvent *);		//!< receive timer events
public slots:
        void setGuiOptions(const QVariant &);
        void setUserInfo(const UserInfo *);
        void parkingEvent(const QVariant &);
        void contextMenuEvent(QContextMenuEvent *);
private slots:
	void dialNumber();
	void hangUp();
	void itemClicked(QTableWidgetItem *);
	void itemDoubleClicked(QTableWidgetItem *);
        void proxyCallRequests(const QString &, const QString &);
signals:
	void copyNumber(const QString &);
	void actionCall(const QString &,
                        const QString &,
                        const QString &);
        void newParkEvent();
private:
	ExtendedTableWidget * m_table;	//! Table
        const UserInfo * m_userinfo;
        int m_timerid;
        int m_deltasec;
	QString m_astid;	//!< asterisk id selected
	QString m_placenum;	//!< number to call to retrieve the parked call
	QString m_parkedpeer;	//!< parked peer
	QList<PeerChannel *> m_mychannels;	//!< "my channels" list for transfer menu
};

#endif
