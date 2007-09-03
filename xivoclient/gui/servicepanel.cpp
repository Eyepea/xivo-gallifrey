/* $Id$ */
#include <QGridLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QDebug>
#include "servicepanel.h"


ServicePanel::ServicePanel(QWidget * parent)
        : QWidget(parent)
{
        qDebug() << "ServicePanel::ServicePanel()";
	int line = 0;
	QGridLayout * gridlayout = new QGridLayout();
        //	layout->setContentsMargins(30, 0, 30, 0);
        //	layout->setSpacing(0);
	gridlayout->setMargin(0);
	gridlayout->setSpacing(0);

	m_voicemail = new QCheckBox(tr("Voice &Mail"), this);
	gridlayout->addWidget(m_voicemail, line++, 0, 1, 0);
	m_callrecording = new QCheckBox(tr("Call &Recording"), this);
	gridlayout->addWidget(m_callrecording, line++, 0, 1, 0);
	m_callfiltering = new QCheckBox(tr("Call &Filtering"), this);
	gridlayout->addWidget(m_callfiltering, line++, 0, 1, 0);
	m_dnd = new QCheckBox(tr("Do Not &Disturb"), this);
	gridlayout->addWidget(m_dnd, line++, 0, 1, 0);

	m_uncondforward = new QCheckBox(tr("&Unconditional Forward"), this);
	gridlayout->addWidget(m_uncondforward, line++, 0, 1, 0);
	QLabel * lbluncond = new QLabel(tr("Destination"), this);
	gridlayout->addWidget(lbluncond, line, 0);
	m_uncondforwarddest = new QLineEdit(this);
	m_uncondforward->setEnabled(false);
	gridlayout->addWidget(m_uncondforwarddest, line++, 1);

	m_forwardonbusy = new QCheckBox(tr("Forward on &Busy"), this);
	gridlayout->addWidget(m_forwardonbusy, line++, 0, 1, 0);
	QLabel * lblonbusy = new QLabel(tr("Destination"), this);
	gridlayout->addWidget(lblonbusy, line, 0);
	m_forwardonbusydest = new QLineEdit(this);
	m_forwardonbusy->setEnabled(false);
	gridlayout->addWidget(m_forwardonbusydest, line++, 1);

	m_forwardonunavailable = new QCheckBox(tr("Forward on &No Reply"), this);
	gridlayout->addWidget(m_forwardonunavailable, line++, 0, 1, 0);
	QLabel * lblonunavailable = new QLabel(tr("Destination"), this);
	gridlayout->addWidget(lblonunavailable, line, 0);
	m_forwardonunavailabledest = new QLineEdit(this);
	m_forwardonunavailable->setEnabled(false);
	gridlayout->addWidget(m_forwardonunavailabledest, line++, 1);

        QVBoxLayout * vlayout = new QVBoxLayout(this);
        vlayout->addLayout(gridlayout);
        vlayout->addStretch();
        vlayout->setSpacing(0);

        this->setStyleSheet("* {background : white}");

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
        qDebug() << "ServicePanel::Connect()";
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
        qDebug() << "ServicePanel::DisConnect()";
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
        qDebug() << "ServicePanel::Reset()";
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
        bool allowed = (text.size() > 0);
        m_uncondforward->setEnabled(allowed);
        if(allowed == false) m_uncondforward->setChecked(false);
        uncondForwardChanged((m_uncondforward->checkState() == Qt::Checked),
                             m_uncondforwarddest->text());
}

void ServicePanel::toggleOnBusyIfAllowed(const QString & text)
{
        bool allowed = (text.size() > 0);
        m_forwardonbusy->setEnabled(allowed);
        if(allowed == false) m_forwardonbusy->setChecked(false);
        forwardOnBusyChanged((m_forwardonbusy->checkState() == Qt::Checked),
                             m_forwardonbusydest->text());
}

void ServicePanel::toggleOnUnavailIfAllowed(const QString & text)
{
        bool allowed = (text.size() > 0);
        m_forwardonunavailable->setEnabled(allowed);
        if(allowed == false) m_forwardonunavailable->setChecked(false);
        forwardOnUnavailableChanged((m_forwardonunavailable->checkState() == Qt::Checked),
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

void ServicePanel::setVoiceMail(bool b)
{
	m_voicemail->setChecked(b);
}

void ServicePanel::setCallRecording(bool b)
{
	m_callrecording->setChecked(b);
}

void ServicePanel::setCallFiltering(bool b)
{
	m_callfiltering->setChecked(b);
}

void ServicePanel::setDnd(bool b)
{
	m_dnd->setChecked(b);
}


void ServicePanel::setUncondForward(bool b, const QString & dest)
{
	m_uncondforwarddest->setText(dest);
	m_uncondforward->setChecked(b);
        m_uncondforward->setEnabled(dest.size() > 0);
}

void ServicePanel::setUncondForward(bool b)
{
	m_uncondforward->setChecked(b);
}

void ServicePanel::setUncondForward(const QString & dest)
{
	m_uncondforwarddest->setText(dest);
        m_uncondforward->setEnabled(dest.size() > 0);
}


void ServicePanel::setForwardOnBusy(bool b, const QString & dest)
{
	m_forwardonbusydest->setText(dest);
	m_forwardonbusy->setChecked(b);
	m_forwardonbusy->setEnabled(dest.size() > 0);
}

void ServicePanel::setForwardOnBusy(bool b)
{
	m_forwardonbusy->setChecked(b);
}

void ServicePanel::setForwardOnBusy(const QString & dest)
{
	m_forwardonbusydest->setText(dest);
	m_forwardonbusy->setEnabled(dest.size() > 0);
}

void ServicePanel::setForwardOnUnavailable(bool b, const QString & dest)
{
	m_forwardonunavailabledest->setText(dest);
	m_forwardonunavailable->setChecked(b);
	m_forwardonunavailable->setEnabled(dest.size() > 0);
}

void ServicePanel::setForwardOnUnavailable(bool b)
{
	m_forwardonunavailable->setChecked(b);
}

void ServicePanel::setForwardOnUnavailable(const QString & dest)
{
	m_forwardonunavailabledest->setText(dest);
	m_forwardonunavailable->setEnabled(dest.size() > 0);
}

/*! \brief change the monitored peer
 */
void ServicePanel::setPeerToDisplay(const QString & peer)
{
        qDebug() << "ServicePanel::setPeerToDisplay()";
	m_peer = peer;
	if(m_peer.size() > 0) {
		askFeatures(m_peer);
	}
}
