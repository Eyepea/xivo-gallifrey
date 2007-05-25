#include "peeritem.h"
#include "peerwidget.h"

Peer::Peer(const QString & ext, const QString & name)
: m_ext(ext), m_name(name)
{
}

Peer::Peer(const Peer & peer)
{
	m_ext = peer.m_ext;
	m_name = peer.m_name;
	m_peerwidget = peer.m_peerwidget;
}

void Peer::updateStatus(const QString & status,
			const QString & avail/*,
			const QString & corrname*/)
{
  //qDebug() << status << avail;
  if(avail == "available")
    m_peerwidget->setGreen(1);
  else if(avail == "away")
    m_peerwidget->setBlue(1);/*setDarkGreen(1);*/
  else if(avail == "donotdisturb")
    m_peerwidget->setRed(1);
  else if(avail == "berightback")
    m_peerwidget->setOrange(1);
  else if(avail == "outtolunch")
	m_peerwidget->setYellow(1);
  else
  	m_peerwidget->setGray(1);

  if(status == "Ready")
    m_peerwidget->setGreen(0);
  else if(status == "Ringing")
    m_peerwidget->setBlue(0);/*setCyan(0);*/
  else if(status == "Calling")
    m_peerwidget->setYellow(0);
  else if(status == "On the phone")
    m_peerwidget->setRed(0);
  else
    m_peerwidget->setGray(0);

//  if(corrname == "")
    m_peerwidget->setToolTip(status);
//  else
//    m_peerwidget->setToolTip(status + "\n" + corrname);
}

