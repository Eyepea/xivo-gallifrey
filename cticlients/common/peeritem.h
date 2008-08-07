/* XIVO CTI clients
 * Copyright (C) 2007, 2008  Proformatique
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

#ifndef __PEERITEM_H__
#define __PEERITEM_H__

#include <QObject>
#include <QString>
#include <QStringList>

class PeerWidget;
class UserInfo;

/*! \brief PeerItem object, linking to a PeerWidget
 */
class PeerItem// : public QObject
{
public:
	//PeerItem( const QString & ext, QObject * parent = 0 );
	PeerItem( UserInfo * );
	PeerItem( const PeerItem & );
	PeerItem( );
        UserInfo * userinfo();
        // PeerItem & operator=(const PeerItem & peer);
	//! set m_peerwidget
	void setWidget(PeerWidget * widget) { m_peerwidget = widget; };
	//! get m_peerwidget
	PeerWidget * getWidget() { return m_peerwidget; };
        
	void updateStatus(const QString &);
        void updateIMStatus(const QString &);
	void updateAgentStatus(const QStringList &);
	void updateChans(const QStringList &,
	                 const QStringList &,
	                 const QStringList &);
	void updateName(const QString &);

	void updateDisplayedStatus();
	void updateDisplayedChans();
	void updateDisplayedName();
private:
	PeerWidget * m_peerwidget;	//!< related PeerWidget

        // Properties of each peer
        UserInfo * m_ui;
	// QString m_name;		//!< Person name
        QString m_imavail;
        QString m_sipstatus;
        QString m_vmstatus;
        QStringList m_agentstatus;
        QString m_pausestatus;
        QStringList m_chanIds;
        QStringList m_chanStates;
        QStringList m_chanOthers;
        QStringList m_queuelist;
};

#endif
