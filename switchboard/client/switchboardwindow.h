#ifndef __SWITCHBOARDWINDOW_H__
#define __SWITCHBOARDWINDOW_H__
#include <QWidget>
#include <QList>
#include "peerwidget.h"
#include "peerslayout.h"
#include "peeritem.h"

class SwitchBoardEngine;
class QGridLayout;
class QMouseEvent;


/*! \brief Widget displaying Peers
 *
 * This widget use a PeersLayout to display Peers in a grid.
 */
class SwitchBoardWindow : public QWidget
{
	Q_OBJECT
public:
	//! Constructor
	SwitchBoardWindow( QWidget * parent = 0);
	//! Destructor
	virtual ~SwitchBoardWindow();
	void setEngine(SwitchBoardEngine *);
	int width() const;	//!< get width property
	void setWidth(int);	//!< set width property
	void saveSettings() const;
	void savePositions() const;
protected:
	void mousePressEvent(QMouseEvent *);	//!< Catch mouse press events
	void dragEnterEvent(QDragEnterEvent *);
	void dropEvent(QDropEvent *);
public slots:
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
	void removePeers(void);
private slots:
	void removePeerFromLayout(const QString &);
private:
	//QGridLayout * m_layout;
	PeersLayout * m_layout;	//!< Grid Layout for displaying peers
	QList<Peer> m_peerlist;	//!< Peer list
	SwitchBoardEngine * m_engine;	//!< engine to connect to peer widgets
	int m_width;	//!< width property
};

#endif

