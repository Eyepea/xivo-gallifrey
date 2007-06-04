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
#include <QSettings>
#include <QStringList>
#include <QTcpSocket>
#include <QTime>
#include <QTimerEvent>
#include "loginengine.h"

/*! \brief Constructor.
 *
 * Construct the LoginEngine object and load settings.
 * The TcpSocket Object used to communicate with the server
 * is created and connected to the right slots/signals
 */
LoginEngine::LoginEngine(QObject * parent)
: QObject(parent)
{
	m_ka_timerid = 0;
	m_try_timerid = 0;
	m_loginsocket = new QTcpSocket(this);
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

	// Connect socket signals
	connect( m_loginsocket, SIGNAL(connected()),
	         this, SLOT(identifyToTheServer()) );
	connect( m_loginsocket, SIGNAL(readyRead()),
	         this, SLOT(processLoginDialog()) );

	if(m_autoconnect)
		start();
}

/*!
 * Load Settings from the registery/configuration file
 */
void LoginEngine::loadSettings()
{
	QSettings settings;
	m_serverhost = settings.value("engine/serverhost").toString();
	m_loginport = settings.value("engine/loginport", 5000).toUInt();
	m_asterisk = settings.value("engine/asterisk").toString();
	m_autoconnect = settings.value("engine/autoconnect", false).toBool();
	m_protocol = settings.value("engine/protocol").toString();
	m_extension = settings.value("engine/extension").toString();
	m_passwd = settings.value("engine/passwd").toString();
	m_availstate = settings.value("engine/availstate", "available").toString();
	m_keepaliveinterval = settings.value("engine/keepaliveinterval", 20*1000).toUInt();
	m_trytoreconnect = settings.value("engine/trytoreconnect", false).toBool();
	m_trytoreconnectinterval = settings.value("engine/trytoreconnectinterval", 20*1000).toUInt();
}

/*!
 * Save Settings to the registery/configuration file
 */
void LoginEngine::saveSettings()
{
	QSettings settings;
	settings.setValue("engine/serverhost", m_serverhost);
	settings.setValue("engine/loginport", m_loginport);
	settings.setValue("engine/asterisk", m_asterisk);
	settings.setValue("engine/autoconnect", m_autoconnect);
	settings.setValue("engine/protocol", m_protocol);
	settings.setValue("engine/extension", m_extension);
	settings.setValue("engine/passwd", m_passwd);
	settings.setValue("engine/availstate", m_availstate);
	settings.setValue("engine/keepaliveinterval", m_keepaliveinterval);
	settings.setValue("engine/trytoreconnect", m_trytoreconnect);
	settings.setValue("engine/trytoreconnectinterval", m_trytoreconnectinterval);
}

/*! \brief Start the connection to the server
 */
void LoginEngine::start()
{
	qDebug() << "LoginEngine::start()";
	m_loginsocket->abort();
	m_loginsocket->connectToHost(m_serverhost, m_loginport);
}

/*! \brief close the connection to the server
 */
void LoginEngine::stop()
{
	QString outline;
	qDebug() << "LoginEngine::stop()";
	outline = "STOP ";
	if(m_asterisk.length() > 0) {
		outline.append(m_asterisk);
		outline.append("/");
		outline.append("sip");
	}
	outline.append(m_extension);
	outline.append("\r\n");

	m_udpsocket.writeDatagram( outline.toAscii(),
				   m_serveraddress, m_loginport+1 );
	stopKeepAliveTimer();
	stopTryAgainTimer();
	setState(ENotLogged);
	m_socket->disconnectFromHost();
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
		if(state == ELogged)
		{
			stopTryAgainTimer();
			logged();
		}
		else if(state == ENotLogged)
			delogged();
	}
}

/*! \brief get server host */
const QString & LoginEngine::host() const
{
	return m_serverhost;
}

/*! \brief get server port */
quint16 LoginEngine::loginport() const
{
	return m_loginport;
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
	if(m_asterisk.length() > 0) {
		outline.append(m_asterisk);
		outline.append("/");
		outline.append("sip");
	}
	outline.append(m_extension + "\r\n");
	m_loginsocket->write(outline.toAscii());
	m_loginsocket->flush();
	qDebug() << outline;
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
		outline.append("\r\n");
	}
	else if(readLine.startsWith("Send PORT"))
	{
		outline = "PORT ";
		outline.append(QString::number(0)); // m_listenport
		outline.append("\r\n");
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
			m_context = sessionResp[3];
		if(sessionResp.size() > 4)
			m_capabilities = sessionResp[4];
		m_loginsocket->close();
		setState(ELogged);
		// start the keepalive timer
		m_ka_timerid = startTimer(m_keepaliveinterval);
		return;
	}
	else
	{
		qDebug() << "Response from server not recognized, closing" << readLine;
		m_loginsocket->close();
		setState(ENotLogged);
		return;
	}
	qDebug() << outline;
	m_loginsocket->write(outline.toAscii());
	m_loginsocket->flush();
}

/*!
 * Send a keep alive message to the login server.
 * The message is sent in a datagram through m_udpsocket
 */ 
void LoginEngine::keepLoginAlive()
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
	outline.append(m_asterisk);
	outline.append("/sip");
	outline.append(m_extension);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" STATE ");
	outline.append(m_availstate);
	outline.append("\r\n");
	qDebug() <<  "LoginEngine::keepLoginAlive()" << outline;
	m_udpsocket.writeDatagram( outline.toAscii(),
	                           m_serveraddress, m_loginport+1 );
	m_pendingkeepalivemsg++;
	// if the last keepalive msg has not been answered, send this one
	// twice
	if(m_pendingkeepalivemsg > 1)
	{
		m_udpsocket.writeDatagram( outline.toAscii(),
	    	                       m_serveraddress, m_loginport+1 );
		m_pendingkeepalivemsg++;
	}
}

/*!
 * Process incoming UDP datagrams which are likely to be 
 * response from keep alive messages.
 * If the response is not 'OK', goes to
 * the "not connected" state.
 */
void LoginEngine::readKeepLoginAliveDatagrams()
{
	char buffer[256];
	int len;
	qDebug() << "LoginEngine::readKeepLoginAliveDatagrams()";
	while( m_udpsocket.hasPendingDatagrams() )
	{
		len = m_udpsocket.readDatagram(buffer, sizeof(buffer)-1);
		if(len == 0)
			continue;
		buffer[len] = '\0';
		//		qDebug() << len << ":" << buffer;
		if(buffer[0] != 'O' && buffer[0] != 'S' || buffer[1] !='K' && buffer[1] !='E')
		{
			stopKeepAliveTimer();
			setState(ENotLogged);
			startTryAgainTimer();
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

void LoginEngine::timerEvent(QTimerEvent * event)
{
	int timerId = event->timerId();
	qDebug() << "LoginEngine::timerEvent() timerId=" << timerId;
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

