#ifndef __SWITCHBOARDENGINE_H__
#define __SWITCHBOARDENGINE_H__
#include <QHash>
#include <QObject>
#include <QTcpSocket>

class QTimer;

class SwitchBoardEngine: public QObject
{
	Q_OBJECT
public:
	SwitchBoardEngine(QObject * parent = 0);
	void setAddress(const QString & host, quint16 port);
	quint16 port() const;
	const QString & host() const;
	void setAutoconnect(bool b) { m_autoconnect = b;};
	bool autoconnect() const {return m_autoconnect;};
	void saveSettings();	//!< save settings
private:
	void connectSocket();
	void loadSettings();	//!< load settings
	void sendCommand();
protected:
	void timerEvent(QTimerEvent *event);
public slots:
	void start();
	void stop();
	void originateCall(const QString & src, const QString & dst);
	void transferCall(const QString & src, const QString & dst);
private slots:
	void updatePeers(const QStringList & liststatus);
	void updateCallerids(const QStringList & liststatus);
	void socketConnected();
	void socketDisconnected();
	void socketHostFound();
	void socketError(QAbstractSocket::SocketError);
	void socketStateChanged(QAbstractSocket::SocketState);
	void socketReadyRead();
	void hangUp(const QString & peer);
signals:
	void started();
	void stopped();
	void emitTextMessage(const QString &);
	void updateCall(const QString & channelme,
			const QString & action,
			const int & time,
			const QString & direction,
			const QString & channelpeer,
			const QString & exten,
			const QString & phone);
	void endCall(const QString &);
	//void showCalls(const QString & tomonitor, const QString & callerid);
	void callsUpdated();
	void updatePeer(const QString &, const QString &,
	                const QString &, const QString &,
					const QString &);
	void removePeer(const QString &);
private:
	QTcpSocket * m_socket;	//!< socket to connect to the server
	int m_timer;	//!< timer id
	quint16 m_port;	//!< port to connect to server
	QString m_host;	//!< server host name
	bool m_autoconnect;	//!< Autoconnect to server at startup ?
	QString m_pendingcommand;	//!< command to be sent to the server.
	QHash<QString, QString> m_callerids;
};

#endif

