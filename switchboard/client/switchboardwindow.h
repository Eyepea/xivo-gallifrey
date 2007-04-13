#ifndef __SWITCHBOARDWINDOW_H__
#define __SWITCHBOARDWINDOW_H__
#include <QWidget>
#include <QList>
#include "peerwidget.h"
#include "peerslayout.h"

class SwitchBoardEngine;
class QGridLayout;
class QMouseEvent;

class Peer// : public QObject
{
public:
	//Peer( const QString & ext, QObject * parent = 0 );
	Peer( const QString & ext);
	Peer( const Peer & peer);
	const QString & ext() const { return m_ext; };
//	Peer & operator=(const Peer & peer);
	void setWidget(PeerWidget * widget) { m_peerwidget = widget; };
	PeerWidget * getWidget() { return m_peerwidget; };
	void updateStatus(const QString & status,
			  const QString & avail,
			  const QString & corrname);
private:
	QString m_ext;
	PeerWidget * m_peerwidget;
	//int m_x;
	//int m_y;
};

/*! \brief Widget displaying Peers
 *
 * This widget use a PeersLayout to display Peers in a grid.
 */
class SwitchBoardWindow : public QWidget
{
public:
	//! Constructor
	SwitchBoardWindow( QWidget * parent = 0);
	//! Destructor
	virtual ~SwitchBoardWindow();
	void setEngine(SwitchBoardEngine *);
	void updatePeer(const QString & ext,
	        const QString & name,
			const QString & status,
			const QString & avail,
			const QString & corrname);
	void removePeer(const QString & ext);
	void removePeers(void);
	int width() const;	//!< get width property
	void setWidth(int);	//!< set width property
	void saveSettings() const;
	void savePositions() const;
protected:
	void mousePressEvent(QMouseEvent *);	//!< Catch mouse press events
	void dragEnterEvent(QDragEnterEvent *);
	void dropEvent(QDropEvent *);
private:
	//QGridLayout * m_layout;
	PeersLayout * m_layout;	//!< Grid Layout for displaying peers
	QList<Peer> m_peerlist;	//!< Peer list
	SwitchBoardEngine * m_engine;	//!< engine to connect to peer widgets
	//int m_x;
	//int m_y;
	int m_width;	//!< width property
};

#endif

