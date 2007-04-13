#include <QDebug>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QToolTip>
#include <QPoint>
#include <QSettings>
#include <QMouseEvent>
#include "switchboardwindow.h"
#include "switchboardengine.h"
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
			const QString & avail,
			const QString & corrname)
{
  //qDebug() << status << avail;
  if(avail == "available")
    m_peerwidget->setGreen(1);
  else if(avail == "away")
    m_peerwidget->setDarkGreen(1);
  else if(avail == "doesnotdisturb")
    m_peerwidget->setRed(1);
  else if(avail == "berightback")
    m_peerwidget->setOrange(1);
  else if(avail == "outtolunch")
	m_peerwidget->setYellow(1);
  else
  	m_peerwidget->setGray(1);

  if(status == "Ready")
    m_peerwidget->setGreen(0);
  else if(status == "Ringing")
    m_peerwidget->setCyan(0);
  else if(status == "Calling")
    m_peerwidget->setYellow(0);
  else if(status == "On the phone")
    m_peerwidget->setRed(0);
  else
    m_peerwidget->setGray(0);

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
	//m_layout = new QGridLayout(this);
	m_layout = new PeersLayout(this);
	//m_x = 0;
	//m_y = 0;
	QSettings settings;
	m_width = settings.value("display/width", 5).toInt();
	//m_layout->setRowStretch(99, 1);
	//m_layout->setColumnStretch(m_width, 1);
	setAcceptDrops(true);
}

/*!
 * Save the positions in the grid of the peer widgets.
 */
SwitchBoardWindow::~SwitchBoardWindow()
{
	qDebug() << "SwitchBoardWindow::~SwitchBoardWindow()";
	savePositions();
}

/*! \brief Save settings
 */
void SwitchBoardWindow::saveSettings() const
{
	QSettings settings;
	settings.setValue("display/width", m_width);
}

/*! \brief setter for m_engine
 *
 * set SwitchBoardEngine object to be used to connect to
 * peer object slot/signals.
 */
void SwitchBoardWindow::setEngine(SwitchBoardEngine * engine)
{
	m_engine = engine;
}

/*! \brief update or add a peer
 *
 * The peer with the ext extension is updated or added
 * to the list if it is not present.
 * The placement of the PeerWidget is restored from the settings.
 *
 * \sa removePeer
 */
void SwitchBoardWindow::updatePeer(const QString & ext,
                                   const QString & name,
                                   const QString & status,
                                   const QString & avail,
                                   const QString & corrname)
{
	int i;
	// first search in the peerlist
	for(i=0; i<m_peerlist.count(); i++)
	{
		//		qDebug() << i << m_peerlist[i].ext();
		if(ext == m_peerlist[i].ext())
		{
		  m_peerlist[i].updateStatus(status, avail, corrname);
			return;
		}
	}
	// if not found in the peerlist, create a new Peer
	QSettings settings;
	Peer peer(ext);
	PeerWidget * peerwidget = new PeerWidget(ext, name, this);
	connect( peerwidget, SIGNAL(originateCall(const QString&, const QString&)),
	         m_engine, SLOT(originateCall(const QString&, const QString&)) );
	connect( peerwidget, SIGNAL(transferCall(const QString&, const QString&)),
	         m_engine, SLOT(transferCall(const QString&, const QString&)) );
	QPoint pos = settings.value("layout/" + ext).toPoint();
	//qDebug() << " " << ext << " " << pos;
	m_layout->addWidget( peerwidget, pos );
	peer.setWidget(peerwidget);
	peer.updateStatus(status, avail, corrname);
	m_peerlist << peer;
}

/*! \brief remove a Peer
 *
 * Find the peer with extension ext and remove it from the list
 * and the widget.
 *
 * \sa updatePeer
 * \sa removePeers
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

/*! \brief remove all peers
 *
 * remove all peers and widget.
 *
 * \sa removePeer
 */
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
}

int SwitchBoardWindow::width() const
{
	return m_width;
}

void SwitchBoardWindow::setWidth(int width)
{
	m_width = width;
}

void SwitchBoardWindow::mousePressEvent(QMouseEvent * event)
{
	qDebug() << "SwitchBoardWindow::mousePressEvent" << event;
	qDebug() << "   " << event->x() << event->y() << event->pos();
	qDebug() << "   " << event->globalX() << event->globalY() << event->globalPos();
}

/*!
 * This method accept or reject the drag event.
 *
 * \sa dropEvent()
 */
void SwitchBoardWindow::dragEnterEvent(QDragEnterEvent * event)
{
	qDebug() << "SwitchBoardWindow::dragEnterEvent" << event;
	if(event->mimeData()->hasFormat("text/plain"))
		event->acceptProposedAction();
}

/*! \brief Receives drop events
 * 
 * This method recieve drop events. It is currently used to 
 * move PeerWidgets arount :)
 *
 * \sa dragEnterEvent()
 */
void SwitchBoardWindow::dropEvent(QDropEvent * event)
{
	int i;
	QString text = event->mimeData()->text();
	qDebug() << "SwitchBoardWindow::dropEvent" << event
	         << text;
	qDebug() << "  " << event->pos() << m_layout->getPosInGrid(event->pos());
	//
	for(i=0; i<m_peerlist.count(); i++)
	{
		if(text == m_peerlist[i].ext())
		{
			qDebug() << "   " << i;
			m_layout->setItemPosition(i, m_layout->getPosInGrid(event->pos()));
			updateGeometry();
			//update();
		}
	}
	event->acceptProposedAction();
}

/*! \brief Save the positions of Peer Widgets
 *
 * Save the positions of all Peer Widgets to the settings.
 */
void SwitchBoardWindow::savePositions() const
{
	int i;
	QSettings settings;
	for(i = 0; i < m_peerlist.size(); i++)
	{
		settings.setValue("layout/" + m_peerlist[i].ext(),
		                  m_layout->getItemPosition(i));
	}
}

