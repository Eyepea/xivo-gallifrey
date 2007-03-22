#ifndef __SWITCHBOARDWINDOW_H__
#define __SWITCHBOARDWINDOW_H__
#include <QWidget>
#include <QList>
#include "peerwidget.h"

class SwitchBoardEngine;
class QGridLayout;

class Peer// : public QObject
{
public:
	//Peer( const QString & ext, QObject * parent = 0 );
	Peer( const QString & ext);
	Peer( const Peer & peer);
	const QString & ext() { return m_ext; };
//	Peer & operator=(const Peer & peer);
	void setWidget(PeerWidget * widget) { m_peerwidget = widget; };
	PeerWidget * getWidget() { return m_peerwidget; };
	void updateStatus(const QString & status);
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
	void setEngine(SwitchBoardEngine *);
	void updatePeer(const QString & ext,
	                const QString & status);
	void addPeer(const QString & ext,
		     const QString & status);
	void removePeer(const QString & ext);
	void removePeers(void);
private:
	QGridLayout * m_layout;
	QList<Peer> m_peerlist;
	SwitchBoardEngine * m_engine;
	int m_x;
	int m_y;
};

#endif

