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

#include <QComboBox>
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QSizePolicy>
#include <QPushButton>
#include <QRegExp>
#include <QScrollArea>

#include "identitydisplay.h"

/*! \brief Constructor
 */
IdentityDisplay::IdentityDisplay(QWidget * parent)
        : QWidget(parent), m_agentstatus(false)
{
	QGridLayout * glayout = new QGridLayout(this);
	// glayout->setMargin(0);
        m_user = new SizeableLabel( "", QSize(3000, 40), this );
        m_user->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        m_user->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        m_agent = new QLabel("", this);
        m_agentaction = new QPushButton(tr("Logout"), this);
        m_agentaction->setIconSize(QSize(16, 16));
        m_queueleaveall = new QPushButton(tr("Leave All"), this);
        m_queueleaveall->setIcon(QIcon(":/images/cancel.png"));
        m_queueleaveall->setIconSize(QSize(16, 16));
        m_queuejoinall = new QPushButton(tr("Join All"), this);
        m_queuejoinall->setIcon(QIcon(":/images/add.png"));
        m_queuejoinall->setIconSize(QSize(16, 16));
        m_queueaction = new QPushButton(tr("Leave"), this);
        m_queueaction->setIconSize(QSize(16, 16));

        m_queuelist = new QComboBox(this);

        connect(m_queuelist, SIGNAL(currentIndexChanged(const QString &)),
                this, SLOT(idxChanged(const QString &)));
        connect(m_agentaction, SIGNAL(clicked()),
                this, SLOT(doAgentAction()));
        connect(m_queueaction, SIGNAL(clicked()),
                this, SLOT(doQueueAction()));
        connect(m_queueleaveall, SIGNAL(clicked()),
                this, SLOT(doQueueLeaveAll()));
        connect(m_queuejoinall, SIGNAL(clicked()),
                this, SLOT(doQueueJoinAll()));

	glayout->addWidget( m_user, 0, 0, 1, 6, Qt::AlignCenter );
	glayout->addWidget( m_agent, 1, 0, Qt::AlignCenter );
	glayout->addWidget( m_agentaction, 1, 1, Qt::AlignCenter );
	glayout->addWidget( m_queueaction, 1, 2, Qt::AlignCenter );
	glayout->addWidget( m_queuelist, 1, 3, Qt::AlignCenter );
	glayout->addWidget( m_queuejoinall, 1, 4, Qt::AlignCenter );
	glayout->addWidget( m_queueleaveall, 1, 5, Qt::AlignCenter );

        m_agent->hide();
        m_agentaction->hide();

        m_queueaction->hide();
        m_queuelist->hide();
        m_queueleaveall->hide();
        m_queuejoinall->hide();

// 	glayout->setColumnStretch( 0, 1 );
// 	glayout->setRowStretch( 0, 1 );
}


/*! \brief the input was validated
 *
 * check the input and call emitDial() if ok.
 */
void IdentityDisplay::setUser(const QString & user)
{
        qDebug() << user;
        m_user->setText(user);
        // qDebug() << "IdentityDisplay::IdentityDisplay() : label" << m_user->geometry() << m_user->sizeHint();
}

void IdentityDisplay::setQueueList(const QString & qlist)
{
        QStringList qsl = qlist.split(";");
        QStringList queues = qsl[1].split(",");
        queues.sort();
        qDebug() << "queues" << queues;
        for(int i = 0 ; i < queues.size(); i++) {
                m_queuelist->addItem(queues[i]);
                m_queuelist->setItemIcon(i, QIcon(":/images/cancel.png"));
                m_queuesindexes[queues[i]] = i;
        }
        if((queues.size() > 0) && (m_agentstatus)) {
                m_queueaction->show();
                m_queuelist->show();
                m_queueleaveall->show();
                m_queuejoinall->show();
        }
}

void IdentityDisplay::setStatus(const QString & status)
{
        QStringList newstatuses = status.split(";");
        if (newstatuses.size() == 3) {
                QString command = newstatuses[0];
                QString agentnum = newstatuses[1];
                QString arg = newstatuses[2];
                m_agent->setText("Agent " + agentnum);
                if (command == "login") {
                        m_agent->show();
                        m_agentaction->show();
                        m_agentaction->setIcon(QIcon(":/images/button_ok.png"));
                        m_agentaction->setText(arg);
                        m_agentstatus = true;
                        if(m_queuesindexes.size() > 0) {
                                m_queueaction->show();
                                m_queuelist->show();
                                m_queueleaveall->show();
                                m_queuejoinall->show();
                        }
                } else if (command == "logout") {
                        m_agent->show();
                        m_agentaction->show();
                        m_agentaction->setIcon(QIcon(":/images/cancel.png"));
                        m_agentaction->setText(arg);
                        m_agentstatus = false;
                        m_queueaction->hide();
                        m_queuelist->hide();
                        m_queueleaveall->hide();
                        m_queuejoinall->hide();
                } else if (command == "joinqueue") {
                        int idx = m_queuesindexes[arg];
                        m_queuelist->setItemIcon(idx, QIcon(":/images/button_ok.png"));
                        m_queuesstatuses[arg] = true;
                        if(arg == m_queuelist->currentText())
                                idxChanged(arg);
                } else if (command == "leavequeue") {
                        int idx = m_queuesindexes[arg];
                        m_queuelist->setItemIcon(idx, QIcon(":/images/cancel.png"));
                        m_queuesstatuses[arg] = false;
                        if(arg == m_queuelist->currentText())
                                idxChanged(arg);
                }
        } else if (newstatuses.size() == 4) {
                QString command = newstatuses[0];
                if (command == "queuememberstatus") {
                        qDebug() << newstatuses;
                        QString agentnum = newstatuses[1];
                        QString status = newstatuses[2];
                        if(status == "1") {
                                m_agent->setText("Agent " + agentnum);
                        } else if(status == "3") {
                                m_agent->setText("Agent " + agentnum + " (called)");
                        }
                }
        }

}

void IdentityDisplay::doAgentAction()
{
        if(m_agentstatus) {
                agentAction("logout");
        } else {
                agentAction("login");
        }
}

void IdentityDisplay::doQueueAction()
{
        QString ctext = m_queuelist->currentText();
        bool status = m_queuesstatuses[ctext];
        if(status) {
                agentAction("leave " + ctext);
        } else {
                agentAction("join " + ctext);
        }
}

void IdentityDisplay::doQueueLeaveAll()
{
        QHashIterator<QString, bool> statiter(m_queuesstatuses);
        while(statiter.hasNext()) {
                statiter.next();
                if(statiter.value())
                        agentAction("leave " + statiter.key());
        }
}

void IdentityDisplay::doQueueJoinAll()
{
        QHashIterator<QString, bool> statiter(m_queuesstatuses);
        while(statiter.hasNext()) {
                statiter.next();
                if(! statiter.value())
                        agentAction("join " + statiter.key());
        }
}

void IdentityDisplay::idxChanged(const QString & newidx)
{
        if (m_queuesstatuses[newidx]) {
                m_queueaction->setText(tr("Leave"));
                m_queueaction->setIcon(QIcon(":/images/cancel.png"));
        } else {
                m_queueaction->setText(tr("Join"));
                m_queueaction->setIcon(QIcon(":/images/add.png"));
        }
}

SizeableLabel::SizeableLabel(const QString &text, const QSize &size, QWidget *parent)
        : QLabel(parent)
{
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        setText(text);
        m_size = size;
}

QSize SizeableLabel::sizeHint() const
{
        //        QSize size = QLabel::sizeHint();
        return m_size;
}
