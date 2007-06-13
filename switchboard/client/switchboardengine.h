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

/*! \brief Class which handle connection with the Xivo CTI server
 */
class SwitchBoardEngine: public QObject
{
	Q_OBJECT
public:
	//! Enum for Engine state logged/not logged
	typedef enum {ENotLogged, ELogged } EngineState;
	
	SwitchBoardEngine(QObject * parent = 0);
	//! set address used to connect to the server
	void setAddress(const QString & host, quint16 port);
	quint16 sbport() const;
//	quint16 loginport() const;
	const QString & host() const;
	//! set m_autoconnect
	void setAutoconnect(bool b) { m_autoconnect = b;};
	//! get m_autoconnect
	bool autoconnect() const {return m_autoconnect;};
	//! save settings
	void saveSettings();
	//! set m_asterisk
	void setAsterisk(const QString & ast) { m_asterisk = ast; };
	//! get m_asterisk
	const QString & asterisk() const { return m_asterisk; };
	//! set m_protocol
	void setProtocol(const QString & proto) { m_protocol = proto; };
	//! get m_protocol
	const QString & protocol() const { return m_protocol; };
	//! set m_extension
	void setExtension(const QString & ext) { m_extension = ext; };
	//! get m_extension
	const QString & extension() const { return m_extension; };
	//! set m_dialcontext
	void setDialContext(const QString & context) { m_dialcontext = context; };
	//! get m_dialcontext
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
	//! connected to the server
	void started();
	//! disconnected from the server
	void stopped();
	//! message to be displayed to the user.
	void emitTextMessage(const QString &);
	//! a call
	void updateCall(const QString & channelme,
			const QString & action,
			int time,
			const QString & direction,
			const QString & channelpeer,
			const QString & exten,
			const QString & phone);
	//! "my" calls are updated
	void updateMyCalls(const QStringList &, const QStringList &, const QStringList &);
	//! useless ?
	void endCall(const QString &);
	//void showCalls(const QString & tomonitor, const QString & callerid);
	//! call list is updated
	void callsUpdated();
	//! update informations about a peer
	void updatePeer(const QString &, const QString &,
	                const QString &, const QString &,
	                const QString &, const QString &,
	                const QStringList &, const QStringList &, const QStringList &);
	//! the server requested a peer remove
	void removePeer(const QString &);
	//! a log entry has to be updated.
	void updateLogEntry(const QDateTime &, int, const QString &, int);
	//! the directory search response has been received.
	void directoryResponse(const QString &);
private:
	QTcpSocket * m_socket;	//!< socket to connect to the server
	int m_timer;	//!< timer id
	QString m_serverhost;	//!< server host name
	quint16 m_sbport;	//!< port to connect to server
	//quint16 m_loginport;	//!< port to login to server
	bool m_autoconnect;	//!< Autoconnect to server at startup
	QString m_pendingcommand;	//!< command to be sent to the server.
	QHash<QString, QString> m_callerids;	//!< List of caller Ids
	// poste à utiliser pour les commandes "DIAL"
	QHostAddress m_serveraddress;	//!< Resolved address of the login server
	QString m_asterisk;		//!< asterisk server id of "my phone"
	QString m_protocol;		//!< protocol (SIP/IAX/ZAP...) for "my phone"
	QString m_extension;	//!< extension for "my phone"
	QString m_dialcontext;	//!< Context of the phone, as returned by the xivo_daemon server
	QString m_sessionid;	//!< Session id obtained after a successful login
	QString m_capabilities;	//!< List of capabilities issued by the server after a successful login
};

#endif

