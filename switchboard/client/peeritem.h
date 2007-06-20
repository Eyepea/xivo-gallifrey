/* $Id$ */
#ifndef __PEERITEM_H__
#define __PEERITEM_H__
#include <QString>
#include <QObject>

class PeerWidget;

/*! \brief Peer object, linking to a PeerWidget
 */
class Peer// : public QObject
{
public:
	//Peer( const QString & ext, QObject * parent = 0 );
	Peer( const QString & ext, const QString & name );
	Peer( const Peer & peer);
	//! get m_ext
	const QString & ext() const { return m_ext; };
	//! get m_name
	const QString & name() const { return m_name; };
//	Peer & operator=(const Peer & peer);
	//! set m_peerwidget
	void setWidget(PeerWidget * widget) { m_peerwidget = widget; };
	//! get m_peerwidget
	PeerWidget * getWidget() { return m_peerwidget; };
	void updateStatus(const QString & imavail,
			  const QString & sipstatus,
			  const QString & vmstatus,
			  const QString & queuestatus);
	void updateChans(const QStringList & chanIds,
	                 const QStringList & chanStates,
	                 const QStringList & chanOthers);
	void updateName(const QString & newname);
private:
	QString m_ext;		//!< Extension
	QString m_name;		//!< Person name
	PeerWidget * m_peerwidget;	//!< related PeerWidget
};


#endif

