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
	SwitchBoardEngine(QObject * parent = 0);	//!< Constructor
	void setWindow(SwitchBoardWindow *);
	void setAddress(const QString & host, quint16 port);
	void originateCall(const QString & src, const QString & dst);
	void transferCall(const QString & src, const QString & dst);
	void hangUp(const QString & peer);
	quint16 port() const;			//!< server port
	const QString & host() const;	//!< Server host name
	void saveSettings();	//!< save settings
private:
	void connectSocket();
	void finishedReceivingHints();
	void loadSettings();	//!< load settings
protected:
	void timerEvent(QTimerEvent *event);
public slots:
	void start();	//!< Start the engine
	void stop();	//!< Stop the engine, disconnect from server.
private slots:
	void socketConnected();
	void socketDisconnected();
	void socketHostFound();
	void socketError(QAbstractSocket::SocketError);
	void socketStateChanged(QAbstractSocket::SocketState);
	void socketReadyRead();
signals:
	void started();
	void stopped();
	void emitTextMessage(const QString &);
	void updateCall(const QString &, const QString &,
	                const QString &, const QString &);
	void endCall(const QString &);
	void updatePeer(const QString &, const QString &,
	                const QString &, const QString &);
	void removePeer(const QString &);
private:
	QTcpSocket * m_socket;
	int m_timer;
	quint16 m_port;
	QString m_host;
	SwitchBoardWindow * m_window;
	QString m_pendingcommand;
};

#endif

