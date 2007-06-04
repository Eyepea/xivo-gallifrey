#ifndef __LOGINENGINE_H__
#define __LOGINENGINE_H__
#include <QHash>
#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QUdpSocket>

class QTimer;
class QDateTime;

class LoginEngine: public QObject
{
	Q_OBJECT
public:
	//! Enum for Engine state logged/not logged
	typedef enum {ENotLogged, ELogged } EngineState;
	
	LoginEngine(QObject * parent = 0);
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
	void setPassword(const QString & pass) { m_passwd = pass; };
	const QString & password() const { return m_passwd; };
	void setAvailstate(const QString & availstate) { m_availstate = availstate; };
	const QString & availstate() const { return m_availstate; };

	const EngineState state() const;	//!< Engine state (Logged/Not Logged)
	void setState(EngineState state);	//!< see state()
	uint trytoreconnectinterval() const;	//!< try to reconnect interval
	void setTrytoreconnectinterval(uint);	//!< set try to reconnect interval
	const QString & getAvailState() const {return m_availstate;} //!< returns availability status
private:
	void connectSocket();
	void loadSettings();	//!< load settings
protected:
	void timerEvent(QTimerEvent *event);
signals:
	void logged();	//!< signal emitted when the state becomes ELogged
	void delogged();	//!< signal emitted when the state becomes ENotLogged
public slots:
	void start();
	void stop();
	void setAvailable();	//!< set user status as "available"
	void setAway();			//!< set user status as "away"
	void setBeRightBack();	//!< set user status as "be right back"
	void setOutToLunch();	//!< set user status as "out to lunch"
	void setDoNotDisturb();	//!< set user status as "do not disturb"
private slots:
	void identifyToTheServer();	//!< perform the first login step
	void processLoginDialog();	//!< perform the following login steps
	void keepLoginAlive();		//!< Send a UDP datagram to keep session alive
	void readKeepLoginAliveDatagrams();	//!< handle the responses to keep alive
	void setKeepaliveinterval(uint);	//!< set keep alive interval
signals:
	void started();
	void stopped();
	void emitTextMessage(const QString &);
private:
	void stopKeepAliveTimer();	//!< Stop the keep alive timer if running
	void startTryAgainTimer();	//!< Start the "try to reconnect" timer
	void stopTryAgainTimer();	//!< Stop the "try to reconnect" timer
	void setAvailState(const QString &);	//!< set Availability state

	QTcpSocket * m_socket;	//!< socket to connect to the server
	QTcpSocket * m_loginsocket;	//!< socket to login to the server
	int m_timer;	//!< timer id
	QString m_serverhost;	//!< server host name
	quint16 m_loginport;	//!< port to login to server
	bool m_autoconnect;	//!< Autoconnect to server at startup ?
	// poste à utiliser pour les commandes "DIAL"
	QHostAddress m_serveraddress;	//!< Resolved address of the login server
	QString m_asterisk;
	QString m_protocol;
	QString m_extension;
	QString m_passwd;	//!< password for account
	QString m_dialcontext;
	QString m_availstate;	//!< Availability state to send to the server
	QString m_sessionid;	//!< Session id obtained after a successful login
	QString m_capabilities;	//!< List of capabilities issued by the server after a successful login
	QString m_context;	//!< Context of the phone, as returned by the xivo_daemon server
	EngineState m_state;	//!< State of the engine (Logged/Not Logged)
	uint m_keepaliveinterval;	//!< Keep alive interval (in msec)
	uint m_trytoreconnectinterval;	//!< Try to reconnect interval (in msec)
	bool m_trytoreconnect;	//!< "try to reconnect" flag
	int m_ka_timerid;			//!< timer id for keep alive
	int m_try_timerid;			//!< timer id for try to reconnect
	QUdpSocket m_udpsocket;		//!< UDP socket used for keep alive
	int m_pendingkeepalivemsg;	//!< number of keepalivemsg sent without response
};

#endif

