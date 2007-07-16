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
	m_dnd = new QCheckBox(tr("Do Not &Disturb"), this);
	connect(m_dnd, SIGNAL(toggled(bool)),
	        this, SIGNAL(dndToggled(bool)));
	layout->addWidget(m_dnd, line, 0, 1, 0);
	line++;


	m_uncondforward = new QCheckBox(tr("&Unconditional Forward"), this);
	layout->addWidget(m_uncondforward, line, 0, 1, 0);
	line++;
	QLabel * lbluncond = new QLabel(tr("Destination"), this);
	layout->addWidget(lbluncond, line, 0);
	m_uncondforwarddest = new QLineEdit(this);
	m_uncondforward->setEnabled(false);
	layout->addWidget(m_uncondforwarddest, line, 1);
	line++;

	connect(m_uncondforwarddest, SIGNAL(textChanged(const QString &)),
		this, SLOT(toggleUncondIfAllowed(const QString &)));
	connect(m_uncondforward, SIGNAL(toggled(bool)),
	        this, SLOT(uncondForwardToggled(bool)));


	m_forwardonbusy = new QCheckBox(tr("Forward on &Busy"), this);
	layout->addWidget(m_forwardonbusy, line, 0, 1, 0);
	line++;
	QLabel * lblonbusy = new QLabel(tr("Destination"), this);
	layout->addWidget(lblonbusy, line, 0);
	m_forwardonbusydest = new QLineEdit(this);
	m_forwardonbusy->setEnabled(false);
	layout->addWidget(m_forwardonbusydest, line, 1);
	line++;

	connect(m_forwardonbusydest, SIGNAL(textChanged(const QString &)),
		this, SLOT(toggleOnBusyIfAllowed(const QString &)));
	connect(m_forwardonbusy, SIGNAL(toggled(bool)),
	        this, SLOT(forwardOnBusyToggled(bool)));


	m_forwardonunavailable = new QCheckBox(tr("Forward on &No Reply"), this);
	layout->addWidget(m_forwardonunavailable, line, 0, 1, 0);
	line++;
	QLabel * lblonunavailable = new QLabel(tr("Destination"), this);
	layout->addWidget(lblonunavailable, line, 0);
	m_forwardonunavailabledest = new QLineEdit(this);
	m_forwardonunavailable->setEnabled(false);
	layout->addWidget(m_forwardonunavailabledest, line, 1);
	line++;

	connect(m_forwardonunavailabledest, SIGNAL(textChanged(const QString &)),
		this, SLOT(toggleOnUnavailIfAllowed(const QString &)));
	connect(m_forwardonunavailable, SIGNAL(toggled(bool)),
	        this, SLOT(forwardOnUnavailableToggled(bool)));
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

