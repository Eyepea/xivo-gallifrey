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

class SwitchBoardWindow : public QWidget
{
public:
	SwitchBoardWindow( QWidget * parent = 0);
	virtual ~SwitchBoardWindow();
	void setEngine(SwitchBoardEngine *);
	void updatePeer(const QString & ext,
	        const QString & name,
			const QString & status,
			const QString & avail,
			const QString & corrname);
	void removePeer(const QString & ext);
	void removePeers(void);
	int width() const;
	void setWidth(int);
	void saveSettings() const;
	void savePositions() const;
protected:
	void mousePressEvent(QMouseEvent *);
	void dragEnterEvent(QDragEnterEvent *);
	void dropEvent(QDropEvent *);
private:
	//QGridLayout * m_layout;
	PeersLayout * m_layout;
	QList<Peer> m_peerlist;
	SwitchBoardEngine * m_engine;
	int m_x;
	int m_y;
	int m_width;
};

#endif

