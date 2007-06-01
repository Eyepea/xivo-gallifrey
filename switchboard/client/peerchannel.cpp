/* $Id$ */
#include <QDebug>
#include "peerchannel.h"

PeerChannel::PeerChannel(const QString &id, const QString &state,
                         const QString &otherPeer, QObject *parent)
: QObject(parent), m_id(id), m_state(state), m_otherPeer(otherPeer)
{
}

#if 0
PeerChannel::PeerChannel(const PeerChannel & other)
: QObject(other.parent())//bof...
{
	m_id = other.m_id;
	m_state = other.m_state;
	m_otherPeer = other.m_otherPeer;
}

PeerChannel& PeerChannel::operator=(const PeerChannel& other)
{
	m_id = other.m_id;
	m_state = other.m_state;
	m_otherPeer = other.m_otherPeer;
	return *this;
}
#endif

void PeerChannel::intercept()
{
	qDebug() << "PeerChanne::intercept()" << m_id;
	// emit a signal to be sent to the engine.
	interceptChan( m_id );
}

void PeerChannel::hangUp()
{
	qDebug() << "PeerChannel::hangUp()" << m_id;
	// TO BE IMPLEMENTED :)
}

