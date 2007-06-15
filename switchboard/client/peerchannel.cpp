/* $Id$ */
#include <QDebug>
#include "peerchannel.h"

/*! \brief Constructor
 *
 * Just fill the members m_id, m_state and m_otherPeer.
 */
PeerChannel::PeerChannel(const QString &id, const QString &state,
                         const QString &otherPeer, QObject *parent)
: QObject(parent), m_id(id), m_state(state), m_otherPeer(otherPeer)
{
}

/*! \brief intercept the channel
 */
void PeerChannel::intercept()
{
	//qDebug() << "PeerChanne::intercept()" << m_id;
	// emit a signal to be sent to the engine.
	interceptChan( m_id );
}

/*! \brief hang up the channel
 */
void PeerChannel::hangUp()
{
	//qDebug() << "PeerChannel::hangUp()" << m_id;
	hangUpChan( m_id );
}

/*! \brief transfer this channel
 */
void PeerChannel::transfer()
{
	//qDebug() << "PeerChannel::transfer()" << m_id;
	transferChan( m_id );
}

