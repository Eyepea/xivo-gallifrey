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
#include <QContextMenuEvent>
#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QScrollArea>
#include <QVariant>

#include "agentdetailspanel.h"
#include "userinfo.h"

QColor Orange = QColor(255, 128, 0);

/*! \brief Constructor
 */
AgentdetailsPanel::AgentdetailsPanel(const QVariant & options,
                                     QWidget * parent)
        : QWidget(parent)
{
        m_linenum = 0;
	m_gridlayout = new QGridLayout(this);

        m_agent = "";
        m_agentname = new QLabel("", this);
        m_agentstatus = new QLabel("", this);
        m_agentlegend_qname = new QLabel(tr("Queues"), this);
        m_agentlegend_joined = new QLabel(tr("Joined"), this);
        m_agentlegend_paused = new QLabel(tr("Paused"), this);
        m_agentlegend_njoined = new QLabel("0", this);
        m_agentlegend_npaused = new QLabel("0", this);
        
        m_actionlegends["record"] = new QLabel(tr("Record"), this);
        m_actionlegends["alogin"] = new QLabel(tr("Login"), this);
        
        foreach (QString function, m_actionlegends.keys())
                m_action[function] = new QPushButton(this);
        m_action["record"]->setIconSize(QSize(10, 10));
        m_action["record"]->setIcon(QIcon(":/images/player_stop.png"));
        m_action["alogin"]->setIconSize(QSize(10, 10));
        m_action["alogin"]->setIcon(QIcon(":/images/button_ok.png"));
        
        m_gridlayout->setRowStretch( 100, 1 );
        m_gridlayout->addWidget(m_agentname, m_linenum, 0);
        m_gridlayout->addWidget(m_agentstatus, m_linenum, 1, 1, 7);
        m_linenum ++;
        
        int colnum = 0;
        foreach (QString function, m_actionlegends.keys()) {
                m_gridlayout->addWidget(m_actionlegends[function], m_linenum, 2 + 3 * colnum, 1, 1, Qt::AlignCenter);
                m_gridlayout->addWidget(m_action[function], m_linenum, 3 + 3 * colnum, 1, 2, Qt::AlignCenter);
                colnum ++;
        }
        m_gridlayout->setColumnStretch( 8, 1 );
        m_linenum ++;
        
        m_gridlayout->addWidget(m_agentlegend_qname, m_linenum, 0, Qt::AlignLeft);
        m_gridlayout->addWidget(m_agentlegend_joined, m_linenum, 2, 1, 3, Qt::AlignCenter);
        m_gridlayout->addWidget(m_agentlegend_paused, m_linenum, 5, 1, 3, Qt::AlignCenter);
        m_linenum ++;
        m_gridlayout->addWidget(m_agentlegend_njoined, m_linenum, 2, 1, 3, Qt::AlignCenter);
        m_gridlayout->addWidget(m_agentlegend_npaused, m_linenum, 5, 1, 3, Qt::AlignCenter);
        m_linenum ++;
        
        m_gridlayout->setVerticalSpacing(0);
        m_agentlegend_qname->hide();
        m_agentlegend_joined->hide();
        m_agentlegend_paused->hide();
        m_agentlegend_njoined->hide();
        m_agentlegend_npaused->hide();
        
        foreach (QString function, m_actionlegends.keys()) {
                m_actionlegends[function]->hide();
                m_action[function]->hide();
                m_action[function]->setProperty("function", function);
                connect( m_action[function], SIGNAL(clicked()),
                         this, SLOT(actionClicked()));
        }
        setGuiOptions(options);
}

/*! \brief Destructor
 */
AgentdetailsPanel::~AgentdetailsPanel()
{
        // qDebug() << "AgentdetailsPanel::~AgentdetailsPanel()";
}

/*! \brief set options
 */
void AgentdetailsPanel::setGuiOptions(const QVariant & options)
{
        m_options = options;
}

/*! \brief set user info 
 */
void AgentdetailsPanel::setUserInfo(const UserInfo * ui)
{
        m_userinfo = ui;
}

/*! \brief update everything
 */
void AgentdetailsPanel::updatePeerAgent(double,
                                        const QString &,
                                        const QString & what,
                                        const QVariant & params)
{
        if(what != "agentstatus")
                return;
        // qDebug() << "AgentdetailsPanel::updatePeerAgent()" << params << m_astid << m_agent;
        QString action = params.toMap()["action"].toString();
        QString astid = params.toMap()["astid"].toString();
        QString agentnum = params.toMap()["agent_channel"].toString().mid(6);
        
        if(action == "agentlogin") {
                QString phonenum = params.toMap()["phonenum"].toString();
                if((m_agent == agentnum) && (m_astid == astid)) {
                        m_agentstatus->setText(tr("logged on phone number <b>%1</b>").arg(phonenum));
                        m_action["alogin"]->setIcon(QIcon(":/images/cancel.png"));
                        m_action["alogin"]->setProperty("function", "alogout");
                        m_actionlegends["alogin"]->setText(tr("Logout"));
                }
        } else if(action == "agentlogout") {
                if((m_agent == agentnum) && (m_astid == astid)) {
                        m_agentstatus->setText(tr("logged off"));
                        m_action["alogin"]->setIcon(QIcon(":/images/button_ok.png"));
                        m_action["alogin"]->setProperty("function", "alogin");
                        m_actionlegends["alogin"]->setText(tr("Login"));
                }
        } else if(action == "joinqueue") {
                if((m_agent == agentnum) && (m_astid == astid)) {
                        QString qname = params.toMap()["queuename"].toString();
                        if(m_queue_labels.contains(qname)) {
                                QPixmap * square = new QPixmap(12, 12);
                                square->fill(Qt::green);
                                m_queue_join_status[qname]->setPixmap(* square);
                                m_queue_join_status[qname]->setProperty("joined", true);
                                m_queue_join_action[qname]->setIcon(QIcon(":/images/cancel.png"));
                                
                                QString pstatus = params.toMap()["pausedstatus"].toString();
                                if(pstatus == "1") {
                                        square->fill(Orange);
                                        m_queue_pause_status[qname]->setPixmap(* square);
                                        m_queue_pause_status[qname]->setProperty("paused", true);
                                        m_queue_pause_action[qname]->setIcon(QIcon(":/images/button_ok.png"));
                                } else {
                                        square->fill(Qt::green);
                                        m_queue_pause_status[qname]->setPixmap(* square);
                                        m_queue_pause_status[qname]->setProperty("paused", false);
                                        m_queue_pause_action[qname]->setIcon(QIcon(":/images/cancel.png"));
                                }
                                m_queue_pause_status[qname]->show();
                                m_queue_pause_action[qname]->show();
                        }
                }
        } else if(action == "leavequeue") {
                if((m_agent == agentnum) && (m_astid == astid)) {
                        QString qname = params.toMap()["queuename"].toString();
                        if(m_queue_labels.contains(qname)) {
                                QPixmap * square = new QPixmap(12, 12);
                                square->fill(Qt::gray);
                                m_queue_join_status[qname]->setPixmap(* square);
                                m_queue_join_status[qname]->setProperty("joined", false);
                                m_queue_join_action[qname]->setIcon(QIcon(":/images/button_ok.png"));

                                m_queue_pause_status[qname]->hide();
                                m_queue_pause_action[qname]->hide();
                        }
                }
        } else if(action == "paused") {
                if((m_agent == agentnum) && (m_astid == astid)) {
                        QString qname = params.toMap()["queuename"].toString();
                        if(m_queue_labels.contains(qname)) {
                                QPixmap * square = new QPixmap(12, 12);
                                square->fill(Orange);
                                m_queue_pause_status[qname]->setPixmap(* square);
                                m_queue_pause_status[qname]->setProperty("paused", true);
                                m_queue_pause_action[qname]->setIcon(QIcon(":/images/button_ok.png"));
                        }
                }
        } else if(action == "unpaused") {
                if((m_agent == agentnum) && (m_astid == astid)) {
                        QString qname = params.toMap()["queuename"].toString();
                        if(m_queue_labels.contains(qname)) {
                                QPixmap * square = new QPixmap(12, 12);
                                square->fill(Qt::green);
                                m_queue_pause_status[qname]->setPixmap(* square);
                                m_queue_pause_status[qname]->setProperty("paused", false);
                                m_queue_pause_action[qname]->setIcon(QIcon(":/images/cancel.png"));
                        }
                }
        } else if(action == "queuememberstatus") {
                if((m_agent == agentnum) && (m_astid == astid)) {
                        QString qname = params.toMap()["queuename"].toString();
                        if(m_queue_labels.contains(qname)) {
                                QString jstatus = params.toMap()["joinedstatus"].toString();
                                QString pstatus = params.toMap()["pausedstatus"].toString();
                                
                                QPixmap * square = new QPixmap(12, 12);
                                if (jstatus == "1") {
                                        square->fill(Qt::green);
                                        m_queue_join_status[qname]->setPixmap(* square);
                                        m_queue_join_status[qname]->setProperty("joined", true);
                                        m_queue_join_action[qname]->setIcon(QIcon(":/images/cancel.png"));
                                        
                                        if(pstatus == "1") {
                                                square->fill(Orange);
                                                m_queue_pause_status[qname]->setPixmap(* square);
                                                m_queue_pause_status[qname]->setProperty("paused", true);
                                                m_queue_pause_action[qname]->setIcon(QIcon(":/images/button_ok.png"));
                                        } else {
                                                square->fill(Qt::green);
                                                m_queue_pause_status[qname]->setPixmap(* square);
                                                m_queue_pause_status[qname]->setProperty("paused", false);
                                                m_queue_pause_action[qname]->setIcon(QIcon(":/images/cancel.png"));
                                        }
                                        m_queue_pause_status[qname]->show();
                                        m_queue_pause_action[qname]->show();
                                } else if (jstatus == "5") {
                                        square->fill(Qt::gray);
                                        m_queue_join_status[qname]->setPixmap(* square);
                                        m_queue_join_status[qname]->setProperty("joined", false);
                                        m_queue_join_action[qname]->setIcon(QIcon(":/images/button_ok.png"));
                                        
                                        m_queue_pause_status[qname]->hide();
                                        m_queue_pause_action[qname]->hide();
                                }
                        }
                }
        }
        // queuememberstatus
        // qDebug() << "AgentdetailsPanel::updatePeerAgent()" << params;
        
        summaryCount();
}

/*! \brief update m_agentlegend_njoined and m_agentlegend_npaused
 */
void AgentdetailsPanel::summaryCount()
{
        int njoined = 0;
        int npaused = 0;
        foreach(QString qname, m_queue_join_status.keys())
                if(m_queue_join_status[qname]->property("joined").toBool()) {
                        njoined ++;
                        if(m_queue_pause_status[qname]->property("paused").toBool())
                                npaused ++;
                }
        m_agentlegend_njoined->setText(QString::number(njoined));
        m_agentlegend_npaused->setText(QString::number(npaused));
}

/*! \brief set agent informations
 *
 *
 */
void AgentdetailsPanel::newAgent(const QString & astid, const QString & agentid, const QVariant & agentstatus)
{
        // qDebug() << "AgentdetailsPanel::newAgent()" << astid << agentid << agentstatus;
        if(m_userinfo == NULL)
                return;
        m_astid = astid;
        m_agent = agentid;
        QVariantMap agentstatusmap = agentstatus.toMap();
        QVariantMap queuesstats = agentstatusmap["queues"].toMap();
        QVariant properties = agentstatusmap["properties"];
        QString longname = QString("%1 %2").arg(agentstatusmap["firstname"].toString()).arg(agentstatusmap["lastname"].toString());
        QString lstatus = properties.toMap()["status"].toString();
        QString phonenum = properties.toMap()["phonenum"].toString();
        
        m_agentname->setText(tr("<b>%1</b> (%2) on <b>%3</b>").arg(m_agent).arg(longname).arg(m_astid));
        
        foreach(QString q, m_queue_labels.keys())
                delete m_queue_labels[q];
        foreach(QString q, m_queue_more.keys())
                delete m_queue_more[q];
        foreach(QString q, m_queue_join_status.keys())
                delete m_queue_join_status[q];
        foreach(QString q, m_queue_join_action.keys())
                delete m_queue_join_action[q];
        foreach(QString q, m_queue_pause_status.keys())
                delete m_queue_pause_status[q];
        foreach(QString q, m_queue_pause_action.keys())
                delete m_queue_pause_action[q];
        
        m_queue_labels.clear();
        m_queue_more.clear();
        m_queue_join_status.clear();
        m_queue_join_action.clear();
        m_queue_pause_status.clear();
        m_queue_pause_action.clear();
        // m_queuestatus.clear();
        
        if(lstatus == "AGENT_LOGGEDOFF") {
                m_agentstatus->setText(tr("logged off <b>%1</b>").arg(phonenum));
                m_action["alogin"]->setProperty("function", "alogin");
                m_action["alogin"]->setIcon(QIcon(":/images/button_ok.png"));
                m_actionlegends["alogin"]->setText(tr("Login"));
        } else if((lstatus == "AGENT_IDLE") || (lstatus == "AGENT_ONCALL")) {
                m_agentstatus->setText(tr("logged on phone number <b>%1</b>").arg(phonenum));
                m_action["alogin"]->setProperty("function", "alogout");
                m_action["alogin"]->setIcon(QIcon(":/images/cancel.png"));
                m_actionlegends["alogin"]->setText(tr("Logout"));
        } else
                qDebug() << "AgentdetailsPanel::newAgent() unknown status" << astid << agentid << lstatus;
        
        m_agentlegend_qname->show();
        m_agentlegend_joined->show();
        m_agentlegend_paused->show();
        m_agentlegend_njoined->show();
        m_agentlegend_npaused->show();
        foreach (QString function, m_actionlegends.keys()) {
                m_actionlegends[function]->show();
                m_action[function]->show();
        }
        
        int ii = 0;
        foreach (QString queueid, queuesstats.keys()) {
                QVariant qv = queuesstats[queueid];
                QString queuecontext = qv.toMap()["context"].toString();
                if(! m_userinfo->contexts().contains(queuecontext))
                        continue;
                QString pstatus = qv.toMap()["Paused"].toString();
                QString sstatus = qv.toMap()["Status"].toString();
                m_queue_labels[queueid] = new QLabel(queueid, this);
                
                m_queue_more[queueid] = new QPushButton(this);
                m_queue_more[queueid]->setProperty("astid", m_astid);
                m_queue_more[queueid]->setProperty("queueid", queueid);
                m_queue_more[queueid]->setProperty("action", "changequeue");
                connect( m_queue_more[queueid], SIGNAL(clicked()),
                         this, SLOT(queueClicked()));
                m_queue_more[queueid]->setIconSize(QSize(10, 10));
                m_queue_more[queueid]->setIcon(QIcon(":/images/add.png"));
                
                m_queue_join_status[queueid] = new QLabel(this);
                m_queue_join_action[queueid] = new QPushButton(this);
                m_queue_join_action[queueid]->setProperty("astid", m_astid);
                m_queue_join_action[queueid]->setProperty("queueid", queueid);
                m_queue_join_action[queueid]->setProperty("agentid", m_agent);
                m_queue_join_action[queueid]->setProperty("action", "leavejoin");
                if(! m_options.toMap()["noqueueaction"].toBool())
                        connect( m_queue_join_action[queueid], SIGNAL(clicked()),
                                 this, SLOT(queueClicked()));
                
                m_queue_pause_status[queueid] = new QLabel(this);
                m_queue_pause_action[queueid] = new QPushButton(this);
                m_queue_pause_action[queueid]->setProperty("astid", m_astid);
                m_queue_pause_action[queueid]->setProperty("queueid", queueid);
                m_queue_pause_action[queueid]->setProperty("agentid", m_agent);
                m_queue_pause_action[queueid]->setProperty("action", "pause");
                if(! m_options.toMap()["noqueueaction"].toBool())
                        connect( m_queue_pause_action[queueid], SIGNAL(clicked()),
                                 this, SLOT(queueClicked()));
                
                m_queue_join_action[queueid]->setIconSize(QSize(8, 8));
                m_queue_pause_action[queueid]->setIconSize(QSize(8, 8));
                
                if (pstatus == "") {
                        QPixmap * square = new QPixmap(12, 12);
                        square->fill(Qt::gray);
                        m_queue_join_status[queueid]->setPixmap(* square);
                        m_queue_join_status[queueid]->setProperty("joined", false);
                        m_queue_join_action[queueid]->setIcon(QIcon(":/images/button_ok.png"));
                        m_queue_pause_status[queueid]->hide();
                        m_queue_pause_action[queueid]->hide();
                } else {
                        QPixmap * square = new QPixmap(12, 12);
                        square->fill(Qt::green);
                        m_queue_join_status[queueid]->setPixmap(* square);
                        m_queue_join_status[queueid]->setProperty("joined", true);
                        m_queue_join_action[queueid]->setIcon(QIcon(":/images/cancel.png"));
                        if(pstatus == "0") {
                                square->fill(Qt::green);
                                m_queue_pause_status[queueid]->setPixmap(* square);
                                m_queue_pause_status[queueid]->setProperty("paused", false);
                                m_queue_pause_action[queueid]->setIcon(QIcon(":/images/cancel.png"));
                        } else {
                                square->fill(Orange);
                                m_queue_pause_status[queueid]->setPixmap(* square);
                                m_queue_pause_status[queueid]->setProperty("paused", true);
                                m_queue_pause_action[queueid]->setIcon(QIcon(":/images/button_ok.png"));
                        }
                        m_queue_pause_status[queueid]->show();
                        m_queue_pause_action[queueid]->show();
                }
                m_gridlayout->addWidget( m_queue_labels[queueid], ii + m_linenum, 0, Qt::AlignLeft );
                m_gridlayout->addWidget( m_queue_more[queueid], ii + m_linenum, 1, Qt::AlignCenter );
                m_gridlayout->addWidget( m_queue_join_status[queueid], ii + m_linenum, 2, Qt::AlignCenter );
                m_gridlayout->addWidget( m_queue_join_action[queueid], ii + m_linenum, 3, Qt::AlignCenter );
                m_gridlayout->addWidget( m_queue_pause_status[queueid], ii + m_linenum, 5, Qt::AlignCenter );
                m_gridlayout->addWidget( m_queue_pause_action[queueid], ii + m_linenum, 6, Qt::AlignCenter );
                ii ++;
        }
        summaryCount();
}

/*! \brief execute action on queue
 *
 * supports actions "changequeue", "leavejoin", "pause"
 */
void AgentdetailsPanel::queueClicked()
{
        // qDebug() << "AgentdetailsPanel::queueClicked()" << sender()->property("queueid");
        QString astid   = sender()->property("astid").toString();
        QString queueid = sender()->property("queueid").toString();
        QString action  = sender()->property("action").toString();
        
        if(action == "changequeue")
                changeWatchedQueue(QString("%1 %2").arg(astid).arg(queueid));
        else if(action == "leavejoin") {
                QString agentid = sender()->property("agentid").toString();
                if(m_queue_join_status[queueid]->property("joined").toBool())
                        agentAction(QString("leave %1 %2 %3").arg(queueid).arg(astid).arg(agentid));
                else {
                        // join the queue in the previously recorded paused status
                        if(m_queue_pause_status[queueid]->property("paused").toBool())
                                agentAction(QString("join %1 %2 %3 pause").arg(queueid).arg(astid).arg(agentid));
                        else
                                agentAction(QString("join %1 %2 %3 unpause").arg(queueid).arg(astid).arg(agentid));
                }
        } else if(action == "pause") {
                QString agentid = sender()->property("agentid").toString();
                if(m_queue_pause_status[queueid]->property("paused").toBool())
                        agentAction(QString("unpause %1 %2 %3").arg(queueid).arg(astid).arg(agentid));
                else
                        agentAction(QString("pause %1 %2 %3").arg(queueid).arg(astid).arg(agentid));
        } else
                qDebug() << "AgentdetailsPanel::queueClicked() : unknown action" << action;
}

/*! \brief 
 */
void AgentdetailsPanel::actionClicked()
{
        // qDebug() << "AgentdetailsPanel::actionClicked()" << sender()->property("function").toString() << m_astid << m_agent;
        QString function = sender()->property("function").toString();
        if(function == "record")
                agentAction(QString("record %1 %2").arg(m_astid).arg(m_agent));
        else if(function == "stoprecord")
                agentAction(QString("stoprecord %1 %2").arg(m_astid).arg(m_agent));
        else if(function == "alogin")
                agentAction(QString("login %1 %2").arg(m_astid).arg(m_agent));
        else if(function == "alogout")
                agentAction(QString("logout %1 %2").arg(m_astid).arg(m_agent));
}

/*! \brief triggerred on right click */
void AgentdetailsPanel::contextMenuEvent(QContextMenuEvent * event)
{
        // qDebug() << "AgentdetailsPanel::contextMenuEvent()" << event;
        m_eventpoint = event->globalPos();
        agentAction(QString("getfilelist %1 %2").arg(m_astid).arg(m_agent));
}

/*! \brief display file list */
void AgentdetailsPanel::serverFileList(const QStringList & qsl)
{
        // qDebug() << "AgentdetailsPanel::serverFileList()" << qsl;
        QMenu contextMenu(this);
        foreach (QString filename, qsl) {
                QAction * action = new QAction(filename, this);
                action->setProperty("filename", filename);
                connect( action, SIGNAL(triggered()),
                         this, SLOT(getFile()) );
                contextMenu.addAction(action);
        }
        contextMenu.exec( m_eventpoint );
}

/*! \brief update Record/Stop Record buttons
 */
void AgentdetailsPanel::statusRecord(const QString & agentnum, const QString & status)
{
        // qDebug() << "AgentdetailsPanel::statusRecord()" << agentnum << m_agent << status;
        if(agentnum == m_agent) {
                if(status == "started") {
                        m_actionlegends["record"]->setText(tr("Stop Record"));
                        m_action["record"]->setProperty("function", "stoprecord");
                } else if(status == "stopped") {
                        m_actionlegends["record"]->setText(tr("Record"));
                        m_action["record"]->setProperty("function", "record");
                }
        }
}

/*! \brief ???
 */
void AgentdetailsPanel::getFile()
{
        // qDebug() << "AgentdetailsPanel::getFile()";
        QString filename = sender()->property("filename").toString();
        agentAction(QString("getfile %1 %2 %3").arg(m_astid).arg(m_agent).arg(filename));
}

/*! \brief to save sound files
 *
 * open a QFileDialog and emit setFileName()
 */
void AgentdetailsPanel::saveToFile()
{
        // qDebug() << "AgentdetailsPanel::saveToFile()";
        QString selectedFilter;
        QString fileName = QFileDialog::getSaveFileName(this,
                                                        tr("Save Sound File"),
                                                        "",
                                                        tr("All Files (*)"),
                                                        &selectedFilter);
        if (!fileName.isEmpty())
                setFileName(fileName);
}
