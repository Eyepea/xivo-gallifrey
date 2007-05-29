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

void Peer::updateStatus(const QString & imavail,
			const QString & sipstatus,
			const QString & vmstatus,
			const QString & queuestatus)
{
	// TBD : tr()
	QString fortooltip = "SIP Presence: " + sipstatus + "\n"
		+ "Xivo Client: " + imavail;/* + "\n"
					       + "Voicemail Status: " + vmstatus + "\n"
					       + "Queues Status: " + queuestatus;*/
	//qDebug() << imavail << sipstatus;
	if(imavail == "available")
		m_peerwidget->setGreen(1);
	else if(imavail == "away")
		m_peerwidget->setBlue(1);/*setDarkGreen(1);*/
	else if(imavail == "donotdisturb")
		m_peerwidget->setRed(1);
	else if(imavail == "berightback")
		m_peerwidget->setOrange(1);
	else if(imavail == "outtolunch")
		m_peerwidget->setYellow(1);
	else
		m_peerwidget->setGray(1);

	if(sipstatus == "Ready")
		m_peerwidget->setGreen(0);
	else if(sipstatus == "Ringing")
		m_peerwidget->setBlue(0);/*setCyan(0);*/
	else if(sipstatus == "Calling")
		m_peerwidget->setYellow(0);
	else if(sipstatus == "On the phone")
		m_peerwidget->setRed(0);
	else
		m_peerwidget->setGray(0);
	
	m_peerwidget->setToolTip(fortooltip);

	//  if(corrname == "")
	//    m_peerwidget->setToolTip(status);
	//  else
	//    m_peerwidget->setToolTip(status + "\n" + corrname);
}

