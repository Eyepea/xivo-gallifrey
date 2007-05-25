#include <QDebug>
#include "peerchannel.h"

PeerChannel::PeerChannel(const QString &id, const QString &state,
                         const QString &otherPeer, QObject *parent)
: QObject(parent), m_id(id), m_state(state), m_otherPeer(otherPeer)
{
}

PeerChannel::PeerChannel(const PeerChannel & other)
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

void PeerChannel::intercept()
{
	qDebug() << "PeerChanne::intercept()" << m_id;
}

