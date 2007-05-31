/* $Id$ */
#ifndef __PEERITEM_H__
#define __PEERITEM_H__
#include <QString>
#include <QObject>

class PeerWidget;

class Peer// : public QObject
{
public:
	//Peer( const QString & ext, QObject * parent = 0 );
	Peer( const QString & ext, const QString & name );
	Peer( const Peer & peer);
	const QString & ext() const { return m_ext; };
	const QString & name() const { return m_name; };
//	Peer & operator=(const Peer & peer);
	void setWidget(PeerWidget * widget) { m_peerwidget = widget; };
	PeerWidget * getWidget() { return m_peerwidget; };
	void updateStatus(const QString & imavail,
			  const QString & sipstatus,
			  const QString & vmstatus,
			  const QString & queuestatus);
	void updateChans(const QStringList & chanIds,
	                 const QStringList & chanStates,
	                 const QStringList & chanOthers);
private:
	QString m_ext;
	QString m_name;
	PeerWidget * m_peerwidget;
};


#endif

