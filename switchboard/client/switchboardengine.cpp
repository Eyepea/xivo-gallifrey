#include <QDebug>
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
}

void SwitchBoardEngine::connectSocket()
{
	m_socket->connectToHost(m_host, m_port);
}

/* Slots */
void SwitchBoardEngine::socketConnected()
{
	qDebug() << "socketConnected()";
	//m_socket->write(QString("show hints\n").toAscii());
	m_socket->write(QString("hints\n").toAscii());
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
	while(m_socket->canReadLine())
	{
		QByteArray data = m_socket->readLine();
		//qDebug() << data;
		QString line(data);
		QStringList list = line.trimmed().split(",");
		qDebug() << list[0] << list[1] << list[2] << list[3];
		if(m_window)
			m_window->updatePeer(list[0], list[1], list[2]);
	}
}

void SwitchBoardEngine::finishedReceivingHints()
{
	qDebug() << "finishedReceivingHints()";
}

void SwitchBoardEngine::timerEvent(QTimerEvent * event)
{
	qDebug() << event;
	if(m_socket->state() == QAbstractSocket::UnconnectedState)
		connectSocket();
}
