/* $Id$ */
#ifndef __SWITCHBOARDENGINE_H__
#define __SWITCHBOARDENGINE_H__
#include <QHash>
#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QUdpSocket>

class QTimer;
class QDateTime;

class SwitchBoardEngine: public QObject
{
	Q_OBJECT
public:
	//! Enum for Engine state logged/not logged
	typedef enum {ENotLogged, ELogged } EngineState;
	
	SwitchBoardEngine(QObject * parent = 0);
	void setAddress(const QString & host, quint16 port);
	quint16 sbport() const;
	quint16 loginport() const;
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
signals:
	void logged();	//!< signal emitted when the state becomes ELogged
	void delogged();	//!< signal emitted when the state becomes ENotLogged
public slots:
	void start();
	void stop();
	void originateCall(const QString & src, const QString & dst);
	void dialFullChannel(const QString & dst);
	void dialExtension(const QString & dst);
	void transferCall(const QString & src, const QString & dst);
	void interceptCall(const QString & src);
	void searchDirectory(const QString &);
	void requestHistory(const QString &, int);
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
	void updateMyCalls(const QStringList &, const QStringList &, const QStringList &);
	void endCall(const QString &);
	//void showCalls(const QString & tomonitor, const QString & callerid);
	void callsUpdated();
	void updatePeer(const QString &, const QString &,
	                const QString &, const QString &,
	                const QString &, const QString &,
	                const QStringList &, const QStringList &, const QStringList &);
	void removePeer(const QString &);
	void updateLogEntry(const QDateTime &, int, const QString &, int);
	void directoryResponse(const QString &);
private:
	QTcpSocket * m_socket;	//!< socket to connect to the server
	int m_timer;	//!< timer id
	QString m_serverhost;	//!< server host name
	quint16 m_sbport;	//!< port to connect to server
	quint16 m_loginport;	//!< port to login to server
	bool m_autoconnect;	//!< Autoconnect to server at startup ?
	QString m_pendingcommand;	//!< command to be sent to the server.
	QHash<QString, QString> m_callerids;
	// poste à utiliser pour les commandes "DIAL"
	QHostAddress m_serveraddress;	//!< Resolved address of the login server
	QString m_asterisk;
	QString m_protocol;
	QString m_extension;
	QString m_dialcontext;	//!< Context of the phone, as returned by the xivo_daemon server
	QString m_sessionid;	//!< Session id obtained after a successful login
	QString m_capabilities;	//!< List of capabilities issued by the server after a successful login
};

#endif

