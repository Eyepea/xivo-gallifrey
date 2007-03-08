#include <QGridLayout>
#include <QPushButton>
#include <QDebug>
#include "switchboardwindow.h"
#include "peerwidget.h"

//Peer::Peer(const QString & ext, QObject * parent)
//: QObject(parent)
Peer::Peer(const QString & ext)
{
	m_ext = ext;
}

Peer::Peer(const Peer & peer)
//: QObject(peer.parent())
{
	m_ext = peer.m_ext;
	m_peerwidget = peer.m_peerwidget;
	//m_x = peer.m_x;
	//m_y = peer.m_y;
}

void Peer::updateStatus(const QString & status)
{
//	qDebug() << status;
	if(status == "Idle")
		m_peerwidget->setGreen();
	else if(status == "Ringing")
		m_peerwidget->setOrange();
	else if(status == "InUse")
		m_peerwidget->setRed();
	else
		m_peerwidget->setGray();
}

/*
Peer & Peer::operator=(const Peer & peer)
{
	m_ext = peer.m_ext;
	m_peerwidget = peer.m_peerwidget;
	m_x = peer.m_x;
	m_y = peer.m_y;
	return *this;
}
*/

SwitchBoardWindow::SwitchBoardWindow(QWidget * parent)
: QWidget(parent), m_engine(0)
{
	m_layout = new QGridLayout(this);
	m_x = 0;
	m_y = 0;
	/*PeerWidget * peer1 = new PeerWidget("Thomas", this);
	layout->addWidget( peer1, 0, 0 );
	layout->addWidget( new PeerWidget("Sylvain", this), 1, 0 );
	layout->addWidget( new PeerWidget("Corentin", this), 0, 1 );
	layout->addWidget( new PeerWidget("Adrien", this), 1, 1 );
	QPushButton * btnred = new QPushButton("&red", this);
	layout->addWidget( btnred, 0, 2 );
	connect( btnred, SIGNAL(clicked()), peer1, SLOT(setRed()) );
	QPushButton * btngreen = new QPushButton("&green", this);
	layout->addWidget( btngreen, 1, 2 );
	connect( btngreen, SIGNAL(clicked()), peer1, SLOT(setGreen()) );
	QPushButton * btnorange = new QPushButton("&orange", this);
	layout->addWidget( btnorange, 2, 2 );
	connect( btnorange, SIGNAL(clicked()), peer1, SLOT(setOrange()) );*/
}

void SwitchBoardWindow::setEngine(SwitchBoardEngine * engine)
{
	m_engine = engine;
}

void SwitchBoardWindow::updatePeer(const QString & ext, const QString & ext2,
                                   const QString & status)
{
	int i;
	for(i=0; i<m_peerlist.count(); i++)
	{
		//qDebug() << i << m_peerlist[i].ext();
		if(ext == m_peerlist[i].ext())
		{
			m_peerlist[i].updateStatus(status);
			return;
		}
	}
	Peer peer(ext);
	PeerWidget * peerwidget = new PeerWidget(ext, this);
	m_layout->addWidget( peerwidget, m_y, m_x++ );
	if(m_x>=4)
	{
		m_x = 0;
		m_y++;
	}
	peer.setWidget(peerwidget);
	peer.updateStatus(status);
	m_peerlist << peer;
}

