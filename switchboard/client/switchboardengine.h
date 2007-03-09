#ifndef __SWITCHBOARDENGINE_H__
#define __SWITCHBOARDENGINE_H__
#include <QObject>
#include <QTcpSocket>

class QTimer;
class SwitchBoardWindow;

class SwitchBoardEngine: public QObject
{
	Q_OBJECT
public:
	SwitchBoardEngine(QObject * parent = 0);
	void setWindow(SwitchBoardWindow *);
	void setAddress(const QString & host, quint16 port);
	void originateCall(const QString & src, const QString & dst);
	void transferCall(const QString & src, const QString & dst);
	void hangUp(const QString & peer);
private:
	void connectSocket();
	void finishedReceivingHints();
protected:
	void timerEvent(QTimerEvent *event);
private slots:
	void socketConnected();
	void socketDisconnected();
	void socketHostFound();
	void socketError(QAbstractSocket::SocketError);
	void socketStateChanged(QAbstractSocket::SocketState);
	void socketReadyRead();
signals:
	void emitTextMessage(const QString &);
private:
	QTcpSocket * m_socket;
	quint16 m_port;
	QString m_host;
	SwitchBoardWindow * m_window;
	QString m_pendingcommand;
};

#endif

