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

#ifndef __IDENTITYDISPLAY_H__
#define __IDENTITYDISPLAY_H__

#include <QHash>
#include <QLabel>
#include <QList>
#include <QObject>
#include <QVariant>
#include <QWidget>

class QComboBox;
class QProgressBar;
class QPushButton;

class UserInfo;

/*! \brief Identity Display
 */
class IdentityDisplay : public QWidget
{
	Q_OBJECT
public:
	IdentityDisplay(const QVariant &,
                        QWidget * parent = 0);
public slots:
        void setGuiOptions(const QVariant &);
        void setUserInfo(const UserInfo *);
        void setAgentList(int, const QVariant &);
	void setQueueList(bool, const QVariant &);
        void updatePeer(UserInfo *,
                        const QString &,
                        const QVariant &);
	void updatePeerAgent(int,
                             const QString &,
                             const QString &,
                             const QVariant &);
        void doAgentAction();
        void doQueueAction();
        void doQueueJoinAll();
        void doQueueLeaveAll();
        void idxChanged(const QString &);
        void updatePresence(const QVariant &);
private:
        void showAgentProps();
        void hideAgentProps();
        
        QLabel * m_user;
        QLabel * m_voicemail;
        QLabel * m_phonenum;
        QLabel * m_presencelabel;
        QComboBox * m_presencevalue;
        QLabel * m_agent;
        QFrame * m_qf;
        QLabel * m_agentstatus_label;
        QPushButton * m_agentstatus_value;
        QPushButton * m_queueaction;
        QPushButton * m_queuejoinall;
        QPushButton * m_queueleaveall;
        QComboBox * m_queuelist;
        QHash<QString, int> m_queuesindexes;
        QHash<QString, bool> m_queuesstatuses;
        QHash<QString, QString> m_queuesbusyness;
        
        QHash<QString, QString> m_presence_names;
        const UserInfo * m_ui;
        bool m_agentstatus;
        bool m_queuechangeallow;
        int m_maxqueues;
        
        QFont m_gui_font;
        quint32 m_gui_buttonsize;
        quint32 m_loginkind;
        QStringList m_functions;
signals:
        void agentAction(const QString &);
        void setAvailState(const QString &, bool);
        void changeWatchedAgent(const QString &, bool);
};

#endif
