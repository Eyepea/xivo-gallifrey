/* XIVO CTI clients
Copyright (C) 2007  Proformatique

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

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

#include <QCheckBox>
#include <QDebug>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

#include "servicepanel.h"

ServicePanel::ServicePanel(QWidget * parent)
        : QWidget(parent)
{
        // qDebug() << "ServicePanel::ServicePanel()";

        m_status = new ServiceStatus();
	QGroupBox * groupBox1 = new QGroupBox( tr("Services") );
	groupBox1->setAlignment( Qt::AlignLeft );
	QGridLayout * gridlayout1 = new QGridLayout(groupBox1);

	int line = 0;
	m_voicemail = new QCheckBox(tr("Voice &Mail"), this);
        m_voicemail->setObjectName("service");
	gridlayout1->addWidget(m_voicemail, line++, 0, 1, 0);
	m_callrecording = new QCheckBox(tr("Call &Recording"), this);
        m_callrecording->setObjectName("service");
	gridlayout1->addWidget(m_callrecording, line++, 0, 1, 0);
	m_callfiltering = new QCheckBox(tr("Call &Filtering"), this);
        m_callfiltering->setObjectName("service");
	gridlayout1->addWidget(m_callfiltering, line++, 0, 1, 0);
	m_dnd = new QCheckBox(tr("Do Not &Disturb"), this);
        m_dnd->setObjectName("service");
	gridlayout1->addWidget(m_dnd, line++, 0, 1, 0);

	QGroupBox * groupBox2 = new QGroupBox( tr("Call Forwards") );
	groupBox2->setAlignment( Qt::AlignLeft );
	QGridLayout * gridlayout2 = new QGridLayout(groupBox2);

	m_uncondforward = new QCheckBox(tr("&Unconditional Forward"), this);
        m_uncondforward->setObjectName("service");
	gridlayout2->addWidget(m_uncondforward, line++, 0, 1, 0);
	QLabel * lbluncond = new QLabel(tr("Destination"), this);
	gridlayout2->addWidget(lbluncond, line, 0);
	m_uncondforwarddest = new QLineEdit(this);
	m_uncondforward->setEnabled(false);
	gridlayout2->addWidget(m_uncondforwarddest, line++, 1);
        lbluncond->setObjectName("service");

	m_forwardonbusy = new QCheckBox(tr("Forward on &Busy"), this);
        m_forwardonbusy->setObjectName("service");
	gridlayout2->addWidget(m_forwardonbusy, line++, 0, 1, 0);
	QLabel * lblonbusy = new QLabel(tr("Destination"), this);
	gridlayout2->addWidget(lblonbusy, line, 0);
	m_forwardonbusydest = new QLineEdit(this);
	m_forwardonbusy->setEnabled(false);
	gridlayout2->addWidget(m_forwardonbusydest, line++, 1);
        lblonbusy->setObjectName("service");

	m_forwardonunavailable = new QCheckBox(tr("Forward on &No Reply"), this);
        m_forwardonunavailable->setObjectName("service");
	gridlayout2->addWidget(m_forwardonunavailable, line++, 0, 1, 0);
	QLabel * lblonunavailable = new QLabel(tr("Destination"), this);
	gridlayout2->addWidget(lblonunavailable, line, 0);
	m_forwardonunavailabledest = new QLineEdit(this);
	m_forwardonunavailable->setEnabled(false);
	gridlayout2->addWidget(m_forwardonunavailabledest, line++, 1);
        lblonunavailable->setObjectName("service");

        QVBoxLayout * vlayout = new QVBoxLayout(this);
        vlayout->addWidget(groupBox1);
        vlayout->addWidget(groupBox2);
        vlayout->addStretch(1);

        Reset();
	connect(m_uncondforwarddest, SIGNAL(textEdited(const QString &)),
		this, SLOT(toggleUncondIfAllowed(const QString &)));
	connect(m_forwardonbusydest, SIGNAL(textEdited(const QString &)),
		this, SLOT(toggleOnBusyIfAllowed(const QString &)));
	connect(m_forwardonunavailabledest, SIGNAL(textEdited(const QString &)),
		this, SLOT(toggleOnUnavailIfAllowed(const QString &)));
        Connect();
}

void ServicePanel::Connect()
{
        //qDebug() << "ServicePanel::Connect()";
	connect(m_voicemail, SIGNAL(clicked(bool)),
	        this, SIGNAL(voiceMailToggled(bool)));
	connect(m_callrecording, SIGNAL(clicked(bool)),
	        this, SIGNAL(callRecordingToggled(bool)));
	connect(m_callfiltering, SIGNAL(clicked(bool)),
	        this, SIGNAL(callFilteringToggled(bool)));
	connect(m_dnd, SIGNAL(clicked(bool)),
	        this, SIGNAL(dndToggled(bool)));
	connect(m_uncondforward, SIGNAL(clicked(bool)),
	        this, SLOT(uncondForwardToggled(bool)));
	connect(m_forwardonbusy, SIGNAL(clicked(bool)),
	        this, SLOT(forwardOnBusyToggled(bool)));
	connect(m_forwardonunavailable, SIGNAL(clicked(bool)),
	        this, SLOT(forwardOnUnavailableToggled(bool)));
}

void ServicePanel::DisConnect()
{
        //qDebug() << "ServicePanel::DisConnect()";
	disconnect(m_voicemail, SIGNAL(clicked(bool)),
                   this, SIGNAL(voiceMailToggled(bool)));
	disconnect(m_callrecording, SIGNAL(clicked(bool)),
                   this, SIGNAL(callRecordingToggled(bool)));
	disconnect(m_callfiltering, SIGNAL(clicked(bool)),
                   this, SIGNAL(callFilteringToggled(bool)));
	disconnect(m_dnd, SIGNAL(clicked(bool)),
                   this, SIGNAL(dndToggled(bool)));
	disconnect(m_uncondforward, SIGNAL(clicked(bool)),
                   this, SLOT(uncondForwardToggled(bool)));
	disconnect(m_forwardonbusy, SIGNAL(clicked(bool)),
                   this, SLOT(forwardOnBusyToggled(bool)));
	disconnect(m_forwardonunavailable, SIGNAL(clicked(bool)),
                   this, SLOT(forwardOnUnavailableToggled(bool)));

// 	disconnect(m_uncondforwarddest, SIGNAL(textEdited(const QString &)),
//                    this, SLOT(toggleUncondIfAllowed(const QString &)));
// 	disconnect(m_forwardonbusydest, SIGNAL(textEdited(const QString &)),
//                    this, SLOT(toggleOnBusyIfAllowed(const QString &)));
// 	disconnect(m_forwardonunavailabledest, SIGNAL(textEdited(const QString &)),
//                    this, SLOT(toggleOnUnavailIfAllowed(const QString &)));
}

void ServicePanel::Reset()
{
        //qDebug() << "ServicePanel::Reset()";
	m_voicemail->setChecked(false);
	m_callrecording->setChecked(false);
	m_callfiltering->setChecked(false);
	m_dnd->setChecked(false);

	m_uncondforward->setChecked(false);
        m_uncondforwarddest->setText("");
	m_forwardonbusy->setChecked(false);
        m_forwardonbusydest->setText("");
	m_forwardonunavailable->setChecked(false);
        m_forwardonunavailabledest->setText("");
}

void ServicePanel::toggleUncondIfAllowed(const QString & text)
{
        bool allowed     = (text.size() > 0);
        bool was_checked = (m_uncondforward->checkState() == Qt::Checked);
        m_uncondforward->setEnabled(allowed);
        if(allowed == false) {
                m_uncondforward->setChecked(false);
                if(was_checked)
                        uncondForwardChanged(false,
                                             m_uncondforwarddest->text());
        } else if(was_checked)
                uncondForwardChanged(true,
                                     m_uncondforwarddest->text());
}

void ServicePanel::toggleOnBusyIfAllowed(const QString & text)
{
        bool allowed     = (text.size() > 0);
        bool was_checked = (m_forwardonbusy->checkState() == Qt::Checked);
        m_forwardonbusy->setEnabled(allowed);
        if(allowed == false) {
                m_forwardonbusy->setChecked(false);
                if(was_checked)
                        forwardOnBusyChanged(false,
                                             m_forwardonbusydest->text());
        } else if(was_checked)
                forwardOnBusyChanged(true,
                                     m_forwardonbusydest->text());
}

void ServicePanel::toggleOnUnavailIfAllowed(const QString & text)
{
        bool allowed     = (text.size() > 0);
        bool was_checked = (m_forwardonunavailable->checkState() == Qt::Checked);
        m_forwardonunavailable->setEnabled(allowed);
        if(allowed == false) {
                m_forwardonunavailable->setChecked(false);
                if(was_checked)
                        forwardOnUnavailableChanged(false,
                                                    m_forwardonunavailabledest->text());
        } else if(was_checked)
                forwardOnUnavailableChanged(true,
                                            m_forwardonunavailabledest->text());
}

void ServicePanel::uncondForwardToggled(bool b)
{
	uncondForwardChanged(b, m_uncondforwarddest->text());
}

void ServicePanel::forwardOnBusyToggled(bool b)
{
	forwardOnBusyChanged(b, m_forwardonbusydest->text());
}

void ServicePanel::forwardOnUnavailableToggled(bool b)
{
	forwardOnUnavailableChanged(b, m_forwardonunavailabledest->text());
}


// The following actions are entered in when the status is received from the server (init or update)

void ServicePanel::setVoiceMail(bool b)
{
        m_status->setVoiceMail(b);
	m_voicemail->setChecked(b);
}

void ServicePanel::setCallRecording(bool b)
{
        m_status->setCallRecording(b);
	m_callrecording->setChecked(b);
}

void ServicePanel::setCallFiltering(bool b)
{
        m_status->setCallFiltering(b);
	m_callfiltering->setChecked(b);
}

void ServicePanel::setDnd(bool b)
{
        m_status->setDnd(b);
	m_dnd->setChecked(b);
}


void ServicePanel::setUncondForward(bool b, const QString & dest)
{
        m_status->setUncondForward(b, dest);
	m_uncondforwarddest->setText(dest);
	m_uncondforward->setChecked(b);
        m_uncondforward->setEnabled(dest.size() > 0);
}

void ServicePanel::setUncondForward(bool b)
{
        m_status->setUncondForward(b);
	m_uncondforward->setChecked(b);
}

void ServicePanel::setUncondForward(const QString & dest)
{
        m_status->setUncondForward(dest);
	m_uncondforwarddest->setText(dest);
        m_uncondforward->setEnabled(dest.size() > 0);
}


void ServicePanel::setForwardOnBusy(bool b, const QString & dest)
{
        m_status->setForwardOnBusy(b, dest);
	m_forwardonbusydest->setText(dest);
	m_forwardonbusy->setChecked(b);
	m_forwardonbusy->setEnabled(dest.size() > 0);
}

void ServicePanel::setForwardOnBusy(bool b)
{
        m_status->setForwardOnBusy(b);
	m_forwardonbusy->setChecked(b);
}

void ServicePanel::setForwardOnBusy(const QString & dest)
{
        m_status->setForwardOnBusy(dest);
	m_forwardonbusydest->setText(dest);
	m_forwardonbusy->setEnabled(dest.size() > 0);
}

void ServicePanel::setForwardOnUnavailable(bool b, const QString & dest)
{
        m_status->setForwardOnUnavailable(b, dest);
	m_forwardonunavailabledest->setText(dest);
	m_forwardonunavailable->setChecked(b);
	m_forwardonunavailable->setEnabled(dest.size() > 0);
}

void ServicePanel::setForwardOnUnavailable(bool b)
{
        m_status->setForwardOnUnavailable(b);
	m_forwardonunavailable->setChecked(b);
}

void ServicePanel::setForwardOnUnavailable(const QString & dest)
{
        m_status->setForwardOnUnavailable(dest);
	m_forwardonunavailabledest->setText(dest);
	m_forwardonunavailable->setEnabled(dest.size() > 0);
}



/*! \brief change the monitored peer
 */
void ServicePanel::setPeerToDisplay(const QString & peer)
{
        qDebug() << "ServicePanel::setPeerToDisplay()" << peer;
	m_peer = peer;
	if(m_peer.size() > 0) {
		askFeatures(m_peer);
	}
}

void ServicePanel::setRecordedStatus()
{
        // qDebug() << "ServicePanel::setRecordedStatus()";
        m_status->m_voicemail = m_voicemail->isChecked();
        m_status->m_callrecording = m_callrecording->isChecked();
        m_status->m_callfiltering = m_callfiltering->isChecked();
        m_status->m_dnd = m_dnd->isChecked();

        m_status->m_uncondforward = m_uncondforward->isChecked();
        m_status->m_forwardonbusy = m_forwardonbusy->isChecked();
        m_status->m_forwardonunavailable = m_forwardonunavailable->isChecked();
}

void ServicePanel::getRecordedStatus()
{
        // qDebug() << "ServicePanel::getRecordedStatus()";
	m_voicemail->setChecked(m_status->m_voicemail);
	m_callrecording->setChecked(m_status->m_callrecording);
	m_callfiltering->setChecked(m_status->m_callfiltering);
	m_dnd->setChecked(m_status->m_dnd);

	m_uncondforwarddest->setText(m_status->m_uncondforwarddest);
	m_uncondforward->setChecked(m_status->m_uncondforward);
	m_forwardonbusydest->setText(m_status->m_forwardonbusydest);
	m_forwardonbusy->setChecked(m_status->m_forwardonbusy);
	m_forwardonunavailabledest->setText(m_status->m_forwardonunavailabledest);
	m_forwardonunavailable->setChecked(m_status->m_forwardonunavailable);
}


////////////////////////////////////////////////:::

ServiceStatus::ServiceStatus()
{
        m_voicemail = false;
        m_callrecording = false;
        m_callfiltering = false;
        m_dnd = false;
        m_uncondforward = false;
        m_uncondforwarddest = "";
        m_forwardonbusy = false;
        m_forwardonbusydest = "";
        m_forwardonunavailable = false;
        m_forwardonunavailabledest = "";
}

void ServiceStatus::setVoiceMail(bool b)
{
	m_voicemail = b;
}

void ServiceStatus::setCallRecording(bool b)
{
	m_callrecording = b;
}

void ServiceStatus::setCallFiltering(bool b)
{
	m_callfiltering = b;
}

void ServiceStatus::setDnd(bool b)
{
	m_dnd = b;
}

void ServiceStatus::setUncondForward(bool b, const QString & dest)
{
        m_uncondforward = b;
        m_uncondforwarddest = dest;
}

void ServiceStatus::setUncondForward(bool b)
{
        m_uncondforward = b;
}

void ServiceStatus::setUncondForward(const QString & dest)
{
        m_uncondforwarddest = dest;
}

void ServiceStatus::setForwardOnBusy(bool b, const QString & dest)
{
        m_forwardonbusy = b;
        m_forwardonbusydest = dest;
}

void ServiceStatus::setForwardOnBusy(bool b)
{
        m_forwardonbusy = b;
}

void ServiceStatus::setForwardOnBusy(const QString & dest)
{
        m_forwardonbusydest = dest;
}

void ServiceStatus::setForwardOnUnavailable(bool b, const QString & dest)
{
        m_forwardonunavailable = b;
        m_forwardonunavailabledest = dest;
}

void ServiceStatus::setForwardOnUnavailable(bool b)
{
        m_forwardonunavailable = b;
}

void ServiceStatus::setForwardOnUnavailable(const QString & dest)
{
        m_forwardonunavailabledest = dest;
}

void ServiceStatus::display()
{
        qDebug() << m_voicemail << m_callrecording << m_callfiltering << m_dnd << "/"
                 << m_uncondforward << m_uncondforwarddest
                 << m_forwardonbusy << m_forwardonbusydest
                 << m_forwardonunavailable << m_forwardonunavailabledest;
}
