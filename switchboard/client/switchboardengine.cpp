#include "switchboardengine.h"

SwitchBoardEngine::SwitchBoardEngine(QObject * parent)
: QObject(parent)
{
	m_socket = new QTcpSocket(this);
}

void SwitchBoardEngine::connectSocket()
{
	m_socket->connectToHost();
}

