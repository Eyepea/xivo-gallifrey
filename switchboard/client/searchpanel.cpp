#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QScrollArea>
#include <QDebug>
#include "searchpanel.h"
#include "peerwidget.h"
#include "switchboardengine.h"

SearchPanel::SearchPanel(QWidget * parent)
: QWidget(parent)
{
	QVBoxLayout * vlayout = new QVBoxLayout(this);
	QLabel * lbl = new QLabel( tr("Name or number to search"), this );
	vlayout->addWidget(lbl);
	m_input = new QLineEdit( this );
	connect( m_input, SIGNAL(textChanged(const QString &)),
	         this, SLOT(affTextChanged(const QString &)) );
	vlayout->addWidget( m_input );
	QScrollArea * scrollarea = new QScrollArea(this);
	scrollarea->setWidgetResizable(true);
	QWidget * widget = new QWidget(scrollarea);
	scrollarea->setWidget(widget);
	QVBoxLayout * scrollarealayout = new QVBoxLayout(widget);
	m_peerlayout = new QVBoxLayout();
	scrollarealayout->addLayout( m_peerlayout );
	scrollarealayout->addStretch( 1 );
	vlayout->addWidget(scrollarea);
}

void SearchPanel::setEngine(SwitchBoardEngine * engine)
{
	m_engine = engine;
}

void SearchPanel::affTextChanged(const QString & text)
{
	int i;
	//qDebug() << "affTextChanged" << text;
	for(i = 0; i < m_peerlist.count(); i++)
	{
		if( m_peerlist[i].name().contains(text, Qt::CaseInsensitive) )
			m_peerlist[i].getWidget()->show();
		else
			m_peerlist[i].getWidget()->hide();
	}
}

void SearchPanel::updatePeer(const QString & ext,
                             const QString & name,
							 const QString & status,
							 const QString & avail,
							 const QString & corrname)
{
	int i;
	//qDebug() << "SearchPanel::updatePeer" << ext << name << status << avail << corrname;
	for(i = 0; i < m_peerlist.count(); i++)
	{
		if(ext == m_peerlist[i].ext())
		{
			m_peerlist[i].updateStatus(status, avail, corrname);
			return;
		}
	}
	Peer peer(ext, name);
	PeerWidget * peerwidget = new PeerWidget(ext, name, this);
	connect( peerwidget, SIGNAL(originateCall(const QString&, const QString&)),
	         m_engine, SLOT(originateCall(const QString&, const QString&)) );
	connect( peerwidget, SIGNAL(transferCall(const QString&, const QString&)),
	         m_engine, SLOT(transferCall(const QString&, const QString&)) );
	m_peerlayout->addWidget( peerwidget );
	if( !name.contains(m_input->text(), Qt::CaseInsensitive) )
	{
		peerwidget->hide();
	}
	peer.setWidget(peerwidget);
	peer.updateStatus(status, avail, corrname);
	m_peerlist << peer;
}

void SearchPanel::removePeer(const QString & ext)
{
	int i;
	for(i = 0; i < m_peerlist.size(); i++)
	{
		if(m_peerlist[i].ext() == ext)
		{
			PeerWidget * peerwidget = m_peerlist[i].getWidget();
			m_peerlayout->removeWidget( peerwidget );
			m_peerlist.removeAt(i);
			peerwidget->deleteLater();
			return;
		}
	}
}

void SearchPanel::removePeers()
{
	int i;
	for(i = 0; i < m_peerlist.size(); i++)
	{
		PeerWidget * peerwidget = m_peerlist[i].getWidget();
		m_peerlayout->removeWidget( peerwidget );
		peerwidget->deleteLater();
	}
	m_peerlist.clear();
}

