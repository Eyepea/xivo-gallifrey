#include <QTcpSocket>
#include <QThread>
#include <QMessageBox>
#include <QDebug>
#include <QSettings>
#include "engine.h"
#include "popup.h"

/*!
 * This constructor initialize the UDP socket and
 * the TCP listening socket.
 * It also connects signals with the right slots.
 */
Engine::Engine(QObject *parent)
: QObject(parent),
  m_serverip(""), m_serverport(0), m_login(""), m_passwd(""),
  m_listenport(0), m_timer(this), m_sessionid(""), m_state(ENotLogged)
{
	loadSettings();
	
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
             this, SLOT(readKeepLoginAliveDatagrams()));
	connect( &m_timer, SIGNAL(timeout()),
	         this, SLOT(keepLoginAlive()));
}

/*!
 * Load settings using QSettings class which is portable.
 */
void Engine::loadSettings()
{
	QSettings settings;
	m_serverip = settings.value("engine/serverhost").toString();
	m_serverport = settings.value("engine/serverport", 12345).toUInt();
	m_login = settings.value("engine/login").toString();
	m_passwd = settings.value("engine/passwd").toString();
}

/*!
 * Save settings using QSettings class which is portable.
 */
void Engine::saveSettings()
{
	QSettings settings;
	settings.setValue("engine/serverhost", m_serverip);
	settings.setValue("engine/serverport", m_serverport);
	settings.setValue("engine/login", m_login);
	settings.setValue("engine/passwd", m_passwd);
}

void Engine::initListenSocket()
{
	if (!m_listensocket.listen())
	{
		QMessageBox::critical(NULL, tr("glop"),
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

// === Getter and Setters ===
const QString & Engine::serverip() const
{
	return m_serverip;
}

void Engine::setServerip(const QString & serverip)
{
	m_serverip = serverip;
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
			logged();
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
		m_timer.start(20*1000);	// start the keepalive timer
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
	//popup->show();
	//connection->disconnectFromHost();
}

void Engine::popupDestroyed(QObject * obj)
{
	qDebug() << "Popup destroyed" << obj;
	//qDebug() << "========================";
	obj->dumpObjectTree();
	qDebug() << "========================";
}

/*!
 * Send a keep alive message to the login server.
 * The message is sent in a datagram through m_udpsocket
 */ 
void Engine::keepLoginAlive()
{
	QString outline = "ALIVE ";
	outline.append(m_login);
	outline.append(" SESSIONID ");
	outline.append(m_sessionid);
	outline.append(" STATE ");
	outline.append("connected");
	outline.append("\r\n");
	qDebug() <<  "Engine::keepLoginAlive()" << outline;
	m_udpsocket.writeDatagram( outline.toAscii(),
	                           m_serveraddress, m_serverport+1 );
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
			m_timer.stop();
			setState(ENotLogged);
		}
	}
}

