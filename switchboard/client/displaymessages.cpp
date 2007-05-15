#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include "searchpanel.h"
#include "peerwidget.h"
#include "displaymessages.h"
#include "switchboardengine.h"

DisplayMessagesPanel::DisplayMessagesPanel(QWidget * parent)
	: QWidget(parent)
{
	QVBoxLayout * vlayout = new QVBoxLayout(this);
	//	vlayout->setMargin(0);
	QLabel * lbl = new QLabel( tr("Messages :"), this );
	m_text = new QLabel( "", this );
	m_text->setWordWrap(true);
	QLabel * dummy = new QLabel( "", this );
	
	vlayout->addWidget( lbl, 0 );
	vlayout->addWidget( m_text, 0 );
	vlayout->addWidget( dummy, 1 );
}

void DisplayMessagesPanel::updateMessage(const QString & str)
{
	m_text->setText(str);
}

