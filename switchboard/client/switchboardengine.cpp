#include <QDebug>
#include <QTime>
#include <QStringList>
#include "switchboardengine.h"
#include "switchboardwindow.h"

SwitchBoardEngine::SwitchBoardEngine(QObject * parent)
: QObject(parent), m_port(0)
{
	m_socket = new QTcpSocket(this);
/*
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

void SwitchBoardEngine::setWindow(SwitchBoardWindow * window)
{
	m_window = window;
}

void SwitchBoardEngine::setAddress(const QString & host, quint16 port)
{
	m_host = host;
	m_port = port;
	connectSocket();
	m_pendingcommand = "hints";
	socketConnected();
}

void SwitchBoardEngine::connectSocket()
{
	m_socket->connectToHost(m_host, m_port);
}

/* Slots */
void SwitchBoardEngine::socketConnected()
{
	qDebug() << "socketConnected()";
	m_socket->write((m_pendingcommand + "\n").toAscii());
}

void SwitchBoardEngine::socketDisconnected()
{
	qDebug() << "socketDisconnected()";
	finishedReceivingHints();
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
}

void SwitchBoardEngine::socketReadyRead()
{
	qDebug() << "socketReadyRead()";
	//QByteArray data = m_socket->readAll();
	//qDebug() << data;
	bool b = false;
	while(m_socket->canReadLine())
	{
		QByteArray data = m_socket->readLine();
		QString line(data);
		QStringList list = line.trimmed().split("=");
		if((list.size() == 2) && m_window) {
			if(list[0] == QString("hints")) {
				QStringList listpeers = list[1].split(";");
				for(int i = 0 ; i < listpeers.size() - 1; i++) {
					QStringList liststatus = listpeers[i].split(":");
					m_window->updatePeer(liststatus[0], liststatus[1]);
				}
				b = true;
			} else if(list[0] == QString("update")) {
				QStringList liststatus = list[1].split(":");
				m_window->updatePeer(liststatus[0], liststatus[1]);
				b = true;
			} else if(list[0] == QString("asterisk")) {
				QTime currentTime = QTime::currentTime();
				QString currentTimeStr = currentTime.toString("hh:mm:ss");
				emitTextMessage(list[1] + " at " + currentTimeStr);
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
	//	m_pendingcommand = "hints";
	//	socketConnected();
}

void SwitchBoardEngine::originateCall(const QString & src, const QString & dst)
{
	m_pendingcommand = "originate " + src + " " + dst;
	socketConnected();
// 	if(m_socket->state() == QAbstractSocket::UnconnectedState)
// 		connectSocket();
}

void SwitchBoardEngine::transferCall(const QString & src, const QString & dst)
{
	m_pendingcommand = "transfer " + src + " " + dst;
	socketConnected();
// 	if(m_socket->state() == QAbstractSocket::UnconnectedState)
// 		connectSocket();
}

void SwitchBoardEngine::hangUp(const QString & peer)
{
	m_pendingcommand = "hangup " + peer;
	socketConnected();
// 	if(m_socket->state() == QAbstractSocket::UnconnectedState)
// 		connectSocket();
}

