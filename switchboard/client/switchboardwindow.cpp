#include <QGridLayout>
#include <QPushButton>
#include <QDebug>
#include <QToolTip>
#include <QPoint>
#include <QSettings>
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

void Peer::updateStatus(const QString & status,
			const QString & corrname)
{
//	qDebug() << status;
	if(status == "Ready")
		m_peerwidget->setGreen();
	else if(status == "Ringing")
		m_peerwidget->setOrange();
	else if(status == "On the phone")
		m_peerwidget->setRed();
	else
		m_peerwidget->setGray();
	if(corrname == "")
	  m_peerwidget->setToolTip(status);
	else
	  m_peerwidget->setToolTip(status + "\n" + corrname);
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
	QSettings settings;
	m_width = settings.value("display/width", 5).toInt();
}

void SwitchBoardWindow::setEngine(SwitchBoardEngine * engine)
{
	m_engine = engine;
}

void SwitchBoardWindow::updatePeer(const QString & ext,
                                   const QString & status,
				   const QString & corrname)
{
	int i;
	// first search in the peerlist
	for(i=0; i<m_peerlist.count(); i++)
	{
		//		qDebug() << i << m_peerlist[i].ext();
		if(ext == m_peerlist[i].ext())
		{
		  m_peerlist[i].updateStatus(status, corrname);
			return;
		}
	}
	// if not found in the peerlist, create a new Peer
	Peer peer(ext);
	PeerWidget * peerwidget = new PeerWidget(ext, m_engine, this);
	m_layout->addWidget( peerwidget, m_y, m_x++ );
	if(m_x >= m_width)
	{
		m_x = 0;
		m_y++;
	}
	peer.setWidget(peerwidget);
	peer.updateStatus(status, corrname);
	m_peerlist << peer;
}

/*
void SwitchBoardWindow::addPeer(const QString & ext,
				const QString & status)
{
	Peer peer(ext);
	PeerWidget * peerwidget = new PeerWidget(ext, m_engine, this);
	m_layout->addWidget( peerwidget, m_y, m_x++ );
	if(m_x >= m_width)
	{
		m_x = 0;
		m_y++;
	}
	peer.setWidget(peerwidget);
	peer.updateStatus(status);
	m_peerlist << peer;
}
*/

void SwitchBoardWindow::removePeer(const QString & ext)
{
	int i;
	for(i=0; i < m_peerlist.count(); i++) {
		if(ext == m_peerlist[i].ext()) {
			PeerWidget * peerwidget = m_peerlist[i].getWidget();
			m_layout->removeWidget( peerwidget );
			m_peerlist.removeAt(i);
			//delete peerwidget;
			peerwidget->deleteLater();
			return;
		}
	}
}

void SwitchBoardWindow::removePeers(void)
{
	int i;
	for(i=0; i < m_peerlist.count(); i++) {
		PeerWidget * peerwidget = m_peerlist[i].getWidget();
		m_layout->removeWidget( peerwidget );
		//		m_peerlist.removeAt(i);
		//delete peerwidget;
		peerwidget->deleteLater();
	}
	m_peerlist.clear();
	m_x = 0;
	m_y = 0;
	return;
}

int SwitchBoardWindow::width() const
{
	return m_width;
}

void SwitchBoardWindow::setWidth(int width)
{
	m_width = width;
}

