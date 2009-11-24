/* XiVO Client
 * Copyright (C) 2007-2009, Proformatique
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

#include <QCheckBox>
#include <QDebug>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QVariant>
#include <QVBoxLayout>

#include "servicepanel.h"
#include "baseengine.h"
#include "userinfo.h"

const QStringList fwdcapas = (QStringList() << "fwdrna" << "fwdbusy" << "fwdunc");
const QStringList chkcapas = (QStringList() << "enablevm" << "callrecord" << "incallfilter" << "enablednd");

ServicePanel::ServicePanel(BaseEngine * engine,
                           QWidget * parent)
    : XLet(engine, parent)
{
    setTitle( tr("Services") );
    m_capalegend["enablevm"]     = tr("Voice &Mail");
    m_capalegend["callrecord"]    = tr("Call &Recording");
    m_capalegend["incallfilter"] = tr("Call &Filtering");
    m_capalegend["enablednd"]    = tr("Do Not &Disturb");
    m_capalegend["fwdrna"]  = tr("Forward on &No Reply");
    m_capalegend["fwdbusy"] = tr("Forward on &Busy");
    m_capalegend["fwdunc"]  = tr("&Unconditional Forward");
    
    m_capas = m_engine->getGuiOptions("merged_gui").value("services").toStringList();
    // NOTE : we dont get anything here in 1.1
    qDebug() << "ServicePanel::ServicePanel" << m_capas;
    
    int line = 0;
    m_status = new ServiceStatus();
    
    QGroupBox * groupBox1 = new QGroupBox( tr("Services") );
    groupBox1->setAlignment( Qt::AlignLeft );
    groupBox1->hide();
    QGridLayout * gridlayout1 = new QGridLayout(groupBox1);
    
    foreach(QString capa, chkcapas)
        if(m_capas.contains(capa)) {
            m_chkopt[capa] = new QCheckBox(m_capalegend[capa], this);
            m_chkopt[capa]->setObjectName("service");
            m_chkopt[capa]->setProperty("capa", capa);
            gridlayout1->addWidget(m_chkopt[capa], line++, 0, 1, 0);
        }
    
    QGroupBox * groupBox2 = new QGroupBox(tr("Call Forwards"), this);
    groupBox2->setAlignment( Qt::AlignLeft );
    groupBox2->hide();
    QGridLayout * gridlayout2 = new QGridLayout(groupBox2);
    QHash<QString, QLabel *> label;
    
    foreach(QString capa, fwdcapas)
        if(m_capas.contains(capa)) {
            m_forward[capa] = new QCheckBox(m_capalegend[capa], this);
            m_forward[capa]->setObjectName("service");
            m_forward[capa]->setProperty("capa", capa);
            gridlayout2->addWidget(m_forward[capa], line++, 0, 1, 0);
            label[capa] = new QLabel(tr("Destination"), this);
            gridlayout2->addWidget(label[capa], line, 0);
            m_forwarddest[capa] = new QLineEdit(this);
            m_forwarddest[capa]->setProperty("capa", capa);
            m_forward[capa]->setEnabled(false);
            gridlayout2->addWidget(m_forwarddest[capa], line++, 1);
            label[capa]->setObjectName("service");
        }
    
    QVBoxLayout * vlayout = new QVBoxLayout(this);
    if(m_capas.contains("enablevm") || m_capas.contains("callrecord") || m_capas.contains("incallfilter") || m_capas.contains("enablednd")) {
        groupBox1->show();
        vlayout->addWidget(groupBox1);
    }
    if(m_capas.contains("fwdrna") || m_capas.contains("fwdbusy") || m_capas.contains("fwdunc")) {
        groupBox2->show();
        vlayout->addWidget(groupBox2);
    }
    vlayout->addStretch(1);
    
    Reset();
    foreach(QString capa, fwdcapas)
        if(m_capas.contains(capa)) {
            connect(m_forwarddest[capa], SIGNAL(textEdited(const QString &)),
                    this, SLOT(toggleIfAllowed(const QString &)));
        }
    Connect();

    // connect signals/slots
    connect( this, SIGNAL(askFeatures()),
             m_engine, SLOT(askFeatures()) );
    connect( m_engine, SIGNAL(monitorPeer(UserInfo *)),
             this, SLOT(monitorPeer(UserInfo *)) );
                
    connect( m_engine, SIGNAL(disconnectFeatures()),
             this, SLOT(DisConnect()) );
    connect( m_engine, SIGNAL(connectFeatures()),
             this, SLOT(Connect()) );
    connect( m_engine, SIGNAL(resetFeatures()),
             this, SLOT(Reset()) );
    connect( m_engine, SIGNAL(featurePutIsKO()),
             this, SLOT(getRecordedStatus()) );
    connect( m_engine, SIGNAL(featurePutIsOK()),
             this, SLOT(setRecordedStatus()) );
                
    connect( this, SIGNAL(chkoptChanged(const QString &, bool)),
             m_engine, SLOT(featurePutOpt(const QString &, bool)) );
                
    connect( m_engine, SIGNAL(optChanged(const QString &, bool)),
             this, SLOT(setOpt(const QString &, bool)) );
    connect( this, SIGNAL(forwardChanged(const QString &, bool, const QString &)),
             m_engine, SLOT(featurePutForward(const QString &, bool, const QString &)) );
    connect( m_engine, SIGNAL(forwardUpdated(const QString &, const QVariant &)),
             this, SLOT(setForward(const QString &, const QVariant &)) );
    connect( m_engine, SIGNAL(localUserInfoDefined(const UserInfo *)),
             this, SLOT(setUserInfo(const UserInfo *)) );
}

ServicePanel::~ServicePanel()
{
    delete m_status;
}

void ServicePanel::setUserInfo(const UserInfo * ui)
{
    if(ui == NULL)
        return;
    if((ui->mwi().size() < 3) && (m_chkopt.contains("enablevm")))
        m_chkopt["enablevm"]->hide();
}

void ServicePanel::Connect()
{
    //qDebug() << "ServicePanel::Connect()";
    foreach(QString capa, chkcapas)
        if(m_capas.contains(capa))
            connect(m_chkopt[capa], SIGNAL(clicked(bool)),
                    this, SLOT(chkoptToggled(bool)));
    foreach(QString capa, fwdcapas)
        if(m_capas.contains(capa))
            connect(m_forward[capa], SIGNAL(clicked(bool)),
                    this, SLOT(Toggled(bool)));
}

void ServicePanel::DisConnect()
{
    //qDebug() << "ServicePanel::DisConnect()";
    foreach(QString capa, chkcapas)
        if(m_capas.contains(capa))
            disconnect(m_chkopt[capa], SIGNAL(clicked(bool)),
                       this, SLOT(chkoptToggled(bool)));
    foreach(QString capa, fwdcapas)
        if(m_capas.contains(capa))
            disconnect(m_forward[capa], SIGNAL(clicked(bool)),
                       this, SLOT(Toggled(bool)));
    // foreach(QString capa, fwdcapas)
    // if(m_capas.contains(capa))
    // disconnect(m_forward[capa], SIGNAL(textEdited(const QString &)),
    // this, SLOT(toggleIfAllowed(const QString &)));
}

void ServicePanel::Reset()
{
    //qDebug() << "ServicePanel::Reset()";
    foreach(QString capa, chkcapas)
        if(m_capas.contains(capa))
            m_chkopt[capa]->setChecked(false);
    foreach(QString capa, fwdcapas)
        if(m_capas.contains(capa)) {
            m_forward[capa]->setChecked(false);
            m_forwarddest[capa]->setText("");
        }
}

void ServicePanel::toggleIfAllowed(const QString & text)
{
    QString capa = sender()->property("capa").toString();
    bool allowed     = (text.size() > 0);
    bool was_checked = (m_forward[capa]->checkState() == Qt::Checked);
    m_forward[capa]->setEnabled(allowed);
    if(allowed == false) {
        m_forward[capa]->setChecked(false);
        if(was_checked)
            forwardChanged(capa, false,
                           m_forwarddest[capa]->text());
    } else if(was_checked)
        forwardChanged(capa, true,
                       m_forwarddest[capa]->text());
}

void ServicePanel::chkoptToggled(bool b)
{
    QString capa = sender()->property("capa").toString();
    chkoptChanged(capa, b);
}

void ServicePanel::Toggled(bool b)
{
    QString capa = sender()->property("capa").toString();
    forwardChanged(capa, b, m_forwarddest[capa]->text());
}

// The following actions are entered in when the status is received from the server (init or update)

void ServicePanel::setOpt(const QString & capa, bool b)
{
    m_status->setOpt(capa, b);
    if(m_capas.contains(capa))
        m_chkopt[capa]->setChecked(b);
}

void ServicePanel::setForward(const QString & capa, const QVariant & value)
{
    bool b = value.toMap()["enabled"].toBool();
    QString thiscapa = "fwd" + capa;
    if(m_capas.contains(thiscapa)) {
        if(value.toMap().keys().contains("number")) {
            QString dest = value.toMap()["number"].toString();
            m_status->setForward(thiscapa, b, dest);
            m_forward[thiscapa]->setChecked(b);
            m_forwarddest[thiscapa]->setText(dest);
            m_forward[thiscapa]->setEnabled(dest.size() > 0);
        } else {
            // m_status->setForward(thiscapa, b, dest);
            m_forward[thiscapa]->setChecked(b);
        }
    }
}


/*! \brief change the monitored peer
 */
void ServicePanel::monitorPeer(UserInfo * /*ui*/)
{
    // qDebug() << "ServicePanel::monitorPeer()" << peer;
    askFeatures();
}

void ServicePanel::setRecordedStatus()
{
    // qDebug() << "ServicePanel::setRecordedStatus()";
    foreach(QString capa, chkcapas)
        if(m_capas.contains(capa))
            m_status->m_chkopt[capa] = m_chkopt[capa]->isChecked();
    foreach(QString capa, fwdcapas)
        if(m_capas.contains(capa))
            m_status->m_forward[capa] = m_forward[capa]->isChecked();
}

void ServicePanel::getRecordedStatus()
{
    // qDebug() << "ServicePanel::getRecordedStatus()";
    foreach(QString capa, chkcapas)
        if(m_capas.contains(capa))
            m_chkopt[capa]->setChecked(m_status->m_chkopt[capa]);
    foreach(QString capa, fwdcapas)
        if(m_capas.contains(capa)) {
            m_forwarddest[capa]->setText(m_status->m_forwarddest[capa]);
            m_forward[capa]->setChecked(m_status->m_forward[capa]);
        }
}


ServiceStatus::ServiceStatus()
{
    foreach(QString capa, chkcapas)
        m_chkopt[capa] = false;
    foreach(QString capa, fwdcapas) {
        m_forward[capa] = false;
        m_forwarddest[capa] = "";
    }
}

void ServiceStatus::setOpt(const QString & capa, bool b)
{
    m_chkopt[capa] = b;
}

void ServiceStatus::setForward(const QString & capa, bool b, const QString & dest)
{
    m_forward[capa] = b;
    m_forwarddest[capa] = dest;
}

void ServiceStatus::display()
{
    qDebug() << m_chkopt << "/" << m_forward << m_forwarddest;
}
