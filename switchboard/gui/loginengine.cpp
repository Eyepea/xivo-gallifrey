/*
XIVO switchboard & customer information client : 
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

/* $Revision$
   $Date$
*/

#include <QDebug>
#include <QMessageBox>
#include <QSettings>
#include <QStringList>
#include <QTcpSocket>
#include <QTime>
#include <QTimerEvent>

#include "loginengine.h"
#include "popup.h"

/*! \brief Constructor.
 *
 * Construct the LoginEngine object and load settings.
 * The TcpSocket Object used to communicate with the server
 * is created and connected to the right slots/signals
 */
LoginEngine::LoginEngine(QObject * parent)
	: QObject(parent),
	  m_loginport(0), m_sessionid(""),
	  m_state(ENotLogged), m_pendingkeepalivemsg(0)
{
	m_ka_timerid = 0;
	m_try_timerid = 0;
	m_loginsocket  = new QTcpSocket(this);
	m_udpsocket    = new QUdpSocket(this);
	m_listensocket = new QTcpServer(this);
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
	connect( m_loginsocket, SIGNAL(connected()),
	         this, SLOT(identifyToTheServer()) );
	connect( m_loginsocket, SIGNAL(readyRead()),
	         this, SLOT(processLoginDialog()) );
	connect( m_loginsocket, SIGNAL(hostFound()),
	         this, SLOT(serverHostFound()) );
	if(!m_tcpmode)
		initListenSocket();
	connect( m_listensocket, SIGNAL(newConnection()),
	         this, SLOT(handleProfilePush()) );
	// init UDP socket used for keep alive
	//if(!m_tcpmode)
	m_udpsocket->bind();
	connect( m_udpsocket, SIGNAL(readyRead()),
		 this, SLOT(readKeepLoginAliveDatagrams()) );
	
	if(m_autoconnect)
		start();
}

/*!
 * Load Settings from the registery/configuration file
 */
void LoginEngine::loadSettings()
{
	QSettings settings;
	m_serverip = settings.value("engine/serverhost").toString();
	m_userid = settings.value("engine/userid").toString();
	m_autoconnect = settings.value("engine/autoconnect", false).toBool();
	m_trytoreconnect = settings.value("engine/trytoreconnect", false).toBool();
	m_trytoreconnectinterval = settings.value("engine/trytoreconnectinterval", 20*1000).toUInt();
	m_tcpmode = settings.value("engine/tcpmode", false).toBool();
	m_asterisk = settings.value("engine/asterisk").toString();
	m_protocol = settings.value("engine/protocol").toString();

	m_enabled = settings.value("engine/enabled", true).toBool();
	m_loginport = settings.value("engine/loginport", 5000).toUInt();
	m_passwd = settings.value("engine/passwd").toString();
	m_availstate = settings.value("engine/availstate", "available").toString();
	m_keepaliveinterval = settings.value("engine/keepaliveinterval", 20*1000).toUInt();
}

/*!
 * Save Settings to the registery/configuration file
 */
void LoginEngine::saveSettings()
{
	QSettings settings;
	// these commented settings are saved by BaseEngine::saveSettings()
	// in the Switchboard framework :
	//
	//	settings.setValue("engine/serverhost", m_serverip);
	//
	//	settings.setValue("engine/userid", m_userid);
	//	settings.setValue("engine/autoconnect", m_autoconnect);
	//	settings.setValue("engine/trytoreconnect", m_trytoreconnect);
	//	settings.setValue("engine/trytoreconnectinterval", m_trytoreconnectinterval);
	//	settings.setValue("engine/asterisk", m_asterisk);
	//	settings.setValue("engine/protocol", m_protocol);
	
	settings.setValue("engine/enabled", m_enabled);
	settings.setValue("engine/loginport", m_loginport);
	settings.setValue("engine/passwd", m_passwd);
	settings.setValue("engine/availstate", m_availstate);
	settings.setValue("engine/keepaliveinterval", m_keepaliveinterval);
}

/*!
 *
 */
void LoginEngine::setEnabled(bool b) {
	if(b != m_enabled) {
		m_enabled = b;
		if(state() == ELogged)
			availAllowChanged(b);
	}
}

void LoginEngine::initListenSocket()
{
	if (!m_listensocket->listen())
	{
		QMessageBox::critical(NULL, tr("Critical error"),
		                            tr("Unable to start the server: %1.")
		                            .arg(m_listensocket->errorString()));
		return;
	}
	m_listenport = m_listensocket->serverPort();
	qDebug() << "LoginEngine::initListenSocket()" << m_listenport;
}

/*! \brief Start the connection to the server
 */
void LoginEngine::start()
{
	qDebug() << "LoginEngine::start()" << m_serverip << m_loginport << m_enabled;
	m_loginsocket->abort();
	if(m_enabled)
		m_loginsocket->connectToHost(m_serverip, m_loginport);
}

/*! \brief close the connection to the server
 */
void LoginEngine::stop()
{
	qDebug() << "LoginEngine::stop()";
	if(m_sessionid != "") {
		QString outline = "STOP ";
		outline.append(m_asterisk + "/" + m_protocol.toLower() + m_userid);
		outline.append(" SESSIONID ");
		outline.append(m_sessionid);
		//qDebug() << "LoginEngine::stop()" << outline;
		outline.append("\r\n");
		m_udpsocket->writeDatagram( outline.toAscii(),
					    m_serveraddress, m_loginport + 1 );
	}
	stopKeepAliveTimer();
	stopTryAgainTimer();
	setState(ENotLogged);
	m_sessionid = "";
	m_loginsocket->disconnectFromHost();
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
void LoginEngine::setAvailState(const QString & newstate)
{
	if(m_availstate != newstate)
	{
		QSettings settings;
		m_availstate = newstate;
		settings.setValue("engine/availstate", m_availstate);
		keepLoginAlive();
	}
}

void LoginEngine::setAvailable()
{
	//qDebug() << "setAvailable()";
	setAvailState("available");
}

void LoginEngine::setAway()
{
	//qDebug() << "setAway()";
	setAvailState("away");
}

void LoginEngine::setBeRightBack()
{
	setAvailState("berightback");
}

void LoginEngine::setOutToLunch()
{
	setAvailState("outtolunch");
}

void LoginEngine::setDoNotDisturb()
{
	//qDebug() << "setDoNotDistrurb()";
	setAvailState("donotdisturb");
}


void LoginEngine::setVoiceMail(bool b)
{
	QString outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol.toLower() + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" FEATURES PUT VM ");
	outline.append(b?"1":"0");
	outline.append("\r\n");
	qDebug() << outline;
	m_udpsocket->writeDatagram( outline.toAscii(),
				    m_serveraddress, m_loginport + 1 );
}

void LoginEngine::setCallRecording(bool b)
{
	QString outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol.toLower() + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" FEATURES PUT Record ");
	outline.append(b?"1":"0");
	outline.append("\r\n");
	qDebug() << outline;
	m_udpsocket->writeDatagram( outline.toAscii(),
				    m_serveraddress, m_loginport + 1 );
}

void LoginEngine::setCallFiltering(bool b)
{
	QString outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol.toLower() + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" FEATURES PUT Screen ");
	outline.append(b?"1":"0");
	outline.append("\r\n");
	qDebug() << outline;
	m_udpsocket->writeDatagram( outline.toAscii(),
				    m_serveraddress, m_loginport + 1 );
}

void LoginEngine::setDnd(bool b)
{
	QString outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol.toLower() + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" FEATURES PUT DND ");
	outline.append(b?"1":"0");
	outline.append("\r\n");
	qDebug() << outline;
	m_udpsocket->writeDatagram( outline.toAscii(),
				    m_serveraddress, m_loginport + 1 );
}

void LoginEngine::setForwardOnUnavailable(bool b, const QString & dst)
{
	QString outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol.toLower() + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" FEATURES PUT FWD/RNA/Status ");
	outline.append(b?"1":"0");
	outline.append("\r\n");
	qDebug() << outline;
	m_udpsocket->writeDatagram( outline.toAscii(),
				    m_serveraddress, m_loginport + 1 );
	outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol.toLower() + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" FEATURES PUT FWD/RNA/Number ");
	outline.append(dst);
	outline.append("\r\n");
	qDebug() << outline;
	m_udpsocket->writeDatagram( outline.toAscii(),
				    m_serveraddress, m_loginport + 1 );
}

void LoginEngine::setForwardOnBusy(bool b, const QString & dst)
{
	QString outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol.toLower() + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" FEATURES PUT FWD/Busy/Status ");
	outline.append(b?"1":"0");
	outline.append("\r\n");
	qDebug() << outline;
	m_udpsocket->writeDatagram( outline.toAscii(),
				    m_serveraddress, m_loginport + 1 );
	outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol.toLower() + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" FEATURES PUT FWD/Busy/Number ");
	outline.append(dst);
	outline.append("\r\n");
	qDebug() << outline;
	m_udpsocket->writeDatagram( outline.toAscii(),
				    m_serveraddress, m_loginport + 1 );
}

void LoginEngine::setUncondForward(bool b, const QString & dst)
{
	QString outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol.toLower() + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" FEATURES PUT FWD/Unc/Status ");
	outline.append(b?"1":"0");
	outline.append("\r\n");
	qDebug() << outline;
	m_udpsocket->writeDatagram( outline.toAscii(),
				    m_serveraddress, m_loginport + 1 );
	outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol.toLower() + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" FEATURES PUT FWD/Unc/Number ");
	outline.append(dst);
	outline.append("\r\n");
	qDebug() << outline;
	m_udpsocket->writeDatagram( outline.toAscii(),
				    m_serveraddress, m_loginport + 1 );
}

void LoginEngine::askFeatures()
{
	QString outline = "COMMAND ";
	outline.append(m_asterisk + "/" + m_protocol.toLower() + m_userid);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" FEATURES GET\r\n");
	qDebug() << outline;
	m_udpsocket->writeDatagram( outline.toAscii(),
				    m_serveraddress, m_loginport + 1 );
}

// === Getter and Setters ===
/*! \brief get server host */
const QString & LoginEngine::serverip() const
{
	return m_serverip;
}

const QString & LoginEngine::serverast() const
{
	return m_asterisk;
}

void LoginEngine::setServerip(const QString & serverip)
{
	m_serverip = serverip;
}

void LoginEngine::setServerAst(const QString & serverast)
{
	m_asterisk = serverast;
}

/*!
 * Setter for the m_keepaliveinterval property.
 * if the value is changed, existing timer is restarted.
 *
 * \sa keepaliveinterval
 */
void LoginEngine::setKeepaliveinterval(uint i)
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

void LoginEngine::setTrytoreconnect(bool b)
{
	m_trytoreconnect = b;
}

bool LoginEngine::trytoreconnect() const
{
	return m_trytoreconnect;
}

uint LoginEngine::trytoreconnectinterval() const
{
	return m_trytoreconnectinterval;
}

/*!
 * Setter for property m_trytoreconnectinterval
 * Restart timer if the value changed.
 *
 * \sa trytoreconnectinterval
 */
void LoginEngine::setTrytoreconnectinterval(uint i)
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

const LoginEngine::EngineState LoginEngine::state() const
{
	return m_state;
}

/*!
 * setter for the m_state property.
 * If the state is becoming ELogged, the
 * signal logged() is thrown.
 * If the state is becoming ENotLogged, the
 * signal delogged() is thrown.
 */
void LoginEngine::setState(EngineState state)
{
	if(state != m_state)
	{
		m_state = state;
		if(state == ELogged) {
			stopTryAgainTimer();
			if(m_enabled) availAllowChanged(true);
			logged();
		} else if(state == ENotLogged) {
			availAllowChanged(false);
			delogged();
		}
	}
}

/*!
 * Perform the first login step once the TCP connection is established.
 */
void LoginEngine::identifyToTheServer()
{
	QString outline;
	qDebug() << "LoginEngine::identifyToTheServer()" << m_loginsocket->peerAddress();
	m_serveraddress = m_loginsocket->peerAddress();
	outline = "LOGIN ";
	outline.append(m_asterisk + "/" + m_protocol.toLower() + m_userid);
#if defined(Q_WS_X11)
	outline.append(" SB@X11");
#elif defined(Q_WS_WIN)
	outline.append(" SB@WIN");
#elif defined(Q_WS_MAC)
	outline.append(" SB@MAC");
#else
	outline.append(" SB@unknown");
#endif
	qDebug() << "LoginEngine::identifyToTheServer() : " << outline;
	outline.append("\r\n");
	m_loginsocket->write(outline.toAscii());
	m_loginsocket->flush();
}

/*!
 * Perform the following of the login process after identifyToTheServer()
 * made the first step.
 * Theses steps are : sending the password, sending the port,
 *   just reading the session id from server response.
 * The state is changed accordingly.
 */
void LoginEngine::processLoginDialog()
{
	char buffer[256];
	int len;
	qDebug() << "LoginEngine::processLoginDialog()";
	if(m_tcpmode && (m_state == ELogged)) {
		Popup * popup = new Popup(m_loginsocket, m_sessionid);
		connect( popup, SIGNAL(destroyed(QObject *)),
		         this, SLOT(popupDestroyed(QObject *)) );
		connect( popup, SIGNAL(wantsToBeShown(Popup *)),
			 this, SLOT(profileToBeShown(Popup *)) );
		popup->streamNewData();
		return;
	}
	if(!m_loginsocket->canReadLine())
	{
		qDebug() << "no line ready to be read";
		return;
	}
	len = m_loginsocket->readLine(buffer, sizeof(buffer));
	if(len<0)
	{
		qDebug() << "readLine() returned -1, closing socket";
		m_loginsocket->close();
		setState(ENotLogged);
		return;
	}
	QString readLine = QString::fromAscii(buffer);
	QString outline;
	if(readLine.startsWith("Send PASS"))
	{
		outline = "PASS ";
		outline.append(m_passwd);
	}
	else if(readLine.startsWith("Send PORT"))
	{
		if(m_tcpmode) {
			outline = "TCPMODE";
		} else {
			outline = "PORT ";
			outline.append(QString::number(m_listenport));
		}
		qDebug() << "LoginEngine::processLoginDialog()" << m_listenport;
	}
	else if(readLine.startsWith("Send STATE"))
	{
		outline = "STATE ";
		outline.append(m_availstate);
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
		qDebug() << m_dialcontext << m_capabilities;
		m_loginsocket->close();
		setState(ELogged);
		// start the keepalive timer
		m_ka_timerid = startTimer(m_keepaliveinterval);
		return;
	}
	else
	{
		readLine.remove(QChar('\r')).remove(QChar('\n'));
		qDebug() << "Response from server not recognized, closing" << readLine;
		m_loginsocket->close();
		setState(ENotLogged);
		return;
	}
	
	qDebug() << "LoginEngine::processLoginDialog() : " << outline;
	outline.append("\r\n");
	m_loginsocket->write(outline.toAscii());
	m_loginsocket->flush();
}

/*!
 * This slot is connected to the hostFound() signal of the
 * m_loginsocket
 */
void LoginEngine::serverHostFound()
{
	qDebug() << "LoginEngine::serverHostFound()" << m_loginsocket->peerAddress();
}

/*!
 * This slot method is called when a pending connection is
 * waiting on the m_listensocket.
 * It processes the incoming data and create a popup to display it.
 */
void LoginEngine::handleProfilePush()
{
	qDebug() << "LoginEngine::handleProfilePush()";
	QTcpSocket *connection = m_listensocket->nextPendingConnection();
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

void LoginEngine::popupDestroyed(QObject * obj)
{
	qDebug() << "Popup destroyed" << obj;
	//qDebug() << "========================";
	//obj->dumpObjectTree();
	//qDebug() << "========================";
}

void LoginEngine::profileToBeShown(Popup * popup)
{
	qDebug() << "LoginEngine::profileToBeShown()";
	newProfile( popup );
}

/*!
 * Send a keep alive message to the login server.
 * The message is sent in a datagram through m_udpsocket
 */ 
void LoginEngine::keepLoginAlive()
{
	qDebug() << "LoginEngine::keepLoginAlive()";
	// got to disconnected state if more than xx keepalive messages
	// have been left without response.
	if(m_pendingkeepalivemsg > 1)
	{
		qDebug() << "m_pendingkeepalivemsg" << m_pendingkeepalivemsg << "=> 0";
		stopKeepAliveTimer();
		setState(ENotLogged);
		m_pendingkeepalivemsg = 0;
		startTryAgainTimer();
		return;
	}
	if(m_state == ELogged) {
		QString outline = "ALIVE ";
		outline.append(m_asterisk + "/" + m_protocol.toLower() + m_userid);
		outline.append(" SESSIONID ");
		outline.append(m_sessionid);
		outline.append(" STATE ");
		outline.append(m_availstate);
		qDebug() << "LoginEngine::keepLoginAlive() : " << outline;
		outline.append("\r\n");
		m_udpsocket->writeDatagram( outline.toAscii(),
					    m_serveraddress, m_loginport + 1 );
		m_pendingkeepalivemsg++;
		// if the last keepalive msg has not been answered, send this one
		// twice
		if(m_pendingkeepalivemsg > 1)
			{
				m_udpsocket->writeDatagram( outline.toAscii(),
							    m_serveraddress, m_loginport + 1 );
				m_pendingkeepalivemsg++;
			}
	}
}

void LoginEngine::initFeatureFields(const QString & field, const QString & value)
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

/*!
 * Process incoming UDP datagrams which are likely to be 
 * response from keep alive messages.
 * If the response is not 'OK', goes to
 * the "not connected" state.
 */
void LoginEngine::readKeepLoginAliveDatagrams()
{
	char buffer[2048];
	int len;
	//	qDebug() << "LoginEngine::readKeepLoginAliveDatagrams()";
	while( m_udpsocket->hasPendingDatagrams() )
	{
		len = m_udpsocket->readDatagram(buffer, sizeof(buffer)-1);
		if(len == 0)
			continue;
		buffer[len] = '\0';
		QStringList qsl = QString::fromUtf8(buffer).trimmed().split(" ");
		QString reply = qsl[0];

		if(reply == "DISC") {
			// stopKeepAliveTimer();
			setState(ENotLogged);
 			// startTryAgainTimer();
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
		}
		m_pendingkeepalivemsg = 0;
	}
}

void LoginEngine::stopKeepAliveTimer()
{
	if( m_ka_timerid > 0 )
	{
		killTimer(m_ka_timerid);
		m_ka_timerid = 0;
	}
}

void LoginEngine::stopTryAgainTimer()
{
	if( m_try_timerid > 0 )
	{
		killTimer(m_try_timerid);
		m_try_timerid = 0;
	}
}

void LoginEngine::startTryAgainTimer()
{
	if( m_try_timerid == 0 && m_trytoreconnect )
	{
		m_try_timerid = startTimer(m_trytoreconnectinterval);
	}
}

/*! \brief periodicaly called method
 *
 * Performs keep alive or periodic "try to reconnect".
 */
void LoginEngine::timerEvent(QTimerEvent * event)
{
	int timerId = event->timerId();
	qDebug() << "LoginEngine::timerEvent() timerId=" << timerId << m_ka_timerid << m_try_timerid;
	if(timerId == m_ka_timerid) {
		keepLoginAlive();
		event->accept();
	} else if(timerId == m_try_timerid) {
		start();
		event->accept();
	} else {
		event->ignore();
	}
}

