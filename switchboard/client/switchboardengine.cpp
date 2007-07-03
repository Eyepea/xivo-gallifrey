/*
XIVO switchboard : 
Copyright (C) 2007  Proformatique

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/

/* $Id$
 * $Revision$
   $Date$
*/

#include <QDebug>
#include <QSettings>
#include <QTime>
#include <QStringList>
#include "switchboardengine.h"
#include "switchboardwindow.h"
#include "astchannel.h"
#include "logeltwidget.h"

/*! \brief Constructor.
 *
 * Construct the SwitchBoardEngine object and load settings.
 * The TcpSocket Object used to communicate with the server
 * is created and connected to the right slots/signals
 */
SwitchBoardEngine::SwitchBoardEngine(QObject * parent)
: QObject(parent)
{
	m_socket = new QTcpSocket(this);
	m_timer = -1;
	loadSettings();
/*  QTcpSocket signals :
      void connected ()
      void disconnected ()
      void error ( QAbstractSocket::SocketError socketError )
      void hostFound ()
      void stateChanged ( QAbstractSocket::SocketState socketState )
*/
/* Signals inherited from QIODevice :
      void aboutToClose ()
      void bytesWritten ( qint64 bytes )
      void readyRead ()
*/
	// Connect socket signals
	connect(m_socket, SIGNAL(connected()),
	        this, SLOT(socketConnected()));
	connect(m_socket, SIGNAL(disconnected()),
	        this, SLOT(socketDisconnected()));
	connect(m_socket, SIGNAL(hostFound()), this, SLOT(socketHostFound()));
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)),
	        this, SLOT(socketError(QAbstractSocket::SocketError)));
	connect(m_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
	        this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
	connect(m_socket, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));

	if(m_autoconnect)
		start();
}

/*!
 * Load Settings from the registery/configuration file
 */
void SwitchBoardEngine::loadSettings()
{
	QSettings settings;
	m_serverhost = settings.value("engine/serverhost").toString();
	m_sbport = settings.value("engine/serverport", 5003).toUInt();
	m_autoconnect = settings.value("engine/autoconnect", false).toBool();
	m_trytoreconnect = settings.value("engine/trytoreconnect", false).toBool();
	m_trytoreconnectinterval = settings.value("engine/trytoreconnectinterval", 20*1000).toUInt();
	m_asterisk = settings.value("engine/asterisk").toString();
	m_protocol = settings.value("engine/protocol").toString();
	//m_extension = settings.value("engine/extension").toString();
	m_userid = settings.value("engine/userid").toString();
}

/*!
 * Save Settings to the registery/configuration file
 */
void SwitchBoardEngine::saveSettings()
{
	QSettings settings;
	settings.setValue("engine/serverhost", m_serverhost);
	settings.setValue("engine/serverport", m_sbport);
	settings.setValue("engine/autoconnect", m_autoconnect);
	settings.setValue("engine/trytoreconnect", m_trytoreconnect);
	settings.setValue("engine/trytoreconnectinterval", m_trytoreconnectinterval);
	settings.setValue("engine/asterisk", m_asterisk);
	settings.setValue("engine/protocol", m_protocol);
	//settings.setValue("engine/extension", m_extension);
	settings.setValue("engine/userid", m_userid);
}

/* \brief set server address
 *
 * Set server host name and server port
 */
void SwitchBoardEngine::setAddress(const QString & host, quint16 port)
{
	m_serverhost = host;
	m_sbport = port;
}

/*! \brief Start the connection to the server
 */
void SwitchBoardEngine::start()
{
	connectSocket();
}

/*! \brief close the connection to the server
 */
void SwitchBoardEngine::stop()
{
	m_socket->disconnectFromHost();
}

/*! \brief initiate connection to the server
 */
void SwitchBoardEngine::connectSocket()
{
	m_socket->connectToHost(m_serverhost, m_sbport);
}

/*! \brief send a command to the server 
 * The m_pendingcommand is sent on the socket.
 *
 * \sa m_pendingcommand
 */
void SwitchBoardEngine::sendCommand()
{
	m_socket->write((m_pendingcommand + "\r\n"/*"\n"*/).toAscii());
	qDebug() << ">>>" << m_pendingcommand;
}

/*! \brief parse history command response
 *
 * parse the history command response from the server and
 * trigger the update of the call history panel.
 *
 * \sa Logwidget
 */
void SwitchBoardEngine::processHistory(const QStringList & histlist)
{
	int i;
	for(i=0; i+6<=histlist.size(); i+=6)
	{
		// DateTime; CallerID; duration; Status?; peer; IN/OUT
		//qDebug() << histlist[i+0] << histlist[i+1]
		//         << histlist[i+2] << histlist[i+3]
		//         << histlist[i+4] << histlist[i+5];
		QDateTime dt = QDateTime::fromString(histlist[i+0], Qt::ISODate);
		QString callerid = histlist[i+1];
		int duration = histlist[i+2].toInt();
		QString status = histlist[i+3];
		QString peer = histlist[i+4];
		LogEltWidget::Direction d;
		d = (histlist[i+5] == "IN") ? LogEltWidget::InCall : LogEltWidget::OutCall;
		//qDebug() << dt << callerid << duration << peer << d;
		updateLogEntry(dt, duration, peer, (int)d);
	}
}

/* Slots */

/*! \brief called when the socket is first connected
 */
void SwitchBoardEngine::socketConnected()
{
	qDebug() << "socketConnected()";
	started();
	stopTryAgainTimer();
	/* do the login/identification ? */
	m_pendingcommand = "login " + m_asterisk + " "
	                   + m_protocol + " " + m_userid;
					   // login <asterisk> <techno> <id>
	//m_pendingcommand = "callerids";
	sendCommand();
}

/*! \brief called when the socket is disconnected from the server
 */
void SwitchBoardEngine::socketDisconnected()
{
	qDebug() << "socketDisconnected()";
	stopped();
	emitTextMessage(tr("Connection lost with Presence Server"));
	startTryAgainTimer();
	//removePeers();
	//connectSocket();
}

/*! \brief cat host found socket signal
 *
 * Do nothing...
 */
void SwitchBoardEngine::socketHostFound()
{
	qDebug() << "socketHostFound()";
}

/*! \brief catch socket errors
 */
void SwitchBoardEngine::socketError(QAbstractSocket::SocketError socketError)
{
	qDebug() << "socketError(" << socketError << ")";
	switch(socketError)
	{
	case QAbstractSocket::ConnectionRefusedError:
		emitTextMessage(tr("Connection refused"));
		if(m_timer != -1)
		{
			killTimer(m_timer);
		 	m_timer = -1;
		}
		//m_timer = startTimer(2000);
		break;
	case QAbstractSocket::HostNotFoundError:
		emitTextMessage(tr("Host not found"));
		break;
	case QAbstractSocket::UnknownSocketError:
		emitTextMessage(tr("Unknown socket error"));
		break;
	default:
		break;
	}
}

/*! \brief receive signals of socket state change
 *
 * useless...
 */
void SwitchBoardEngine::socketStateChanged(QAbstractSocket::SocketState socketState)
{
	qDebug() << "socketStateChanged(" << socketState << ")";
	if(socketState == QAbstractSocket::ConnectedState) {
		if(m_timer != -1)
		{
			killTimer(m_timer);
			m_timer = -1;
		}
		//startTimer(3000);
	}
}

/*! \brief update Peers 
 *
 * update peers and calls
 */
void SwitchBoardEngine::updatePeers(const QStringList & liststatus)
{
	const int nfields0 = 11; // 0th order size (per-phone/line informations)
	const int nfields1 = 6;  // 1st order size (per-channel informations)
	QStringList chanIds;
	QStringList chanStates;
	QStringList chanOthers;

	// liststatus[0] is a dummy field, only used for debug on the daemon side
	// p/(asteriskid)/(context)/(protocol)/(phoneid)/(phonenum)
	
	if(liststatus.count() < 11)
	{
		// not valid
		qDebug() << "Bad data from the server :" << liststatus;
		return;
	}

	//<who>:<asterisk_id>:<tech(SIP/IAX/...)>:<phoneid>:<numero>:<contexte>:<dispo>:<etat SIP/XML>:<etat VM>:<etat Queues>: <nombre de liaisons>:
	QString context = liststatus[5];
	QString pname   = "p/" + liststatus[1] + "/" + context + "/"
		+ liststatus[2] + "/" + liststatus[3] + "/" + liststatus[4];
	QString InstMessAvail   = liststatus[6];
	QString SIPPresStatus   = liststatus[7];
	QString VoiceMailStatus = liststatus[8];
	QString QueueStatus     = liststatus[9];
	int nchans = liststatus[10].toInt();
	//QString pinfos = "";
	if(liststatus.size() == nfields0 + nfields1 * nchans) {
		for(int i = 0; i < nchans; i++) {
			//  <channel>:<etat du channel>:<nb de secondes dans cet etat>:<to/from>:<channel en liaison>:<numero en liaison>
			int refn = nfields0 + nfields1 * i;
			SIPPresStatus = liststatus[refn + 1];
			/*if(liststatus[3] == "114")
			{
				qDebug() << liststatus[refn] << liststatus[refn + 1]
				         << liststatus[refn + 2] << liststatus[refn + 3]
				         << liststatus[refn + 4] << liststatus[refn + 5];
			}*/
			chanIds << ("c/" + liststatus[1] + "/" + context + "/" + liststatus[refn]);
			chanStates << liststatus[refn + 1];
			chanOthers << liststatus[refn + 5];
			/*pinfos += liststatus[refn + 1] + " : " + liststatus[refn] + " "
				+ liststatus[refn + 2] + " " + liststatus[refn + 3] + " "
				+ liststatus[refn + 4] + " " + liststatus[refn + 5];*/
			updateCall("c/" + liststatus[1] + "/" + context + "/" + liststatus[refn],
				   liststatus[refn + 1],
				   liststatus[refn + 2].toInt(), liststatus[refn + 3],
				   liststatus[refn + 4], liststatus[refn + 5],
				   pname);
/*			if(i < nchans - 1)
				pinfos += "\n";*/
		}
	}

	//qDebug() << chanIds;
	//qDebug() << chanStates;
	//qDebug() << chanOthers;
	//qDebug() << "updatePeer" << pname << m_callerids[pname];
	updatePeer(pname, m_callerids[pname],
	           InstMessAvail, SIPPresStatus, VoiceMailStatus, QueueStatus,
	           chanIds, chanStates, chanOthers);
	// and my_context == context ???
	//if(liststatus[4] == m_extension)
	if(   (m_userid == liststatus[3])
	   && (m_dialcontext == liststatus[5]))
	{
		//qDebug() << "glop";
		updateMyCalls(chanIds, chanStates, chanOthers);
	}
}

/*! \brief update a caller id 
 */
void SwitchBoardEngine::updateCallerids(const QStringList & liststatus)
{
	QString pname = "p/" + liststatus[1] + "/" + liststatus[5] + "/"
		+ liststatus[2] + "/" + liststatus[3] + "/" + liststatus[4];
	QString pcid = liststatus[6];
	// liststatus[7] => group informations
	m_callerids[pname] = pcid;
	//qDebug() << "callerid[" << pname<< "]=" << pcid;
}

/*! \brief called when data are ready to be read on the socket.
 *
 * Read and process the data from the server.
 */
void SwitchBoardEngine::socketReadyRead()
{
	//	qDebug() << "socketReadyRead()";
	//QByteArray data = m_socket->readAll();
	//qDebug() << data;
	bool b = false;
	while(m_socket->canReadLine())
	{
		QByteArray data = m_socket->readLine();
		QString line = QString::fromUtf8(data);
		//qDebug() << "<==" << line;
		QStringList list = line.trimmed().split("=");
		//qDebug() << "<<<" << list.size() << list[0];
		if(list.size() == 2) {
			if(list[0] == QString("hints")) {
				QStringList listpeers = list[1].split(";");
				for(int i = 0 ; i < listpeers.size() - 1; i++) {
					QStringList liststatus = listpeers[i].split(":");
					updatePeers(liststatus);
				}
				callsUpdated();
				b = true;
				// TEST !
				//m_pendingcommand = "history obelisk/SIP/103 10";
				//m_pendingcommand = "history obelisk/SIP/103 3";
				//sendCommand();
			} else if(list[0] == "loginok") {
				QStringList params = list[1].split(";");
				if(params.size() > 1) {
					m_dialcontext = params[0];
					m_extension = params[1];
				}
				m_pendingcommand = "callerids";
				sendCommand();
			} else if(list[0] == QString("callerids")) {
				QStringList listpeers = list[1].split(";");
				for(int i = 0 ; i < listpeers.size() - 1; i++) {
					QStringList liststatus = listpeers[i].split(":");
					updateCallerids(liststatus);
				}
				//callsUpdated();
				m_pendingcommand = "hints";
				sendCommand();
				//socketConnected();
			} else if(list[0] == QString("update")) {
				QStringList liststatus = list[1].split(":");
				updatePeers(liststatus);
				callsUpdated();
			} else if(list[0] == QString("asterisk")) {
				QTime currentTime = QTime::currentTime();
				//QString currentTimeStr = currentTime.toString("hh:mm:ss");
				QStringList message = list[1].split("::");
				// message[0] : emitter name
				if(message.size() == 2) {
					emitTextMessage(message[0] + tr(" said : ") + message[1]);
				} else {
					emitTextMessage(tr("Unknown") + tr(" said : ") + list[1]);
				}

			} else if(list[0] == QString("peeradd")) {
				QStringList listpeers = list[1].split(";");
				for(int i = 0 ; i < listpeers.size() - 1; i++) {
					QStringList liststatus = listpeers[i].split(":");
					updatePeers(liststatus);
				}
			} else if(list[0] == QString("peerremove")) {
				QStringList listpeers = list[1].split(";");
				for(int i = 0 ; i < listpeers.size() - 1; i++) {
					QStringList liststatus = listpeers[i].split(":");
					QString toremove = "p/" + liststatus[1] + "/" + \
						liststatus[5] + "/" + liststatus[2] + "/" + \
						liststatus[3] + "/" + liststatus[4];
					removePeer(toremove);
				}
			} else if(list[0] == QString("history")) {
				processHistory(list[1].split(";"));
			} else if(list[0] == "directory-response") {
				directoryResponse(list[1]);
			}
		}
	}
	if(b) {
		//QTime currentTime = QTime::currentTime();
		//QString currentTimeStr = currentTime.toString("hh:mm:ss");
		//emitTextMessage(tr("Peers' status updated at ") + currentTimeStr);
		emitTextMessage(tr("Peers' status updated"));
	}
}

// void SwitchBoardEngine::timerEvent(QTimerEvent * event)
// {
// 	// event->timerId() !
// 	//	qDebug() << event;
// 	//	m_socket->connectToHost(m_host, m_port);
// 	//m_pendingcommand = "history obelisk/SIP/103 3";
// 	//sendCommand();
// }

/*! \brief send an originate command to the server
 */
void SwitchBoardEngine::originateCall(const QString & src, const QString & dst)
{
	qDebug() << "SwitchBoardEngine::originateCall()" << src << dst;
	QStringList dstlist = dst.split("/");
	if(dstlist.size()>5)
	{
		m_pendingcommand = "originate " + src + " " + dst;
		//		+ dstlist[0] + "/" + dstlist[1] + "/" + m_dialcontext + "/"
		//	+ dstlist[3] + "/" + dstlist[4] + "/" + dstlist[5];
	}
	else
	{
		m_pendingcommand = "originate " + src + " p/" + m_asterisk + "/"
		     + m_dialcontext + "/" + "/" + "/" + dst;
	}
	sendCommand();
}

/*! \brief dial (originate with known src)
 */
void SwitchBoardEngine::dialFullChannel(const QString & dst)
{
	qDebug() << "SwitchBoardEngine::dialFullChannel()" << dst;
	m_pendingcommand = "originate p/" +
		m_asterisk + "/" + m_dialcontext + "/" + m_protocol + "/" +
		m_userid + "/" + m_extension + " " + dst;
	sendCommand();
}

/*! \brief dial (originate with known src)
 */
void SwitchBoardEngine::dialExtension(const QString & dst)
{
	qDebug() << "SwitchBoardEngine::dialExtension()" << dst;
	m_pendingcommand = "originate p/" +
		m_asterisk + "/" + m_dialcontext + "/" + m_protocol + "/" + 
		m_userid + "/" + m_extension + " p/" +
		m_asterisk + "/" + m_dialcontext + "/" + "/" + "/" + dst;
	sendCommand();
}

/*! \brief send a transfer call command to the server
 */
void SwitchBoardEngine::transferCall(const QString & src, const QString & dst)
{
	qDebug() << "SwitchBoardEngine::transferCall()" << src << dst;
	QStringList dstlist = dst.split("/");
	if(dstlist.size() >= 6)
	{
		m_pendingcommand = "transfer " + src + " "
			+ dstlist[0] + "/" + dstlist[1] + "/" + m_dialcontext + "/"
			+ dstlist[3] + "/" + dstlist[4] + "/" + dstlist[5];
	}
	else
	{
		m_pendingcommand = "transfer " + src + " p/" + m_asterisk + "/"
		     + m_dialcontext + "/" + "/" + "/" + dst;
	}
	sendCommand();
}

/*! \brief intercept a call (a channel)
 *
 * The channel is transfered to "Me"
 *
 * \sa transferCall
 */
void SwitchBoardEngine::interceptCall(const QString & src)
{
	qDebug() << "SwitchBoardEngine::interceptCall()" << src;
	m_pendingcommand = "transfer " + src + " p/"
	        + m_asterisk + "/" + m_dialcontext + "/"
			+ m_protocol + "/" + "/" + m_extension; 
	sendCommand();
}

/*! \brief hang up a channel
 *
 * send a hang up command to the server
 */
void SwitchBoardEngine::hangUp(const QString & channel)
{
	qDebug() << "SwitchBoardEngine::hangUp()" << channel;
	m_pendingcommand = "hangup " + channel;
	sendCommand();
}

/*! \brief send the directory search command to the server
 *
 * \sa directoryResponse()
 */
void SwitchBoardEngine::searchDirectory(const QString & text)
{
	qDebug() << "SwitchBoardEngine::searchDirectory()" << text;
	m_pendingcommand = "directory-search " + text;
	sendCommand();
}

/*! \brief ask history for an extension 
 */
void SwitchBoardEngine::requestHistory(const QString & peer, int mode)
{
	/* mode = 0 : Out calls
	 * mode = 1 : In calls
	 * mode = 2 : Missed calls */
	//qDebug() << "SwitchBoardEngine::requestHistory()" << peer;
	if(mode >= 0) {
		m_pendingcommand = "history " + peer + " 10 " + QString::number(mode);
		sendCommand();
	}
}

/*! \brief get server host */
const QString & SwitchBoardEngine::host() const
{
	return m_serverhost;
}

/*! \brief get server port */
quint16 SwitchBoardEngine::sbport() const
{
	return m_sbport;
}

/*! \brief implement timer event
 *
 * does nothing
 */
void SwitchBoardEngine::timerEvent(QTimerEvent * event)
{
	int timerId = event->timerId();
	qDebug() << "SwitchBoardEngine::timerEvent() timerId=" << timerId;
	if(timerId == m_try_timerid) {
		emitTextMessage(tr("Attempting to reconnect to server"));
		start();
		event->accept();
	} else {
		event->ignore();
	}
	return;
}

void SwitchBoardEngine::setTrytoreconnect(bool b)
{
	m_trytoreconnect = b;
}

bool SwitchBoardEngine::trytoreconnect() const
{
	return m_trytoreconnect;
}

void SwitchBoardEngine::stopTryAgainTimer()
{
	if( m_try_timerid > 0 )
	{
		killTimer(m_try_timerid);
		m_try_timerid = 0;
	}
}

void SwitchBoardEngine::startTryAgainTimer()
{
	if( m_try_timerid == 0 && m_trytoreconnect )
	{
		m_try_timerid = startTimer(m_trytoreconnectinterval);
	}
}

uint SwitchBoardEngine::trytoreconnectinterval() const
{
	return m_trytoreconnectinterval;
}

/*!
 * Setter for property m_trytoreconnectinterval
 * Restart timer if the value changed.
 *
 * \sa trytoreconnectinterval
 */
void SwitchBoardEngine::setTrytoreconnectinterval(uint i)
{
	if( m_trytoreconnectinterval != i )
	{
		m_trytoreconnectinterval = i;
		if(m_try_timerid > 0)
		{
			killTimer(m_try_timerid);
			m_try_timerid = startTimer(m_trytoreconnectinterval);
		}
	}
}

