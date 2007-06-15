/* $Id$ */
#ifndef __PEERCHANNEL_H__
#define __PEERCHANNEL_H__

#include <QObject>
#include <QString>

/*! \brief Channel associated to a peer
 *
 */
class PeerChannel : public QObject
{
	Q_OBJECT
public:
	PeerChannel(const QString &id, const QString & state,
	            const QString &otherPeer, QObject * parent=0);
	//! get m_otherPeer
	const QString & otherPeer() const { return m_otherPeer; };
public slots:
	void intercept();
	void hangUp();
	void transfer();
signals:
	void interceptChan(const QString &);	//!< intercept
	void hangUpChan(const QString &);		//!< hang up
	void transferChan(const QString &);		//!< transfer
private:
	QString m_id;			//!< Identification string of the channel
	QString m_state;		//!< State of the channel
	QString m_otherPeer;	//!< other side of the channel
};

#endif

