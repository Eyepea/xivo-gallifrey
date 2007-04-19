#ifndef __SEARCHPANEL_H__
#define __SEARCHPANEL_H__
#include <QWidget>
#include <QList>
#include "peeritem.h"

class QVBoxLayout;
class QLineEdit;
class SwitchBoardEngine;

class SearchPanel : public QWidget
{
	Q_OBJECT
public:
	SearchPanel(QWidget * parent = 0);
	void setEngine(SwitchBoardEngine *);
public slots:
	void affTextChanged(const QString &);
	void updatePeer(const QString & ext,
	                const QString & name,
					const QString & status,
					const QString & avail,
					const QString & corrname);
	void removePeer(const QString & ext);
	void removePeers();
private:
	SwitchBoardEngine * m_engine;
	QList<Peer> m_peerlist;	//!< Peer list
	QVBoxLayout * m_peerlayout;
	QLineEdit * m_input;
};

#endif

