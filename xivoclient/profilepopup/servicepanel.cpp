/* $Id$ */
#include <QGridLayout>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include "servicepanel.h"


ServicePanel::ServicePanel(QWidget * parent)
: QWidget(parent)
{
	int line = 0;
	QGridLayout * layout = new QGridLayout(this);
	m_voicemail = new QCheckBox(tr("Voice &Mail"), this);
	connect(m_voicemail, SIGNAL(toggled(bool)),
	        this, SIGNAL(voiceMailToggled(bool)));
	layout->addWidget(m_voicemail, line, 0, 1, 0);
	line++;
	m_callrecording = new QCheckBox(tr("Call &Recording"), this);
	connect(m_callrecording, SIGNAL(toggled(bool)),
	        this, SIGNAL(callRecordingToggled(bool)));
	layout->addWidget(m_callrecording, line, 0, 1, 0);
	line++;
	m_callfiltering = new QCheckBox(tr("Call &Filtering"), this);
	connect(m_callfiltering, SIGNAL(toggled(bool)),
	        this, SIGNAL(callFilteringToggled(bool)));
	layout->addWidget(m_callfiltering, line, 0, 1, 0);
	line++;
	m_dnd = new QCheckBox(tr("Do not &disturb"), this);
	connect(m_dnd, SIGNAL(toggled(bool)),
	        this, SIGNAL(dndToggled(bool)));
	layout->addWidget(m_dnd, line, 0, 1, 0);
	line++;

	m_uncondforward = new QCheckBox(tr("&Unconditional forward"), this);
	layout->addWidget(m_uncondforward, line, 0, 1, 0);
	line++;
	QLabel * lbluncond = new QLabel(tr("Destination"), this);
	layout->addWidget(lbluncond, line, 0);
	m_uncondforwarddest = new QLineEdit(this);
	m_uncondforwarddest->setEnabled(false);
	//m_uncondforwarddest->setInputMask("0");
	layout->addWidget(m_uncondforwarddest, line, 1);
	line++;
	connect(m_uncondforward, SIGNAL(toggled(bool)),
	        m_uncondforwarddest, SLOT(setEnabled(bool)));
	connect(m_uncondforward, SIGNAL(toggled(bool)),
	        this, SLOT(uncondForwardToggled(bool)));

	m_forwardonbusy = new QCheckBox(tr("Forward on &Busy"), this);
	layout->addWidget(m_forwardonbusy, line, 0, 1, 0);
	line++;
	QLabel * lblonbusy = new QLabel(tr("Destination"), this);
	layout->addWidget(lblonbusy, line, 0);
	m_forwardonbusydest = new QLineEdit(this);
	m_forwardonbusydest->setEnabled(false);
	layout->addWidget(m_forwardonbusydest, line, 1);
	line++;
	connect(m_forwardonbusy, SIGNAL(toggled(bool)),
	        m_forwardonbusydest, SLOT(setEnabled(bool)));
	connect(m_forwardonbusy, SIGNAL(toggled(bool)),
	        this, SLOT(forwardOnBusyToggled(bool)));

	m_forwardonunavailable = new QCheckBox(tr("Forward on &Unavailable"), this);
	layout->addWidget(m_forwardonunavailable, line, 0, 1, 0);
	line++;
	QLabel * lblonunavailable = new QLabel(tr("Destination"), this);
	layout->addWidget(lblonunavailable, line, 0);
	m_forwardonunavailabledest = new QLineEdit(this);
	m_forwardonunavailabledest->setEnabled(false);
	layout->addWidget(m_forwardonunavailabledest, line, 1);
	line++;
	connect(m_forwardonunavailable, SIGNAL(toggled(bool)),
	        m_forwardonunavailabledest, SLOT(setEnabled(bool)));
	connect(m_forwardonunavailable, SIGNAL(toggled(bool)),
	        this, SLOT(forwardOnUnavailableToggled(bool)));
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
	m_voicemail->setDown(b);
}

void ServicePanel::setCallRecording(bool b)
{
	m_callrecording->setDown(b);
}

void ServicePanel::setCallFiltering(bool b)
{
	m_callfiltering->setDown(b);
}

void ServicePanel::setDnd(bool b)
{
	m_dnd->setDown(b);
}

void ServicePanel::setUncondForward(bool b, const QString & dest)
{
	m_uncondforward->setDown(b);
	m_uncondforwarddest->setText(dest);
}

void ServicePanel::setForwardOnBusy(bool b, const QString & dest)
{
	m_forwardonbusy->setDown(b);
	m_forwardonbusydest->setText(dest);
}

void ServicePanel::setForwardOnUnavailable(bool b, const QString & dest)
{
	m_forwardonunavailable->setDown(b);
	m_forwardonunavailabledest->setText(dest);
}

