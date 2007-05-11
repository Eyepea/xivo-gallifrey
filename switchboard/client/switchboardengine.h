#ifndef __SWITCHBOARDENGINE_H__
#define __SWITCHBOARDENGINE_H__
#include <QHash>
#include <QObject>
#include <QTcpSocket>

class QTimer;
class QDateTime;

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
	void setAsterisk(const QString & ast) { m_asterisk = ast; };
	const QString & asterisk() const { return m_asterisk; };
	void setProtocol(const QString & proto) { m_protocol = proto; };
	const QString & protocol() const { return m_protocol; };
	void setExtension(const QString & ext) { m_extension = ext; };
	const QString & extension() const { return m_extension; };
	void setDialContext(const QString & context) { m_dialcontext = context; };
	const QString & dialContext() const { return m_dialcontext; };
private:
	void connectSocket();
	void loadSettings();	//!< load settings
	void sendCommand();
	void processHistory(const QStringList &);
protected:
	void timerEvent(QTimerEvent *event);
public slots:
	void start();
	void stop();
	void originateCall(const QString & src, const QString & dst);
	void transferCall(const QString & src, const QString & dst);
	void dial(const QString & dst);
	void searchDirectory(const QString &);
	void requestHistory(const QString &);
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
			int time,
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
	void updateLogEntry(const QDateTime &, int, const QString &, int);
	void directoryResponse(const QString &);
private:
	QTcpSocket * m_socket;	//!< socket to connect to the server
	int m_timer;	//!< timer id
	quint16 m_port;	//!< port to connect to server
	QString m_host;	//!< server host name
	bool m_autoconnect;	//!< Autoconnect to server at startup ?
	QString m_pendingcommand;	//!< command to be sent to the server.
	QHash<QString, QString> m_callerids;
	// poste à utiliser pour les commandes "DIAL"
	QString m_asterisk;
	QString m_protocol;
	QString m_extension;
	QString m_dialcontext;
};

#endif

