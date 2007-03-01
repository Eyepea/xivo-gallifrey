#ifndef __SWITCHBOARDENGINE_H__
#define __SWITCHBOARDENGINE_H__
#include <QObject>
#include <QTcpSocket>

class SwitchBoardEngine: public QObject
{
	Q_OBJECT
public:
	SwitchBoardEngine(QObject * parent = 0);
private:
	QTcpSocket * m_socket;
};

#endif

