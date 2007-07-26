/* $Id$ */
#include <QGridLayout>
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
	QGridLayout * layout = new QGridLayout(this);

	m_voicemail = new QCheckBox(tr("Voice &Mail"), this);
	layout->addWidget(m_voicemail, line++, 0, 1, 0);
	m_callrecording = new QCheckBox(tr("Call &Recording"), this);
	layout->addWidget(m_callrecording, line++, 0, 1, 0);
	m_callfiltering = new QCheckBox(tr("Call &Filtering"), this);
	layout->addWidget(m_callfiltering, line++, 0, 1, 0);
	m_dnd = new QCheckBox(tr("Do Not &Disturb"), this);
	layout->addWidget(m_dnd, line++, 0, 1, 0);

	m_uncondforward = new QCheckBox(tr("&Unconditional Forward"), this);
	layout->addWidget(m_uncondforward, line++, 0, 1, 0);
	QLabel * lbluncond = new QLabel(tr("Destination"), this);
	layout->addWidget(lbluncond, line, 0);
	m_uncondforwarddest = new QLineEdit(this);
	m_uncondforward->setEnabled(false);
	layout->addWidget(m_uncondforwarddest, line++, 1);

	m_forwardonbusy = new QCheckBox(tr("Forward on &Busy"), this);
	layout->addWidget(m_forwardonbusy, line++, 0, 1, 0);
	QLabel * lblonbusy = new QLabel(tr("Destination"), this);
	layout->addWidget(lblonbusy, line, 0);
	m_forwardonbusydest = new QLineEdit(this);
	m_forwardonbusy->setEnabled(false);
	layout->addWidget(m_forwardonbusydest, line++, 1);

	m_forwardonunavailable = new QCheckBox(tr("Forward on &No Reply"), this);
	layout->addWidget(m_forwardonunavailable, line++, 0, 1, 0);
	QLabel * lblonunavailable = new QLabel(tr("Destination"), this);
	layout->addWidget(lblonunavailable, line, 0);
	m_forwardonunavailabledest = new QLineEdit(this);
	m_forwardonunavailable->setEnabled(false);
	layout->addWidget(m_forwardonunavailabledest, line++, 1);

	QLabel * dummy = new QLabel(this);
	layout->addWidget(dummy, line++, 0, 1, 0, Qt::AlignTop);


        Reset();

	connect(m_uncondforwarddest, SIGNAL(textChanged(const QString &)),
		this, SLOT(toggleUncondIfAllowed(const QString &)));
	connect(m_forwardonbusydest, SIGNAL(textChanged(const QString &)),
		this, SLOT(toggleOnBusyIfAllowed(const QString &)));
	connect(m_forwardonunavailabledest, SIGNAL(textChanged(const QString &)),
		this, SLOT(toggleOnUnavailIfAllowed(const QString &)));

        Connect();
}

void ServicePanel::Connect()
{
        qDebug() << "ServicePanel::Connect()";
	connect(m_voicemail, SIGNAL(toggled(bool)),
	        this, SIGNAL(voiceMailToggled(bool)));
	connect(m_callrecording, SIGNAL(toggled(bool)),
	        this, SIGNAL(callRecordingToggled(bool)));
	connect(m_callfiltering, SIGNAL(toggled(bool)),
	        this, SIGNAL(callFilteringToggled(bool)));
	connect(m_dnd, SIGNAL(toggled(bool)),
	        this, SIGNAL(dndToggled(bool)));
	connect(m_uncondforward, SIGNAL(toggled(bool)),
	        this, SLOT(uncondForwardToggled(bool)));
	connect(m_forwardonbusy, SIGNAL(toggled(bool)),
	        this, SLOT(forwardOnBusyToggled(bool)));
	connect(m_forwardonunavailable, SIGNAL(toggled(bool)),
	        this, SLOT(forwardOnUnavailableToggled(bool)));
}

void ServicePanel::DisConnect()
{
        qDebug() << "ServicePanel::DisConnect()";
	disconnect(m_voicemail, SIGNAL(toggled(bool)),
                   this, SIGNAL(voiceMailToggled(bool)));
	disconnect(m_callrecording, SIGNAL(toggled(bool)),
                   this, SIGNAL(callRecordingToggled(bool)));
	disconnect(m_callfiltering, SIGNAL(toggled(bool)),
                   this, SIGNAL(callFilteringToggled(bool)));
	disconnect(m_dnd, SIGNAL(toggled(bool)),
                   this, SIGNAL(dndToggled(bool)));
	disconnect(m_uncondforward, SIGNAL(toggled(bool)),
                   this, SLOT(uncondForwardToggled(bool)));
	disconnect(m_forwardonbusy, SIGNAL(toggled(bool)),
                   this, SLOT(forwardOnBusyToggled(bool)));
	disconnect(m_forwardonunavailable, SIGNAL(toggled(bool)),
                   this, SLOT(forwardOnUnavailableToggled(bool)));

// 	disconnect(m_uncondforwarddest, SIGNAL(textChanged(const QString &)),
//                    this, SLOT(toggleUncondIfAllowed(const QString &)));
// 	disconnect(m_forwardonbusydest, SIGNAL(textChanged(const QString &)),
//                    this, SLOT(toggleOnBusyIfAllowed(const QString &)));
// 	disconnect(m_forwardonunavailabledest, SIGNAL(textChanged(const QString &)),
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
	if(text.size() > 0)
		m_uncondforward->setEnabled(true);
	else
		m_uncondforward->setEnabled(false);
}

void ServicePanel::toggleOnBusyIfAllowed(const QString & text)
{
	if(text.size() > 0)
		m_forwardonbusy->setEnabled(true);
	else
		m_forwardonbusy->setEnabled(false);
}

void ServicePanel::toggleOnUnavailIfAllowed(const QString & text)
{
	if(text.size() > 0)
		m_forwardonunavailable->setEnabled(true);
	else
		m_forwardonunavailable->setEnabled(false);
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
	//qDebug() << "ServicePanel::setUncondForward" << b << dest;
	m_uncondforwarddest->setText(dest);
	m_uncondforward->setChecked(b);
}

void ServicePanel::setUncondForward(bool b)
{
	m_uncondforward->setChecked(b);
}

void ServicePanel::setUncondForward(const QString & dest)
{
	m_uncondforwarddest->setText(dest);
}

void ServicePanel::setForwardOnBusy(bool b, const QString & dest)
{
	//qDebug() << "ServicePanel::setForwardOnBusy";
	m_forwardonbusydest->setText(dest);
	m_forwardonbusy->setChecked(b);
}

void ServicePanel::setForwardOnBusy(bool b)
{
	m_forwardonbusy->setChecked(b);
}

void ServicePanel::setForwardOnBusy(const QString & dest)
{
	m_forwardonbusydest->setText(dest);
}

void ServicePanel::setForwardOnUnavailable(bool b, const QString & dest)
{
	//qDebug() << "ServicePanel::setForwardOnUnavailable";
	m_forwardonunavailabledest->setText(dest);
	m_forwardonunavailable->setChecked(b);
}

void ServicePanel::setForwardOnUnavailable(bool b)
{
	m_forwardonunavailable->setChecked(b);
}

void ServicePanel::setForwardOnUnavailable(const QString & dest)
{
	m_forwardonunavailabledest->setText(dest);
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
