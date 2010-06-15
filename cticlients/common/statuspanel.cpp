/* XiVO Client
 * Copyright (C) 2007-2010, Proformatique
 *
 * This file is part of XiVO Client.
 *
 * XiVO Client is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, with a Section 7 Additional
 * Permission as follows:
 *   This notice constitutes a grant of such permission as is necessary
 *   to combine or link this software, or a modified version of it, with
 *   the OpenSSL project's "OpenSSL" library, or a derivative work of it,
 *   and to copy, modify, and distribute the resulting work. This is an
 *   extension of the special permission given by Trolltech to link the
 *   Qt code with the OpenSSL library (see
 *   <http://doc.trolltech.com/4.4/gpl.html>). The OpenSSL library is
 *   licensed under a dual license: the OpenSSL License and the original
 *   SSLeay license.
 *
 * XiVO Client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XiVO Client.  If not, see <http://www.gnu.org/licenses/>.
 */

/* $Revision$
 * $Date$
 */

#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVariant>

#include "statuspanel.h"
#include "userinfo.h"
#include "phoneinfo.h"
#include "xivoconsts.h"
#include "baseengine.h"

/*! \brief Constructor
 */
StatusPanel::StatusPanel(QWidget * parent)
    : XLet(parent)
{
    m_glayout = new QGridLayout(this);
    m_lbl = new QLabel( "", this );
    setAccessibleName( tr("Operator panel") );
    setTitle( tr("Operator") );

    QVariantMap opts = b_engine->getGuiOptions("client_gui");
        
    m_actionkey[opts["xlet_operator_keyanswer"        ].toInt()] = (QStringList() << "answer" << tr("Answer"));
    m_actionkey[opts["xlet_operator_keyhangup"        ].toInt()] = (QStringList() << "hangup" << tr("Hangup"));
    m_actionkey[opts["xlet_operator_keydtransfer"     ].toInt()] = (QStringList() << "dtransfer" << tr("D. Transfer"));
    m_actionkey[opts["xlet_operator_keyitransfer"     ].toInt()] = (QStringList() << "itransfer" << tr("I. Transfer"));
    m_actionkey[opts["xlet_operator_keyilink"         ].toInt()] = (QStringList() << "ilink" << tr("I. Link"));
    m_actionkey[opts["xlet_operator_keyicancel"       ].toInt()] = (QStringList() << "icancel" << tr("I. Cancel"));
    m_actionkey[opts["xlet_operator_keypark"          ].toInt()] = (QStringList() << "park" << tr("Park"));
    m_actionkey[opts["xlet_operator_keyatxferfinalize"].toInt()] = (QStringList() << "atxferfinalize" << tr("Finalize Transfer"));
    m_actionkey[opts["xlet_operator_keyatxfercancel"  ].toInt()] = (QStringList() << "atxfercancel" << tr("Cancel Transfer"));
    // m_actionkey[Qt::Key_Return] = (QStringList() << "numreturn" << tr("Call Number"));
        
    m_glayout->addWidget( m_lbl, 0, 0, 1, m_actionkey.size() + 4, Qt::AlignHCenter | Qt::AlignVCenter );
    m_glayout->setRowStretch(100, 1);

    // connect signal/SLOTS
    connect(b_engine, SIGNAL(userUpdated(UserInfo *)),
            this, SLOT(updateUser(UserInfo *)));
    connectDials();
}

/*! \brief add a line of widgets for a call
 *
 * create all widgets and fill m_vlinesl, m_vlinesr, m_statuses, m_tnums
 */
void StatusPanel::newCall(const QString & chan)
{
    m_vlinesl[chan] = new QFrame(this);
    m_vlinesr[chan] = new QFrame(this);
    m_statuses[chan] = new QLabel("none", this);
    m_tnums[chan] = new QLineEdit("", this);
    
    m_vlinesl[chan]->setFrameShape(QFrame::VLine);
    m_vlinesl[chan]->setLineWidth(1);
    m_vlinesr[chan]->setFrameShape(QFrame::VLine);
    m_vlinesr[chan]->setLineWidth(1);
    m_tnums[chan]->hide();
    
    changeCurrentChannel(m_currentchannel, chan);
    m_currentchannel = chan;
    
    QHash<QString, QPushButton *> k;
    QMapIterator<int, QStringList> act(m_actionkey);
    while (act.hasNext()) {
        act.next();
        QString actionname = act.value()[0];
        k[actionname] = new QPushButton("", this);
        k[actionname]->hide();
        //if((act.key() >= Qt::Key_F1) && (act.key() <= Qt::Key_F12))
        //    k[actionname]->setText(act.value()[1] + " (F" + QString::number(act.key() - Qt::Key_F1 + 1) + ")");
        //else if(act.key() == Qt::Key_Return)
        //    k[actionname]->setText(act.value()[1] + " (Return)");
        //else
        //    k[actionname]->setText(act.value()[1] + " (?)");
        
        k[actionname]->setText(act.value()[1] + " (" + QKeySequence(act.key()).toString() + ")");

        k[actionname]->setProperty("channel", chan);
        k[actionname]->setProperty("function", act.key());
        connect(k[actionname], SIGNAL(clicked()),
                this, SLOT(clicked()));
    }
    
    m_actions[chan] = k;
    int row = 1;
    foreach(int r, m_row)
    {
        if(r >= row)
            row = r + 1;
    }
    m_row[chan] = row;
}

/*! \brief update display of a line of buttons
 *
 * first clean the line by removing all buttons
 * then add wanted buttons. 
 */
void StatusPanel::updateLine(const QString & chan, const QStringList & allowed)
{
    int row = m_row[chan];
    int colnum = 1;
    qDebug() << "StatusPanel::updateLine" << row << "allowed=" << allowed;
    m_glayout->addWidget( m_vlinesl[chan], row, 0, Qt::AlignLeft );
    m_glayout->addWidget( m_statuses[chan], row, colnum++, Qt::AlignHCenter );

    QMapIterator<int, QStringList> act(m_actionkey);
    while (act.hasNext()) {
        act.next();
        QString actionname = act.value()[0];
        m_actions[chan][actionname]->hide();
        m_glayout->removeWidget(m_actions[chan][actionname]);
    }
    act.toFront();
    while (act.hasNext()) {
        act.next();
        QString actionname = act.value()[0];
        if (allowed.contains(actionname)) {
            m_glayout->addWidget( m_actions[chan][actionname], row, colnum++, Qt::AlignHCenter );
            m_actions[chan][actionname]->show();
        }
    }
    m_glayout->addWidget( m_tnums[chan],   row, m_actionkey.size() + 2, Qt::AlignHCenter );
    m_glayout->addWidget( m_vlinesr[chan], row, m_actionkey.size() + 3, Qt::AlignRight );
}

/*! \brief received when a button was pressed
 *
 * Simultate the press of a function key
 */
void StatusPanel::clicked()
{
    QString channel = sender()->property("channel").toString();
    int function = sender()->property("function").toInt();
    qDebug() << "StatusPanel::clicked()" << channel << function;
    m_currentchannel = channel;
    functionKeyPressed(function);
}

/*! \brief setup things for a direct transfer
 */
void StatusPanel::dtransfer()
{
    if(m_callchannels.contains(m_currentchannel)) {
        qDebug() << "StatusPanel::dtransfer() Direct   Transfer" << m_currentchannel;
        if(m_linestatuses[m_currentchannel] == WDTransfer) {
            m_tnums[m_currentchannel]->hide();
            m_statuses[m_currentchannel]->setFocus();
            m_linestatuses[m_currentchannel] = Ringing;
            disconnect(m_tnums[m_currentchannel], SIGNAL(returnPressed()),
                       this, SLOT(xferPressed()));
        } else {
            m_tnums[m_currentchannel]->show();
            m_tnums[m_currentchannel]->setFocus();
            m_linestatuses[m_currentchannel] = WDTransfer;
            connect(m_tnums[m_currentchannel], SIGNAL(returnPressed()),
                    this, SLOT(xferPressed()));
        }
    }
}

/*! \brief set up things for an indirect transfer
 */
void StatusPanel::itransfer()
{
    if(m_callchannels.contains(m_currentchannel)){
        qDebug() << "StatusPanel::itransfer() Indirect Transfer" << m_currentchannel;
        if(m_linestatuses[m_currentchannel] == WITransfer){
            m_tnums[m_currentchannel]->hide();
            m_statuses[m_currentchannel]->setFocus();
            m_linestatuses[m_currentchannel] = Online;
            disconnect(m_tnums[m_currentchannel], SIGNAL(returnPressed()),
                       this, SLOT(xferPressed()) );
        }else{
            m_tnums[m_currentchannel]->show();
            m_tnums[m_currentchannel]->setFocus();
            m_linestatuses[m_currentchannel] = WITransfer;
            connect(m_tnums[m_currentchannel], SIGNAL(returnPressed()),
                    this, SLOT(xferPressed()));
        }
    }
}

/*! \brief Does the transfer
 */
void StatusPanel::xferPressed()
{
    QString num = m_tnums[m_currentchannel]->text();
    QString peerchan = getPeerChan(m_currentchannel);
    qDebug() << "StatusPanel::xferPressed()" << m_currentchannel << peerchan << m_linestatuses[m_currentchannel] << num;
    if(m_linestatuses[m_currentchannel] == WDTransfer)
        emit actionCall("transfer",
                        "chan:special:me:" + peerchan,
                        "ext:" + num);
    else if(m_linestatuses[m_currentchannel] == WITransfer) {
        emit actionCall("atxfer",
                        "chan:special:me:" + m_currentchannel,
                        "ext:" + num);
        updateLine(m_currentchannel, (QStringList() << "hangup" << "ilink" << "icancel"));
    }
}

/*! \brief a key was pressed
 *
 * \todo make icancel work
 */
void StatusPanel::functionKeyPressed(int keynum)
{
    // qDebug() << "StatusPanel::functionKeyPressed()" << keynum << m_currentchannel;
                
    if(keynum == Qt::Key_Up){
        if(m_currentchannel.isEmpty())
            return;
        int ci = m_callchannels.indexOf(m_currentchannel);
        if(ci > 0)
            ci--;
        changeCurrentChannel(m_currentchannel, m_callchannels[ci]);
        m_currentchannel = m_callchannels[ci];
    }else if(keynum == Qt::Key_Down){
        if(m_currentchannel.isEmpty())
            return;
        int ci = m_callchannels.indexOf(m_currentchannel);
        if(ci < m_callchannels.size() - 1)
            ci++;
        changeCurrentChannel(m_currentchannel, m_callchannels[ci]);
        m_currentchannel = m_callchannels[ci];
    }
    QString action;
    if(m_actionkey.contains(keynum))
        action = m_actionkey[keynum][0];
    else
        return;
    
    QString userid;
    if(b_engine->getXivoClientUser())
        userid = b_engine->getXivoClientUser()->userid();

    if(m_callchannels.contains(m_currentchannel)){
        Line linestatus = m_linestatuses[m_currentchannel];
        qDebug() << "StatusPanel::functionKeyPressed()" << keynum << action << m_currentchannel << linestatus;
        if(action == "answer") {
            emit actionCall("answer", QString("chan:%1:not_relevant_here").arg(userid));
        }else if(action == "hangup"){
            if(linestatus == Ringing || linestatus == WITransfer || linestatus == WDTransfer) {
                emit actionCall("hangup", QString("chan:%1:%2").arg(userid).arg(getPeerChan(m_currentchannel))); // Call
            } else {
                //actionCall("hangup", QString("chan:%1:%2").arg(userid).arg(getPeerChan(m_currentchannel)));
                emit actionCall("hangup", QString("chan:%1:%2").arg(userid).arg(m_currentchannel)); // Call
            }
        }else if(action == "dtransfer"){
            dtransfer();
            if(linestatus == WDTransfer || linestatus == WITransfer)
                updateLine(m_currentchannel, (QStringList() << "hangup" << "dtransfer" << "itransfer"));
            else if (linestatus == Ringing)
                updateLine(m_currentchannel, (QStringList() << "hangup" << "dtransfer"));
            else
                updateLine(m_currentchannel, (QStringList() << "hangup" << "dtransfer" << "itransfer" << "numreturn"));
        }else if(action == "itransfer"){
            itransfer();
            if(linestatus == WDTransfer || linestatus == WITransfer)
                updateLine(m_currentchannel, (QStringList() << "hangup" << "dtransfer" << "itransfer"));
            else
                updateLine(m_currentchannel, (QStringList() << "hangup" << "dtransfer" << "itransfer" << "numreturn"));
        }else if(action == "park"){
            emit actionCall("transfer", "chan:special:me:" + m_currentchannel, "ext:special:parkthecall"); // Call
        }else if(action == "atxferfinalize"){
            emit actionCall("hangup", QString("chan:%1:%2").arg(userid).arg(m_currentchannel));
        }else if(action == "atxfercancel"){
            emit actionCall("hangup", QString("chan:%1:%2").arg(userid).arg(getPeerChan(m_currentchannel)));
            updateLine(m_currentchannel, (QStringList() << "hangup" << "dtransfer" << "itransfer" << "park"));
        }else if(action == "ilink"){
            emit actionCall("hangup", QString("chan:%1:%2").arg(userid).arg(m_currentchannel)); // Call
        }else if(action == "icancel"){
            /*
              qDebug() << "icancel : currentchannel=" << m_currentchannel
              << ", peerchannel=" << getPeerChan(m_currentchannel)
              << ", m_tferchannel=" << m_tferchannel;
              if(m_ui)
                  qDebug() << "  user channels" << m_ui->channelList();
            */
            //emit actionCall("hangup", QString("chan:%1:%2").arg(userid).arg(getPeerChan(m_currentchannel)));  // does nothing
            //emit actionCall("hangup", QString("chan:%1:%2").arg(userid).arg(m_currentchannel)); // finalize the indirect transfer
            // @TODO we should retrieve the Local/xxx channel that was created by the atxfer
            // command and hangup it. It may involve adding an amievent in asterisk at the right
            // place and doing some work in the xivo_daemon and here.
        }
//            if(action == "unpark")
//                qDebug() << "StatusPanel::functionKeyPressed()" << "F1 when Wait : Take back";
    }
}

/*! \brief set lines width according to current channel
 */
void StatusPanel::changeCurrentChannel(const QString & before, const QString & after)
{
    // qDebug() << "StatusPanel::changeCurrentChannel()" << before << after;
    if(before != after) {
        if(m_vlinesl.contains(before) && m_vlinesr.contains(before)) {
            m_vlinesl[before]->setLineWidth(1);
            m_vlinesr[before]->setLineWidth(1);
        }
        if(m_vlinesl.contains(after) && m_vlinesr.contains(after)) {
            m_vlinesl[after]->setLineWidth(3);
            m_vlinesr[after]->setLineWidth(3);
        }
    }
}

void StatusPanel::updateUser(UserInfo * ui)
{
    //qDebug() << " StatusPanel::updateUser()" << ui << b_engine->getXivoClientUser();
    if (!ui || !b_engine->getXivoClientUser())
        return;
    if (ui == b_engine->getXivoClientUser())
    {
        m_lbl->setText(ui->fullname());
        QStringList chanList;
        // it is concerning our user
        foreach(const QString phone, ui->phonelist()){
            const PhoneInfo * pi = ui->getPhoneInfo(phone);
            if(pi){
                QMapIterator<QString, QVariant> it( pi->comms());
                while(it.hasNext()){
                    it.next();
                    QVariantMap qvm = it.value().toMap();
                    const QString callchannel = qvm["thischannel"].toString();
                    const QString status = qvm["status"].toString();
                    const QString peerchan = qvm["peerchannel"].toString();
                    const QString num = qvm["calleridnum"].toString();
                    qDebug() << " StatusPanel::updateUser" << it.key() << status << num << callchannel << peerchan << qvm["atxfer"];
                    if(callchannel.isEmpty())
                        continue;
                    chanList << callchannel;
                    if(status == CHAN_STATUS_RINGING){
                        if(!m_callchannels.contains(callchannel)){
                            newCall(callchannel);
                            m_callchannels << callchannel;
                            m_linestatuses[callchannel] = Ringing;
                            QStringList action = QStringList() << "hangup"
                                                               << "dtransfer"
                                                               << "park";
                            if(b_engine->getGuiOptions("client_gui").value("xlet_operator_answer_work", 1).toInt()){
                                action << "answer";
                            }
                            updateLine(callchannel, action);
                            m_statuses[callchannel]->setText(tr("%1 Ringing").arg(num));
                            m_statuses[callchannel]->show();
                        }
                    }else if((status == CHAN_STATUS_LINKED_CALLED) ||
                              (status == CHAN_STATUS_LINKED_CALLER)){
                        if(!m_callchannels.contains(callchannel)){
                            newCall(callchannel);
                            m_callchannels << callchannel;
                        }
                        m_linestatuses[callchannel] = Online;
                        QStringList allowed;
                        allowed << "hangup" << "dtransfer" << "itransfer" << "park";
                        if(qvm["atxfer"].toBool())
                            allowed << "atxferfinalize" << "atxfercancel";
                        updateLine(callchannel, allowed);
                        m_statuses[callchannel]->setText(tr("Link %1").arg(num));
                        m_statuses[callchannel]->show();
                    }else if(status == CHAN_STATUS_HANGUP){
                        if(m_callchannels.contains(callchannel)){
                            removeLine(callchannel);
                        }
                    }else{
                        qDebug() << " StatusPanel::updateUser not processed" << callchannel << peerchan << status;
//                        if(m_callchannels.contains(callchannel) == true) {
//                                         m_linestatuses[callchannel] = Ready;
//                                         m_statuses[callchannel]->setText(status + " " + num);
//                                         updateLine(callchannel, (QStringList()));
//                                         m_tnums[callchannel]->hide();
//                                 }
                    }
                }
            }
        }
        //qDebug() << " StatusPane::updateUser chanList" << chanList << "m_callchannels" << m_callchannels;
        // clean up "ghost" entries...
        foreach(QString chan, m_callchannels){
            if(!chanList.contains(chan)){
                removeLine(chan);
            }
        }
    }
#if 0
    else if (ui->astid() == m_engine->getXivoClientUser()->astid())
    {
        // another user on the same asterisk instance,
        // we should look for channels concerning our user !
        foreach(const QString phone, ui->phonelist())
        {
            const PhoneInfo * pi = ui->getPhoneInfo( phone );
            if( pi )
            {
                QMapIterator<QString, QVariant> it( pi->comms() );
                while( it.hasNext() )
                {
                    it.next();
                    QVariantMap qvm = it.value().toMap();
                    // qDebug() << "StatusPanel::updatePeer()" << ui->userid() << qvm;
                    //const QString callchannel = qvm["thischannel"].toString();
                    //const QString status = qvm["status"].toString();
                    const QString peerchan = qvm["peerchannel"].toString();
                    const QString num = qvm["calleridnum"].toString();
                    if(m_engine->getXivoClientUser()->phonenumber() == num) {
                        // qDebug() << "not me" << ui->fullname() << chanlist.toMap()[ref];
                        m_tferchannel = peerchan;
                    }
                }
            }
        }
    }
#endif
}

/*! \brief get the peer channel linked to channel
 *
 * Iterate through communications of the user to
 * find the peer channel.
 */
QString StatusPanel::getPeerChan(QString const & chan) const
{
    if(!b_engine->getXivoClientUser())
        return QString();
    foreach(const QString phone, b_engine->getXivoClientUser()->phonelist()){
        const PhoneInfo * pi = b_engine->getXivoClientUser()->getPhoneInfo( phone );
        if(pi){
            QMapIterator<QString, QVariant> it(pi->comms());
            while(it.hasNext()){
                it.next();
                QVariantMap qvm = it.value().toMap();
                if(qvm["thischannel"].toString() == chan){
                    QString peerchan = qvm["peerchannel"].toString();
                    // ugly workaround...
                    if(peerchan.endsWith("<MASQ>"))
                        peerchan.remove("<MASQ>");
                    return peerchan;
                }
            }
        }
    }
    return QString();
}

/*! \brief remove widgets when calls are finished
 *
 * clean everything.
 */
void StatusPanel::removeLine(QString const & chan)
{
    m_vlinesl.take(chan)->deleteLater();
    m_vlinesr.take(chan)->deleteLater();
    m_statuses.take(chan)->deleteLater();
    m_tnums.take(chan)->deleteLater();
    m_linestatuses.remove(chan);
    QHashIterator<QString, QPushButton *> it(m_actions[chan]);
    while(it.hasNext()){
        it.next();
        it.value()->deleteLater();
    }
    m_actions.remove(chan);
    // current channel is next channel
    if(m_currentchannel == chan){
        if(m_callchannels.size() > 1) {
            int ci = m_callchannels.indexOf(m_currentchannel);
            if(ci == m_callchannels.size() - 1)
                ci --;
            changeCurrentChannel(m_currentchannel, m_callchannels[ci]);
            m_currentchannel = m_callchannels[ci];
        }else
            m_currentchannel = "";
    }
    m_callchannels.removeAll(chan);
    m_row.remove(chan);
}

void StatusPanel::doGUIConnects(QWidget * mainwindow)
{
    connect(mainwindow, SIGNAL(functionKeyPressed(int)),
            this, SLOT(functionKeyPressed(int)));
}
