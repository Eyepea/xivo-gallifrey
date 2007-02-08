/* Author : Thomas Bernard
 * Proformatique */
#ifndef __ENGINE_H__
#define __ENGINE_H__

// QT includes.
#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QUdpSocket>
#include <QTimer>

//! Profile popup engine
/*! The engine object is containing all the code to
 *  handle network connection and requests */
class Engine: public QObject
{
	Q_OBJECT
public:
	//! Enum for Engine state logged/not logged
	typedef enum {ENotLogged, ELogged } EngineState;
	//! Constructor
	Engine(QObject *parent = 0);

	void loadSettings();		//!< load server settings
	void saveSettings();		//!< save server settings

	// setter/getter for properties
	const QString & serverip() const;	//!< Host of the login server
	void setServerip(const QString &);	//!< see serverip()
	ushort serverport() const;			//!< TCP port for connection to server
	void setServerport(ushort);			//!< see serverport()
	const QString & login() const;		//!< login to identify to the server
	void setLogin(const QString &);		//!< see login()
	const QString & passwd() const;		//!< password to identify to the sever
	void setPasswd(const QString &);	//!< see passwd()
	const EngineState state() const;	//!< Engine state (Logged/Not Logged)
	void setState(EngineState state);	//!< see state()
signals:
	void logged();	//!< signal emmited when the state becomes ELogged
	void delogged();	//!< signal emmited when the state becomes ENotLogged
public slots:
	void start();	//!< start the connection process.
private slots:
	void identifyToTheServer();	//!< perform the first login step
	void processLoginDialog();	//!< perform the following login steps
	void handleProfilePush();	//!< called when receiving a profile
	void serverHostFound();		//!< called when the host name resolution succeded
	void keepLoginAlive();		//!< Send a UDP datagram to keep session alive
	void readKeepLoginAliveDatagrams();	//!< handle the responses to keep alive
	void popupDestroyed(QObject *);
private:
	void initListenSocket();	//!< initialize the socket listening to profile

	// parameters to connect to server
	QString m_serverip;		//!< Host to the login server
	ushort m_serverport;	//!< TCP port (UDP port for keep alive is +1)
	QString m_login;		//!< User login
	QString m_passwd;		//!< User password
	// 
	QHostAddress m_serveraddress;	//!< Resolved address of the login server
	QTcpSocket m_loginsocket;	//!< TCP socket used to login
	ushort m_listenport;		//!< Port where we are listening for profiles
	QTimer m_timer;				//!< timer object for keep alive
	QUdpSocket m_udpsocket;		//!< UDP socket used for keep alive
	QTcpServer m_listensocket;	//!< TCP socket listening for profiles
	QString m_sessionid;	//!< Session id obtained after a successfull login
	EngineState m_state;	//!< State of the engine (Logged/Not Logged)
};

#endif

