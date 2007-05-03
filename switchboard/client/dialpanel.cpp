#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QScrollArea>
#include <QDebug>
#include "searchpanel.h"
#include "peerwidget.h"
#include "dialpanel.h"
#include "switchboardengine.h"

DialPanel::DialPanel(QWidget * parent)
: QWidget(parent)
{
	QVBoxLayout * vlayout = new QVBoxLayout(this);
	vlayout->setMargin(0);
	QLabel * lbl = new QLabel( tr("Number to dial"), this );
	vlayout->addWidget(lbl);
	m_input = new QLineEdit( this );
	connect( m_input, SIGNAL(returnPressed()),
	         this, SLOT(affTextChanged()) );
	vlayout->addWidget( m_input );
}

void DialPanel::setEngine(SwitchBoardEngine * engine)
{
	m_engine = engine;
}

void DialPanel::affTextChanged()
{
	this->m_engine->originateCall(QString("obelisk/SIP/103"), QString("obelisk/" + this->m_input->text()));
}

