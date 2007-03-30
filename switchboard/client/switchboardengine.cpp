#include <QDebug>
#include <QSettings>
#include <QTime>
#include <QStringList>
#include "switchboardengine.h"
#include "switchboardwindow.h"
#include "astchannel.h"

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
}

/*!
 * Load Settings from the registery/configuration file
 */
void SwitchBoardEngine::loadSettings()
{
	QSettings settings;
	m_host = settings.value("engine/serverhost").toString();
	m_port = settings.value("engine/serverport", 5081).toUInt();
}

/*!
 * Save Settings to the registery/configuration file
 */
void SwitchBoardEngine::saveSettings()
{
	QSettings settings;
	settings.setValue("engine/serverhost", m_host);
	settings.setValue("engine/serverport", m_port);
}

void SwitchBoardEngine::setWindow(SwitchBoardWindow * window)
{
	m_window = window;
}

void SwitchBoardEngine::setAddress(const QString & host, quint16 port)
{
	m_host = host;
	m_port = port;
	//connectSocket();
}

void SwitchBoardEngine::start()
{
	connectSocket();
}

void SwitchBoardEngine::stop()
{
	m_socket->disconnectFromHost();
}

void SwitchBoardEngine::connectSocket()
{
	m_socket->connectToHost(m_host, m_port);
}

/* Slots */
void SwitchBoardEngine::socketConnected()
{
	qDebug() << "socketConnected()";
	started();
	m_socket->write((m_pendingcommand + "\n").toAscii());
	qDebug() << "  " << m_pendingcommand;
}

void SwitchBoardEngine::socketDisconnected()
{
	qDebug() << "socketDisconnected()";
	stopped();
	emitTextMessage("Connection lost with Presence Server");
	//	finishedReceivingHints();
	if(m_window) m_window->removePeers();
	//connectSocket();
}

void SwitchBoardEngine::socketHostFound()
{
	qDebug() << "socketHostFound()";
}

void SwitchBoardEngine::socketError(QAbstractSocket::SocketError socketError)
{
	qDebug() << "socketError(" << socketError << ")";
	switch(socketError)
	{
	case QAbstractSocket::ConnectionRefusedError:
		emitTextMessage("Connection refused");
		if(m_timer != -1) killTimer(m_timer);
		m_timer = startTimer(2000);
		break;
	case QAbstractSocket::HostNotFoundError:
		emitTextMessage("Host not found");
		break;
	case QAbstractSocket::UnknownSocketError:
		emitTextMessage("Unknown socket error");
		break;
	default:
		break;
	}
}

void SwitchBoardEngine::socketStateChanged(QAbstractSocket::SocketState socketState)
{
	qDebug() << "socketStateChanged(" << socketState << ")";
	if(socketState == QAbstractSocket::ConnectedState) {
		if(m_timer != -1) killTimer(m_timer);
		m_pendingcommand = "hints";
		socketConnected();
	}
}

void SwitchBoardEngine::socketReadyRead()
{
	//	qDebug() << "socketReadyRead()";
	//QByteArray data = m_socket->readAll();
	//qDebug() << data;
	bool b = false;
	while(m_socket->canReadLine())
	{
		QByteArray data = m_socket->readLine();
		QString line(data);
		qDebug() << "<==" << line;
		QStringList list = line.trimmed().split("=");
		//qDebug() << "<==" << list.size() << m_window << list[0];
		if((list.size() == 2) && m_window) {
			if(list[0] == QString("hints")) {
				QStringList listpeers = list[1].split(";");
				for(int i = 0 ; i < listpeers.size() - 1; i++) {
					QStringList liststatus = listpeers[i].split(":");
					m_window->updatePeer(liststatus[1] + "/" + liststatus[3],
							     liststatus[4], liststatus[5], liststatus[6]);
				}
				b = true;
			} else if(list[0] == QString("update")) {
				QStringList liststatus = list[1].split(":");
				m_window->updatePeer(liststatus[1] + "/" + liststatus[3],
						     liststatus[4], liststatus[5], liststatus[6]);
				int n = liststatus[6].toInt();
				for (int i=0; i<n; i++) {
					// <channel>:<etat du channel>:<nb de secondes dans cet etat>:<to ou from>:<channel en liaison>:<numero en liaison>
					qDebug() << liststatus[7+6*i] << liststatus[7+6*i+1]
					         << liststatus[7+6*i+2] << liststatus[7+6*i+3]
					         << liststatus[7+6*i+4] << liststatus[7+6*i+5];
					updateCall(liststatus[7+6*i], liststatus[7+6*i+5],
					           "test1", "test2");
				}
			} else if(list[0] == QString("asterisk")) {
				QTime currentTime = QTime::currentTime();
				QString currentTimeStr = currentTime.toString("hh:mm:ss");
				emitTextMessage(list[1] + " at " + currentTimeStr);

			} else if(list[0] == QString("peeradd")) {
				QStringList listpeers = list[1].split(";");
				for(int i = 0 ; i < listpeers.size() - 1; i++) {
					QStringList liststatus = listpeers[i].split(":");
					//m_window->addPeer(liststatus[0] + "/" + liststatus[1], liststatus[2]);
					m_window->updatePeer(liststatus[1] + "/" + liststatus[3],
							     liststatus[4], liststatus[5], liststatus[6]);
				}
			} else if(list[0] == QString("peerremove")) {
				QStringList listpeers = list[1].split(";");
				for(int i = 0 ; i < listpeers.size() - 1; i++) {
					QStringList liststatus = listpeers[i].split(":");
					m_window->removePeer(liststatus[0] + "/" + liststatus[1]);
				}
			}
		}
	}
	if(b) {
		QTime currentTime = QTime::currentTime();
		QString currentTimeStr = currentTime.toString("hh:mm:ss");
		emitTextMessage("Peers' status updated at " + currentTimeStr);
	}
}

void SwitchBoardEngine::finishedReceivingHints()
{
	qDebug() << "finishedReceivingHints()";
}

void SwitchBoardEngine::timerEvent(QTimerEvent * event)
{
	qDebug() << event;
	m_socket->connectToHost(m_host, m_port);
	//	m_pendingcommand = "hints";
	//	socketConnected();
}

void SwitchBoardEngine::originateCall(const QString & src, const QString & dst)
{
	QStringList srcl = src.split("/");
	QStringList dstl = dst.split("/");
	if(srcl[0] == dstl[0]) {
		m_pendingcommand = "originate " + srcl[0] + " " + srcl[1] + " " + dstl[1];
		socketConnected();
	} else {
		emitTextMessage("<" + srcl[0] + "> and <" + dstl[0] + "> are not the same Asterisk !");
	}
// 	if(m_socket->state() == QAbstractSocket::UnconnectedState)
// 		connectSocket();
}

void SwitchBoardEngine::transferCall(const QString & src, const QString & dst)
{
	QStringList srcl = src.split("/");
	QStringList dstl = dst.split("/");
	if(srcl[0] == dstl[0]) {
		m_pendingcommand = "transfer " + srcl[0] + " " + srcl[1] + " " + dstl[1];
		socketConnected();
	} else {
		emitTextMessage("<" + srcl[0] + "> and <" + dstl[0] + "> are not the same Asterisk !");
	}
// 	if(m_socket->state() == QAbstractSocket::UnconnectedState)
// 		connectSocket();
}

void SwitchBoardEngine::hangUp(const QString & peer)
{
	QStringList peerl = peer.split("/");
	m_pendingcommand = "hangup " + peerl[0] + " " + peerl[1];
	socketConnected();
// 	if(m_socket->state() == QAbstractSocket::UnconnectedState)
// 		connectSocket();
}

const QString & SwitchBoardEngine::host() const
{
	return m_host;
}

quint16 SwitchBoardEngine::port() const
{
	return m_port;
}

