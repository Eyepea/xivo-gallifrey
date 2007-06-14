/* $Id$ */
#ifndef __SEARCHPANEL_H__
#define __SEARCHPANEL_H__
#include <QWidget>
#include <QList>
#include "peeritem.h"

class QVBoxLayout;
class QLineEdit;
class SwitchBoardEngine;

/*! \brief search panel widget
 */
class SearchPanel : public QWidget
{
	Q_OBJECT
public:
	SearchPanel(QWidget * parent = 0);	//!< Constructor
	void setEngine(SwitchBoardEngine *);	//!< set m_engine
public slots:
	void affTextChanged(const QString &);
	void updatePeer(const QString & ext,
	                const QString & name,
			const QString & imavail,
			const QString & sipstatus,
			const QString & vmstatus,
			const QString & queuestatus,
			const QStringList & chanIds,
			const QStringList & chanStates,
			const QStringList & chanOthers);
	void removePeer(const QString & ext);
	void removePeers();
private:
	SwitchBoardEngine * m_engine;	//!< engine object reference
	QList<Peer> m_peerlist;	//!< Peer list
	QVBoxLayout * m_peerlayout;	//!< layout object
	QLineEdit * m_input;	//!< widget for search string input
};

#endif

