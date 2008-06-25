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

#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QScrollArea>
#include <QVariant>

#include "queuespanel.h"

const QString commonqss = "QProgressBar {border: 2px solid black;border-radius: 3px;text-align: center;width: 100px; height: 15px}";

/*! \brief Constructor
 */
QueuesPanel::QueuesPanel(QWidget * parent)
        : QWidget(parent)
{
	m_gridlayout = new QGridLayout(this);

        m_maxbusy = 0;
 	m_gridlayout->setColumnStretch( 3, 1 );
 	m_gridlayout->setRowStretch( 100, 1 );
}

QueuesPanel::~QueuesPanel()
{
        // qDebug() << "QueuesPanel::~QueuesPanel()";
}

void QueuesPanel::updatePeerAgent(const QString &, const QString &, const QString &)
{
}

void QueuesPanel::setQueueList(const QString & qlist)
{
        // qDebug() << "QueuesPanel::setQueueList()" << qlist;
        QStringList qsl = qlist.split(";");
        if(qsl[2].size() > 0) {
                QString astid = qsl[0];
                QStringList queues = qsl[2].split(",");
                queues.sort();
                for(int i = 0 ; i < queues.size(); i++) {
                        QStringList qparams = queues[i].split(":");
                        QHash <QString, QString> infos;
                        QString ncalls = "0";
                        for(int j = 1 ; j < qparams.size(); j += 2)
                                infos[qparams[j]] = qparams[j+1];
                        QString queuename = qparams[0];
                        if(! m_queuelabels.contains(queuename)) {
                                m_queuelabels[queuename] = new QPushButton(queuename, this);
                                m_queuelabels[queuename]->setProperty("astid", astid);
                                m_queuelabels[queuename]->setProperty("queueid", queuename);
                                connect( m_queuelabels[queuename], SIGNAL(clicked()),
                                         this, SLOT(queueClicked()));
                                m_queuebusies[queuename] = new QProgressBar(this);
                                m_queuebusies[queuename]->setProperty("queueid", queuename);
                                m_queuebusies[queuename]->setStyleSheet(commonqss + "QProgressBar::chunk {background-color: #ffffff;}");
                                m_queuebusies[queuename]->setFormat("%v");
                                m_queueinfos[queuename] = new QLabel();
                                int linenum = m_queuelabels.size();
                                m_gridlayout->addWidget( m_queuelabels[queuename], linenum, 0, Qt::AlignLeft );
                                m_gridlayout->addWidget( m_queuebusies[queuename], linenum, 1, Qt::AlignCenter );
                                m_gridlayout->addWidget( m_queueinfos[queuename],  linenum, 2, Qt::AlignLeft );
                        }

                        m_queuebusies[queuename]->setProperty("value", infos["Calls"]);
                        m_queueinfos[queuename]->setText("SP=" + infos["ServicelevelPerf"] +
                                                         " Ab=" + infos["Abandoned"] +
                                                         " Mx=" + infos["Max"] +
                                                         " Cm=" + infos["Completed"] +
                                                         " SL=" + infos["ServiceLevel"] +
                                                         " Wt=" + infos["Weight"] +
                                                         " HT=" + infos["Holdtime"]);
                }
                update();
        }
}

void QueuesPanel::update()
{
        m_maxbusy = 0;
        foreach (QProgressBar * qpb, m_queuebusies) {
                int val = qpb->property("value").toInt();
                if(val > m_maxbusy)
                        m_maxbusy = val;
        }
        qDebug() << "QueuesPanel::update() maxbusy =" << m_maxbusy;
        
        foreach (QProgressBar * qpb, m_queuebusies) {
                QString qname = qpb->property("queueid").toString();
                int val = qpb->property("value").toInt();
                qpb->setRange(0, m_maxbusy + 1);
                // qpb->setValue(0); // trick in order to refresh
                qpb->setValue(val);
                int mul = 0;
                if(m_maxbusy > 0)
                        mul = (100 / m_maxbusy);
                QString vv = "ff";
                if(val > 0)
                        vv = QString("%1").arg(100 - val * mul, 2, 16, QChar('0'));
                qpb->setStyleSheet(commonqss + "QProgressBar::chunk {background-color: #ff"
                                   + vv + vv + ";}");
        }
}

void QueuesPanel::queueClicked()
{
        // qDebug() << "QueuesPanel::queueClicked()" << this->sender()->property("queueid");
        QString astid = this->sender()->property("astid").toString();
        QString queueid = this->sender()->property("queueid").toString();
        changeWatchedQueue(astid + " " + queueid);
}


void QueuesPanel::setQueueStatus(const QString & status)
{
        QStringList newstatuses = status.split(";");
        // qDebug() << "QueuesPanel::setQueueStatus()" << newstatuses;
        if (newstatuses.size() >= 4) {
                QString command = newstatuses[0];
                if (command == "queuechannels") {
                        QString astid = newstatuses[1];
                        QString queuename = newstatuses[2];
                        QString busyness = newstatuses[3];
                        if(m_queuebusies.contains(queuename)) {
                                m_queuebusies[queuename]->setProperty("value", busyness);
                                update();
                        }
                } else if (command == "queueentry") {
                        qDebug() << "QueuesPanel::setQueueStatus()" << status;
                }
        }
}
