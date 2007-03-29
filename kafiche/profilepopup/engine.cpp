/*
kafiche client : popup profile for incoming calls
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
#include <QTcpSocket>
#include <QThread>
#include <QMessageBox>
#include <QDebug>
#include <QSettings>
#include <QTimerEvent>
#include "engine.h"
#include "popup.h"

/*!
 * This constructor initialize the UDP socket and
 * the TCP listening socket.
 * It also connects signals with the right slots.
 */
Engine::Engine(QObject *parent)
: QObject(parent),
  m_serverip(""), m_serverport(0), m_serverast(""), m_login(""), m_passwd(""),
  m_listenport(0), m_sessionid(""), m_state(ENotLogged),
  m_pendingkeepalivemsg(0)
{
	m_ka_timerid = 0;
	m_try_timerid = 0;
	loadSettings();
	setAvailable();
	
	// init listen socket for profile push
	connect( &m_loginsocket, SIGNAL(connected()),
	         this, SLOT(identifyToTheServer()) );
	connect( &m_loginsocket, SIGNAL(readyRead()),
	         this, SLOT(processLoginDialog()) );
	connect( &m_loginsocket, SIGNAL(hostFound()),
	         this, SLOT(serverHostFound()) );
	initListenSocket();
	connect( &m_listensocket, SIGNAL(newConnection()),
	         this, SLOT(handleProfilePush()) );
	// init UDP socket used for keep alive
	m_udpsocket.bind();
	connect( &m_udpsocket, SIGNAL(readyRead()),
             this, SLOT(readKeepLoginAliveDatagrams()) );
	
	if(m_autoconnect)
		start();
}

/*!
 * Load settings using QSettings class which is portable.
 */
void Engine::loadSettings()
{
	QSettings settings;
	m_serverip = settings.value("engine/serverhost").toString();
	m_serverport = settings.value("engine/serverport", 12345).toUInt();
	m_serverast = settings.value("engine/serverastid").toString();
	m_login = settings.value("engine/login").toString();
	m_passwd = settings.value("engine/passwd").toString();
	m_autoconnect = settings.value("engine/autoconnect", false).toBool();
	m_trytoreconnect = settings.value("engine/trytoreconnect", false).toBool();
	m_keepaliveinterval = settings.value("engine/keepaliveinterval", 20*1000).toUInt();
	m_trytoreconnectinterval = settings.value("engine/trytoreconnectinterval", 20*1000).toUInt();
}

/*!
 * Save settings using QSettings class which is portable.
 */
void Engine::saveSettings()
{
	QSettings settings;
	settings.setValue("engine/serverhost", m_serverip);
	settings.setValue("engine/serverport", m_serverport);
	settings.setValue("engine/serverastid", m_serverast);
	settings.setValue("engine/login", m_login);
	settings.setValue("engine/passwd", m_passwd);
	settings.setValue("engine/autoconnect", m_autoconnect);
	settings.setValue("engine/trytoreconnect", m_trytoreconnect);
	settings.setValue("engine/keepaliveinterval", m_keepaliveinterval);
	settings.setValue("engine/trytoreconnectinterval", m_trytoreconnectinterval);
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
	m_loginsocket.connectToHost(m_serverip, m_serverport);
}

/*!
 * This method disconnect the engine from the the server
 */
void Engine::stop()
{
	qDebug() << "Engine::stop()";
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
 * \sa setDoesNotDisturb()
 */
void Engine::setAvailState(const QString & newstate)
{
	if(m_availstate != newstate)
	{
		m_availstate = newstate;
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

void Engine::setDoesNotDisturb()
{
	//qDebug() << "setDoesNotDistrurb()";
	setAvailState("doesnotdisturb");
}

// === Getter and Setters ===
const QString & Engine::serverip() const
{
	return m_serverip;
}

const QString & Engine::serverast() const
{
	return m_serverast;
}

void Engine::setServerip(const QString & serverip)
{
	m_serverip = serverip;
}

void Engine::setServerAst(const QString & serverast)
{
	m_serverast = serverast;
}

ushort Engine::serverport() const
{
	return m_serverport;
}

void Engine::setServerport(ushort port)
{
	//qDebug( "Engine::setServerport(%hu)", port);
	m_serverport = port;
}

const QString & Engine::login() const
{
	return m_login;
}

void Engine::setLogin(const QString & login)
{
	m_login = login;
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
	QString outline;
	qDebug() << "Engine::identifyToTheServer()" << m_loginsocket.peerAddress();
	m_serveraddress = m_loginsocket.peerAddress();
	outline = "LOGIN ";
	outline.append(m_serverast + "/");
	outline.append(m_login);
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
	QString outline;
	if(readLine.startsWith("Send PASS"))
	{
		outline = "PASS ";
		outline.append(m_passwd);
		outline.append("\r\n");
	}
	else if(readLine.startsWith("Send PORT"))
	{
		outline = "PORT ";
		outline.append(QString::number(m_listenport));
		outline.append("\r\n");
	}
	else if(readLine.startsWith("OK SESSIONID"))
	{
		readLine.remove(QChar('\r')).remove(QChar('\n'));
		QStringList sessionResp = readLine.split(" ");
		m_sessionid = sessionResp[2];
		m_loginsocket.close();
		setState(ELogged);
		// start the keepalive timer
		m_ka_timerid = startTimer(m_keepaliveinterval);
		return;
	}
	else
	{
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
		stopKeepAliveTimer();
		setState(ENotLogged);
		m_pendingkeepalivemsg = 0;
		startTryAgainTimer();
		return;
	}
	QString outline = "ALIVE ";
	outline.append(m_login);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" STATE ");
	outline.append(m_availstate);
	outline.append("\r\n");
	qDebug() <<  "Engine::keepLoginAlive()" << outline;
	m_udpsocket.writeDatagram( outline.toAscii(),
	                           m_serveraddress, m_serverport+1 );
	m_pendingkeepalivemsg++;
	// if the last keepalive msg has not been answered, send this one
	// twice
	if(m_pendingkeepalivemsg > 1)
	{
		m_udpsocket.writeDatagram( outline.toAscii(),
	    	                       m_serveraddress, m_serverport+1 );
		m_pendingkeepalivemsg++;
	}
}

/*!
 * Process incoming UDP datagrams which are likely to be 
 * response from keep alive messages.
 * If the response is not 'OK', goes to
 * the "not connected" state.
 */
void Engine::readKeepLoginAliveDatagrams()
{
	char buffer[256];
	int len;
	qDebug() << "Engine::readKeepLoginAliveDatagrams()";
	while( m_udpsocket.hasPendingDatagrams() )
	{
		len = m_udpsocket.readDatagram(buffer, sizeof(buffer)-1);
		if(len == 0)
			continue;
		buffer[len] = '\0';
		qDebug() << len << ":" << buffer;
		if(buffer[0] != 'O' || buffer[1] !='K')
		{
			stopKeepAliveTimer();
			setState(ENotLogged);
			startTryAgainTimer();
		}
		m_pendingkeepalivemsg = 0;
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

