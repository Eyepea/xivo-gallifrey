#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QScrollArea>
#include <QPushButton>
#include <QDebug>
#include "searchpanel.h"
#include "peerwidget.h"
#include "dialpanel.h"
#include "switchboardengine.h"

DialPanel::DialPanel(QWidget * parent)
: QWidget(parent)
{
	QHBoxLayout * vlayout = new QHBoxLayout(this);
	vlayout->setMargin(0);
	QLabel * lbl = new QLabel( tr("Enter Number"), this );
	m_input = new QLineEdit( this );
	connect( m_input, SIGNAL(returnPressed()),
	         this, SLOT(affTextChanged()) );
	QPushButton * dialButton = new QPushButton( tr("Dial"), this);
	connect( dialButton, SIGNAL(clicked()),
	         this, SLOT(affTextChanged()) );

	vlayout->addWidget( lbl, 0, Qt::AlignCenter );
	vlayout->addWidget( m_input, 0, Qt::AlignCenter );
	vlayout->addWidget( dialButton, 0, Qt::AlignCenter );
}

/*void DialPanel::setEngine(SwitchBoardEngine * engine)
{
	m_engine = engine;
}
*/

void DialPanel::affTextChanged()
{
//	m_engine->originateCall( QString("obelisk/SIP/103"),
//	                         "obelisk/" + m_input->text() );
	emitDial( "obelisk/" + m_input->text() );
}

