/*
XIVO customer information client : popup profile for incoming calls
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

/* $Id$ */

#include <QDebug>
#include <QTcpSocket>
#include <QTime>
#include <QThread>
#include <QMessageBox>
#include <QSettings>
#include <QTimerEvent>
#include "engine.h"
#include "popup.h"
#include "logeltwidget.h"

const int REQUIRED_SERVER_VERSION = 1165;

/*!
 * This constructor initialize the UDP socket and
 * the TCP listening socket.
 * It also connects signals with the right slots.
 */
Engine::Engine(QObject *parent)
: QObject(parent),
  m_serverip(""), m_loginport(0), m_asterisk(""), m_protocol(""), m_userid(""), m_passwd(""),
  m_listenport(0), m_sessionid(""), m_state(ENotLogged),
  m_pendingkeepalivemsg(0)
{
	m_ka_timerid = 0;
	m_try_timerid = 0;
	loadSettings();
	setAvailState(m_availstate);
	
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

	// init listen socket for profile push
	connect( &m_loginsocket, SIGNAL(connected()),
	         this, SLOT(identifyToTheServer()) );
	connect( &m_loginsocket, SIGNAL(readyRead()),
	         this, SLOT(processLoginDialog()) );
	connect( &m_loginsocket, SIGNAL(hostFound()),
	         this, SLOT(serverHostFound()) );
	if(!m_tcpmode)
		initListenSocket();
	connect( &m_listensocket, SIGNAL(newConnection()),
	         this, SLOT(handleProfilePush()) );
	// init UDP socket used for keep alive
	//if(!m_tcpmode)
		m_udpsocket.bind();
	connect( &m_udpsocket, SIGNAL(readyRead()),
             this, SLOT(readKeepLoginAliveDatagrams()) );
	
	if(m_autoconnect)
		start();
}

/*!
 * Load settings using QSettings class which is portable.
 * Use default values when settings are not found.
 */
void Engine::loadSettings()
{
	QSettings settings;
	m_serverip = settings.value("engine/serverhost").toString();
	m_loginport = settings.value("engine/serverport", 5000).toUInt();
	m_asterisk = settings.value("engine/serverastid").toString();
	m_protocol = settings.value("engine/protocol").toString();
	m_userid = settings.value("engine/userid").toString();
	m_passwd = settings.value("engine/passwd").toString();
	m_autoconnect = settings.value("engine/autoconnect", false).toBool();
	m_trytoreconnect = settings.value("engine/trytoreconnect", false).toBool();
	m_trytoreconnectinterval = settings.value("engine/trytoreconnectinterval", 20*1000).toUInt();
	m_tcpmode = settings.value("engine/tcpmode", false).toBool();
	m_availstate = settings.value("engine/availstate", "available").toString();
	m_keepaliveinterval = settings.value("engine/keepaliveinterval", 20*1000).toUInt();
}

/*!
 * Save settings using QSettings class which is portable.
 */
void Engine::saveSettings()
{
	QSettings settings;
	settings.setValue("engine/serverhost", m_serverip);
	settings.setValue("engine/serverport", m_loginport);
	settings.setValue("engine/serverastid", m_asterisk);
	settings.setValue("engine/protocol", m_protocol);
	settings.setValue("engine/userid", m_userid);
	settings.setValue("engine/passwd", m_passwd);
	settings.setValue("engine/autoconnect", m_autoconnect);
	settings.setValue("engine/trytoreconnect", m_trytoreconnect);
	settings.setValue("engine/trytoreconnectinterval", m_trytoreconnectinterval);
	settings.setValue("engine/keepaliveinterval", m_keepaliveinterval);
	settings.setValue("engine/tcpmode", m_tcpmode);
}

void Engine::initListenSocket()
{
	if (!m_listensocket.listen())
	{
		QMessageBox::critical(NULL, tr("Critical error"),
		                            tr("Unable to start the server: %1.")
		                            .arg(m_listensocket.errorString()));
		return;
	}
	m_listenport = m_listensocket.serverPort();
}

/*!
 * This method start the login process by connection
 * to the server.
 */
void Engine::start()
{
	qDebug() << "Engine::start()";
	m_loginsocket.abort();
	m_loginsocket.connectToHost(m_serverip, m_loginport);
}

/*!
 * This method disconnect the engine from the the server
 */
void Engine::stop()
{
	qDebug() << "Engine::stop()";
	QString outline = "STOP ";
	outline.append(m_asterisk + "/" + m_protocol + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append("\r\n");

	m_udpsocket.writeDatagram( outline.toAscii(),
				   m_serveraddress, m_loginport + 1 );
	stopKeepAliveTimer();
	stopTryAgainTimer();
	setState(ENotLogged);
}

/*!
 * set the availability state and call keepLoginAlive() if needed
 *
 * \sa setAvailable()
 * \sa setAway()
 * \sa setBeRightBack()
 * \sa setOutToLunch()
 * \sa setDoNotDisturb()
 */
void Engine::setAvailState(const QString & newstate)
{
	if(m_availstate != newstate)
	{
		QSettings settings;
		m_availstate = newstate;
		settings.setValue("engine/availstate", m_availstate);
		keepLoginAlive();
	}
}

void Engine::setAvailable()
{
	//qDebug() << "setAvailable()";
	setAvailState("available");
}

void Engine::setAway()
{
	//qDebug() << "setAway()";
	setAvailState("away");
}

void Engine::setBeRightBack()
{
	setAvailState("berightback");
}

void Engine::setOutToLunch()
{
	setAvailState("outtolunch");
}

void Engine::setDoNotDisturb()
{
	//qDebug() << "setDoNotDistrurb()";
	setAvailState("donotdisturb");
}

void Engine::searchDirectory(const QString & text)
{
	qDebug() << "Engine::searchDirectory()";
	QString outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" DIRECTORY ");
	outline.append(text);
	outline.append("\r\n");
	qDebug() << outline;
	m_udpsocket.writeDatagram( outline.toAscii(),
				   m_serveraddress, m_loginport + 1 );
}

/*! \brief ask history for an extension 
 */
void Engine::requestHistory(const QString & peer, int mode)
{
	/* mode = 0 : Out calls
	 * mode = 1 : In calls
	 * mode = 2 : Missed calls */
	if(mode >= 0) {
		qDebug() << "Engine::requestHistory()";
		QString outline = "COMMAND ";
		outline.append(m_asterisk + "/" + m_protocol + m_userid);
		outline.append(" SESSIONID ");
		outline.append(m_sessionid);
		outline.append(" HISTORY ");
		outline.append(peer + " 10 " + QString::number(mode));
		outline.append("\r\n");
		qDebug() << outline;
		m_udpsocket.writeDatagram( outline.toAscii(),
					   m_serveraddress, m_loginport + 1 );
	}
}

/*! \brief ask for Peer's statuses
 */
void Engine::requestPeers(void)
{
	qDebug() << "Engine::requestPeers()";
	QString outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" PEERS ");
	outline.append("a b c");
	outline.append("\r\n");
	qDebug() << outline;
	m_udpsocket.writeDatagram( outline.toAscii(),
				   m_serveraddress, m_loginport + 1 );
}

void Engine::dialExtension(const QString & dst)
{
	qDebug() << "Engine::dialExtension()";
	QString outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" DIAL ");
	outline.append(m_asterisk + "/" + m_protocol + "/" +
		       m_userid + "/" + m_dialcontext + " " + dst);
	outline.append("\r\n");
	qDebug() << outline;
	m_udpsocket.writeDatagram( outline.toAscii(),
				   m_serveraddress, m_loginport + 1 );
}

void Engine::setVoiceMail(bool b)
{
	QString outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" FEATURES PUT VM ");
	outline.append(b?"1":"0");
	outline.append("\r\n");
	qDebug() << outline;
	m_udpsocket.writeDatagram( outline.toAscii(),
				   m_serveraddress, m_loginport + 1 );
}

void Engine::setCallRecording(bool b)
{
	QString outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" FEATURES PUT Record ");
	outline.append(b?"1":"0");
	outline.append("\r\n");
	qDebug() << outline;
	m_udpsocket.writeDatagram( outline.toAscii(),
				   m_serveraddress, m_loginport + 1 );
}

void Engine::setCallFiltering(bool b)
{
	QString outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" FEATURES PUT Screen ");
	outline.append(b?"1":"0");
	outline.append("\r\n");
	qDebug() << outline;
	m_udpsocket.writeDatagram( outline.toAscii(),
				   m_serveraddress, m_loginport + 1 );
}

void Engine::setDnd(bool b)
{
	QString outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" FEATURES PUT DND ");
	outline.append(b?"1":"0");
	outline.append("\r\n");
	qDebug() << outline;
	m_udpsocket.writeDatagram( outline.toAscii(),
				   m_serveraddress, m_loginport + 1 );
}

void Engine::setForwardOnUnavailable(bool b, const QString & dst)
{
	QString outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" FEATURES PUT FWD/RNA/Status ");
	outline.append(b?"1":"0");
	outline.append("\r\n");
	qDebug() << outline;
	m_udpsocket.writeDatagram( outline.toAscii(),
				   m_serveraddress, m_loginport + 1 );
	outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" FEATURES PUT FWD/RNA/Number ");
	outline.append(dst);
	outline.append("\r\n");
	qDebug() << outline;
	m_udpsocket.writeDatagram( outline.toAscii(),
				   m_serveraddress, m_loginport + 1 );
}

void Engine::setForwardOnBusy(bool b, const QString & dst)
{
	QString outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" FEATURES PUT FWD/Busy/Status ");
	outline.append(b?"1":"0");
	outline.append("\r\n");
	qDebug() << outline;
	m_udpsocket.writeDatagram( outline.toAscii(),
				   m_serveraddress, m_loginport + 1 );
	outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" FEATURES PUT FWD/Busy/Number ");
	outline.append(dst);
	outline.append("\r\n");
	qDebug() << outline;
	m_udpsocket.writeDatagram( outline.toAscii(),
				   m_serveraddress, m_loginport + 1 );
}

void Engine::setUncondForward(bool b, const QString & dst)
{
	QString outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" FEATURES PUT FWD/Unc/Status ");
	outline.append(b?"1":"0");
	outline.append("\r\n");
	qDebug() << outline;
	m_udpsocket.writeDatagram( outline.toAscii(),
				   m_serveraddress, m_loginport + 1 );
	outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" FEATURES PUT FWD/Unc/Number ");
	outline.append(dst);
	outline.append("\r\n");
	qDebug() << outline;
	m_udpsocket.writeDatagram( outline.toAscii(),
				   m_serveraddress, m_loginport + 1 );
}

void Engine::askFeatures()
{
	QString outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" FEATURES GET\r\n");
	qDebug() << outline;
	m_udpsocket.writeDatagram( outline.toAscii(),
				   m_serveraddress, m_loginport + 1 );
}

void Engine::askPeers()
{
	QString outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" PEERS\r\n");
	qDebug() << outline;
	m_udpsocket.writeDatagram( outline.toAscii(),
				   m_serveraddress, m_loginport + 1 );
}

void Engine::askCallerIds()
{
	QString outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" CALLERIDS\r\n");
	qDebug() << outline;
	m_udpsocket.writeDatagram( outline.toAscii(),
				   m_serveraddress, m_loginport + 1 );
}

// === Getter and Setters ===
const QString & Engine::serverip() const
{
	return m_serverip;
}

const QString & Engine::serverast() const
{
	return m_asterisk;
}

void Engine::setServerip(const QString & serverip)
{
	m_serverip = serverip;
}

void Engine::setServerAst(const QString & serverast)
{
	m_asterisk = serverast;
}

ushort Engine::serverport() const
{
	return m_loginport;
}

void Engine::setServerport(ushort port)
{
	//qDebug( "Engine::setServerport(%hu)", port);
	m_loginport = port;
}

const QString & Engine::userid() const
{
	return m_userid;
}

void Engine::setUserId(const QString & userid)
{
	m_userid = userid;
}

const QString & Engine::protocol() const
{
	return m_protocol;
}

void Engine::setProtocol(const QString & protocol)
{
	m_protocol = protocol;
}

const QString & Engine::passwd() const
{
	return m_passwd;
}

void Engine::setPasswd(const QString & passwd)
{
	m_passwd = passwd;
}

void Engine::setAutoconnect(bool b)
{
	m_autoconnect = b;
}

bool Engine::autoconnect() const
{
	return m_autoconnect;
}

void Engine::setTrytoreconnect(bool b)
{
	m_trytoreconnect = b;
}

bool Engine::trytoreconnect() const
{
	return m_trytoreconnect;
}

uint Engine::keepaliveinterval() const
{
	return m_keepaliveinterval;
}

/*!
 * Setter for the m_keepaliveinterval property.
 * if the value is changed, existing timer is restarted.
 *
 * \sa keepaliveinterval
 */
void Engine::setKeepaliveinterval(uint i)
{
	if(i != m_keepaliveinterval)
	{
		m_keepaliveinterval = i;
		if(m_ka_timerid > 0)
		{
			killTimer(m_ka_timerid);
			m_ka_timerid = startTimer(m_keepaliveinterval);
		}
	}
}

uint Engine::trytoreconnectinterval() const
{
	return m_trytoreconnectinterval;
}

/*!
 * Setter for property m_trytoreconnectinterval
 * Restart timer if the value changed.
 *
 * \sa trytoreconnectinterval
 */
void Engine::setTrytoreconnectinterval(uint i)
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

/*!
 * setter for the m_state property.
 * If the state is becoming ELogged, the
 * signal logged() is thrown.
 * If the state is becoming ENotLogged, the
 * signal delogged() is thrown.
 */
void Engine::setState(EngineState state)
{
	if(state != m_state)
	{
		m_state = state;
		if(state == ELogged)
		{
			stopTryAgainTimer();
			logged();
		}
		else if(state == ENotLogged)
			delogged();
	}
}

/*!
 * Perform the first login step once the TCP connection is established.
 */
void Engine::identifyToTheServer()
{
	qDebug() << "Engine::identifyToTheServer()" << m_loginsocket.peerAddress();
	m_serveraddress = m_loginsocket.peerAddress();
	QString outline = "LOGIN ";
	outline.append(m_asterisk);
	outline.append("/");
	outline.append(m_protocol);
	outline.append(m_userid);
#if defined(Q_WS_X11)
	outline.append(" XC@X11");
#elif defined(Q_WS_WIN)
	outline.append(" XC@WIN");
#elif defined(Q_WS_MAC)
	outline.append(" XC@MAC");
#else
	outline.append(" XC@unknown");
#endif
	outline.append("\r\n");
	m_loginsocket.write(outline.toAscii());
	m_loginsocket.flush();
	qDebug() << outline;
}

/*!
 * Perform the following of the login process after identifyToTheServer()
 * made the first step.
 * Theses steps are : sending the password, sending the port,
 *   just reading the session id from server response.
 * The state is changed accordingly.
 */
void Engine::processLoginDialog()
{
	char buffer[256];
	int len;
	qDebug() << "Engine::processLoginDialog()";
	if(m_tcpmode && (m_state == ELogged))
	{
		Popup * popup = new Popup(&m_loginsocket, m_sessionid);
		connect( popup, SIGNAL(destroyed(QObject *)),
		         this, SLOT(popupDestroyed(QObject *)) );
		connect( popup, SIGNAL(wantsToBeShown(Popup *)),
				 this, SLOT(profileToBeShown(Popup *)) );
		popup->streamNewData();
		return;
	}
	if(!m_loginsocket.canReadLine())
	{
		qDebug() << "no line ready to be read";
		return;
	}
	len = m_loginsocket.readLine(buffer, sizeof(buffer));
	if(len<0)
	{
		qDebug() << "readLine() returned -1, closing socket";
		m_loginsocket.close();
		setState(ENotLogged);
		return;
	}
	QString readLine = QString::fromAscii(buffer);
	qDebug() << ">>>" << readLine.trimmed();
	QString outline;
	if(readLine.startsWith("Send PASS"))
	{
		outline = "PASS ";
		outline.append(m_passwd);
		outline.append("\r\n");
	}
	else if(readLine.startsWith("Send PORT"))
	{
		if(m_tcpmode)
		{
			outline = "TCPMODE\r\n";
		}
		else
		{
			outline = "PORT ";
			outline.append(QString::number(m_listenport));
			outline.append("\r\n");
		}
	}
	else if(readLine.startsWith("Send STATE"))
	{
		outline = "STATE ";
		outline.append(m_availstate);
		outline.append("\r\n");
	}
	else if(readLine.startsWith("OK SESSIONID"))
	{
		readLine.remove(QChar('\r')).remove(QChar('\n'));
		QStringList sessionResp = readLine.split(" ");
		if(sessionResp.size() > 2)
			m_sessionid = sessionResp[2];
		if(sessionResp.size() > 3)
			m_dialcontext = sessionResp[3];
		if(sessionResp.size() > 4)
			m_capabilities = sessionResp[4];
		m_version = -1;
		if(sessionResp.size() > 5)
			m_version = sessionResp[5].toInt();
		if(!m_tcpmode)
			m_loginsocket.close();
		if(m_version < REQUIRED_SERVER_VERSION) {
			qDebug() << "Your server version is" << m_version << "which is too old. The required one is at least :" << REQUIRED_SERVER_VERSION;
			m_loginsocket.close();
			stop();
			return;
		}
		setState(ELogged);
		// start the keepalive timer
		m_ka_timerid = startTimer(m_keepaliveinterval);
		return;
	}
	else
	{
		readLine.remove(QChar('\r')).remove(QChar('\n'));
		qDebug() << "Response from server not recognized, closing" << readLine;
		m_loginsocket.close();
		setState(ENotLogged);
		return;
	}
	qDebug() << outline;
	m_loginsocket.write(outline.toAscii());
	m_loginsocket.flush();
}

/*!
 * This slot is connected to the hostFound() signal of the
 * m_loginsocket
 */
void Engine::serverHostFound()
{
	qDebug() << "Engine::serverHostFound()" << m_loginsocket.peerAddress();
}

/*!
 * This slot method is called when a pending connection is
 * waiting on the m_listensocket.
 * It processes the incoming data and create a popup to display it.
 */
void Engine::handleProfilePush()
{
	qDebug( "Engine::handleProfilePush()" );
	QTcpSocket *connection = m_listensocket.nextPendingConnection();
	connect( connection, SIGNAL(disconnected()),
	         connection, SLOT(deleteLater()));
	// signals sur la socket : connected() disconnected()
	// error() hostFound() stateChanged()
	// iodevice : readyRead() aboutToClose() bytesWritten()
	//
	qDebug() << connection->peerAddress().toString() << connection->peerPort();
	// Get Data and Popup the profile if ok
	Popup * popup = new Popup(connection, m_sessionid);
	connect( popup, SIGNAL(destroyed(QObject *)),
	         this, SLOT(popupDestroyed(QObject *)) );
	connect( popup, SIGNAL(wantsToBeShown(Popup *)),
	         this, SLOT(profileToBeShown(Popup *)) );
}

void Engine::popupDestroyed(QObject * obj)
{
	qDebug() << "Popup destroyed" << obj;
	//qDebug() << "========================";
	//obj->dumpObjectTree();
	//qDebug() << "========================";
}

void Engine::profileToBeShown(Popup * popup)
{
	newProfile( popup );
}

/*!
 * Send a keep alive message to the login server.
 * The message is sent in a datagram through m_udpsocket
 */ 
void Engine::keepLoginAlive()
{
	// got to disconnected state if more than xx keepalive messages
	// have been left without response.
	if(m_pendingkeepalivemsg > 1)
	{
		qDebug() << "m_pendingkeepalivemsg" << m_pendingkeepalivemsg;
		stopKeepAliveTimer();
		setState(ENotLogged);
		m_pendingkeepalivemsg = 0;
		startTryAgainTimer();
		return;
	}
	QString outline = "ALIVE ";
	outline.append(m_asterisk + "/" + m_protocol + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" STATE ");
	outline.append(m_availstate);
	outline.append("\r\n");
	qDebug() <<  "Engine::keepLoginAlive()" << outline;
	m_udpsocket.writeDatagram( outline.toAscii(),
	                           m_serveraddress, m_loginport + 1 );
	m_pendingkeepalivemsg++;
	// if the last keepalive msg has not been answered, send this one
	// twice
	if(m_pendingkeepalivemsg > 1)
	{
		m_udpsocket.writeDatagram( outline.toAscii(),
	    	                       m_serveraddress, m_loginport + 1 );
		m_pendingkeepalivemsg++;
	}
}

/*!
 * Send a keep alive message to the login server.
 * The message is sent in a datagram through m_udpsocket
 */ 
void Engine::sendMessage(const QString & txt)
{
	if(m_pendingkeepalivemsg > 1)
	{
		qDebug() << "m_pendingkeepalivemsg" << m_pendingkeepalivemsg;
		stopKeepAliveTimer();
		setState(ENotLogged);
		m_pendingkeepalivemsg = 0;
		startTryAgainTimer();
		return;
	}
	QString outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" MESSAGE ");
	outline.append(txt);
	outline.append("\r\n");
	qDebug() <<  "Engine::sendMessage()" << outline;
	m_udpsocket.writeDatagram( outline.toAscii(),
	                           m_serveraddress, m_loginport + 1 );
	m_pendingkeepalivemsg++;
}

void Engine::initFeatureFields(const QString & field, const QString & value)
{
	//	qDebug() << field << value;
	if(field == "VM")
		voiceMailChanged(value == "1");
	else if(field == "DND")
		dndChanged(value == "1");
	else if(field == "Screen")
		callFilteringChanged(value == "1");
	else if(field == "Record")
		callRecordingChanged(value == "1");
	else if(field == "FWD/Unc")
		uncondForwardChanged(value.split(":")[0] == "1", value.split(":")[1]);
	else if(field == "FWD/Unc/Status")
		uncondForwardChanged(value == "1");
	else if(field == "FWD/Unc/Number")
		uncondForwardChanged(value);
	else if(field == "FWD/Busy")
		forwardOnBusyChanged(value.split(":")[0] == "1", value.split(":")[1]);
	else if(field == "FWD/Busy/Status")
		forwardOnBusyChanged(value == "1");
	else if(field == "FWD/Busy/Number")
		forwardOnBusyChanged(value);
	else if(field == "FWD/RNA")
		forwardOnUnavailableChanged(value.split(":")[0] == "1", value.split(":")[1]);
	else if(field == "FWD/RNA/Status")
		forwardOnUnavailableChanged(value == "1");
	else if(field == "FWD/RNA/Number")
		forwardOnUnavailableChanged(value);
}

/*! \brief update a caller id 
 */
void Engine::updateCallerids(const QStringList & liststatus)
{
	QString pname = "p/" + liststatus[1] + "/" + liststatus[5] + "/"
		+ liststatus[2] + "/" + liststatus[3] + "/" + liststatus[4];
	QString pcid = liststatus[6];
	// liststatus[7] => group informations
	m_callerids[pname] = pcid;
}

/*! \brief update Peers 
 *
 * update peers and calls
 */
void Engine::updatePeers(const QStringList & liststatus)
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

	if(liststatus.size() == nfields0 + nfields1 * nchans) {
		for(int i = 0; i < nchans; i++) {
			//  <channel>:<etat du channel>:<nb de secondes dans cet etat>:<to/from>:<channel en liaison>:<numero en liaison>
			int refn = nfields0 + nfields1 * i;
			QString displayedNum;
			
			SIPPresStatus = liststatus[refn + 1];
			chanIds << ("c/" + liststatus[1] + "/" + context + "/" + liststatus[refn]);
			chanStates << liststatus[refn + 1];
			if((liststatus[refn + 5] == "") ||
			   (liststatus[refn + 5] == "<Unknown>") ||
			   (liststatus[refn + 5] == "<unknown>") ||
			   (liststatus[refn + 5] == "anonymous") ||
			   (liststatus[refn + 5] == "(null)"))
				displayedNum = tr("Unknown Number");
			else
				displayedNum = liststatus[refn + 5];

			chanOthers << displayedNum;
// 			updateCall("c/" + liststatus[1] + "/" + context + "/" + liststatus[refn],
// 				   liststatus[refn + 1],
// 				   liststatus[refn + 2].toInt(), liststatus[refn + 3],
// 				   liststatus[refn + 4], displayedNum,
// 				   pname);
		}
	}

	updatePeer(pname, m_callerids[pname],
	           InstMessAvail, SIPPresStatus, VoiceMailStatus, QueueStatus,
	           chanIds, chanStates, chanOthers);

	//  	if(   (m_userid == liststatus[3]) && (m_dialcontext == liststatus[5])) {
	// 		updateMyCalls(chanIds, chanStates, chanOthers);
	// 	}
}

/*!
 * Process incoming UDP datagrams which are likely to be 
 * response from keep alive messages.
 * If the response is not 'OK', goes to
 * the "not connected" state.
 */
void Engine::readKeepLoginAliveDatagrams()
{
	char buffer[2048];
	int len;
	qDebug() << "Engine::readKeepLoginAliveDatagrams()";
	while( m_udpsocket.hasPendingDatagrams() )
	{
		len = m_udpsocket.readDatagram(buffer, sizeof(buffer)-1);
		if(len == 0)
			continue;
		buffer[len] = '\0';
		//		qDebug() << len << ":" << buffer;
		QStringList qsl = QString::fromUtf8(buffer).trimmed().split(" ");
		QString reply = qsl[0];
		
		if(reply == "DISC") {
			stopKeepAliveTimer();
			setState(ENotLogged);
			startTryAgainTimer();
			qDebug() << qsl; //reply;
		} else if(reply == "HISTORY") {
			QStringList list = QString::fromUtf8(buffer).trimmed().split("=");
			processHistory(list[1].split(";"));
		} else if(reply == "DIRECTORY") {
			QStringList list = QString::fromUtf8(buffer).trimmed().split("=");
			directoryResponse(list[1]);
		} else if(reply == "FEATURES") {
			if((qsl.size() == 4) && (qsl[1] == "UPDATE"))
				initFeatureFields(qsl[2], qsl[3]);
			else {
				QStringList list = qsl[1].split(";");
				//				qDebug() << list.size();
				if(list.size() > 1) {
					for(int i=0; i<list.size()-1; i+=2) {
						initFeatureFields(list[i], list[i+1]);
					}
				}
			}
			qDebug() << qsl; //reply;
		} else if(reply == "PEERS") {
			QStringList listpeers = QString::fromUtf8(buffer).trimmed().split("=")[1].split(";");
			for(int i = 0 ; i < listpeers.size() - 1; i++) {
				QStringList liststatus = listpeers[i].split(":");
				updatePeers(liststatus);
			}
		} else if(reply == "CALLERIDS") {
			QStringList listpeers = QString::fromUtf8(buffer).trimmed().split("=")[1].split(";");
			for(int i = 0 ; i < listpeers.size() - 1; i++) {
				QStringList liststatus = listpeers[i].split(":");
				updateCallerids(liststatus);
			}
			askPeers();
		} else if(reply == "PEERUPDATE") {
			QStringList liststatus = QString::fromUtf8(buffer).trimmed().split("=")[1].split(":");
			QString isupdate = QString::fromUtf8(buffer).trimmed().split("=")[0];
			if(isupdate == "update") {
				updatePeers(liststatus);
				qDebug() << qsl; //reply;
			}
		} else {
			qDebug() << qsl; //reply;
		}
		
		m_pendingkeepalivemsg = 0;
	}
}

void Engine::processHistory(const QStringList & histlist)
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

void Engine::stopKeepAliveTimer()
{
	if( m_ka_timerid > 0 )
	{
		killTimer(m_ka_timerid);
		m_ka_timerid = 0;
	}
}

void Engine::stopTryAgainTimer()
{
	if( m_try_timerid > 0 )
	{
		killTimer(m_try_timerid);
		m_try_timerid = 0;
	}
}

void Engine::startTryAgainTimer()
{
	if( m_try_timerid == 0 && m_trytoreconnect )
	{
		m_try_timerid = startTimer(m_trytoreconnectinterval);
	}
}

void Engine::timerEvent(QTimerEvent * event)
{
	int timerId = event->timerId();
	qDebug() << "Engine::timerEvent() timerId=" << timerId;
	if(timerId == m_ka_timerid)
	{
		keepLoginAlive();
		event->accept();
	}
	else if(timerId == m_try_timerid)
	{
		start();
		event->accept();
	}
	else
	{
		event->ignore();
	}
}

