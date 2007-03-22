/*
kafiche client : popup profile for incoming calls
Copyright (C) 2007  Proformatique

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/
#ifndef __ENGINE_H__
#define __ENGINE_H__

// QT includes.
#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QUdpSocket>
#include <QTimer>

class Popup;

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
	bool autoconnect() const;			//!< auto connect flag
	void setAutoconnect(bool b);		//!< set auto connect flag
	bool trytoreconnect() const;		//!< try to reconnect flag
	void setTrytoreconnect(bool b);		//!< set try to reconnect flag
	uint keepaliveinterval() const;		//!< keep alive interval
	void setKeepaliveinterval(uint);	//!< set keep alive interval
	uint trytoreconnectinterval() const;	//!< try to reconnect interval
	void setTrytoreconnectinterval(uint);	//!< set try to reconnect interval
signals:
	void logged();	//!< signal emmited when the state becomes ELogged
	void delogged();	//!< signal emmited when the state becomes ENotLogged
	void newProfile(Popup *);	//!< signal emmited when a new profile has to be shown
public slots:
	void start();	//!< start the connection process.
	void stop();	//!< stop the engine
	void setAvailable();	//!< set user status as "available"
	void setAway();			//!< set user status as "away"
	void setDoesNotDisturb();	//!< set user status as "does not disturb"
private slots:
	void identifyToTheServer();	//!< perform the first login step
	void processLoginDialog();	//!< perform the following login steps
	void handleProfilePush();	//!< called when receiving a profile
	void serverHostFound();		//!< called when the host name resolution succeded
	void keepLoginAlive();		//!< Send a UDP datagram to keep session alive
	void readKeepLoginAliveDatagrams();	//!< handle the responses to keep alive
	void popupDestroyed(QObject *);	//!< know when a profile widget is destroyed *DEBUG*
	void profileToBeShown(Popup *);	//!< a new profile must be displayed
protected:
	void timerEvent(QTimerEvent *);	//!< recieve timer events
private:
	void initListenSocket();	//!< initialize the socket listening to profile
	void stopKeepAliveTimer();	//!< Stop the keep alive timer if running
	void startTryAgainTimer();	//!< Start the "try to reconnect" timer
	void stopTryAgainTimer();	//!< Stop tje "try to reconnect" timer

	// parameters to connect to server
	QString m_serverip;		//!< Host to the login server
	ushort m_serverport;	//!< TCP port (UDP port for keep alive is +1)
	QString m_login;		//!< User login
	QString m_passwd;		//!< User password
	uint m_keepaliveinterval;	//!< Keep alive interval (in msec)
	uint m_trytoreconnectinterval;	//!< Try to reconnect interval (in msec)
	// 
	bool m_autoconnect;		//!< Auto connect flag
	bool m_trytoreconnect;	//!< "try to reconnect" flag
	//
	QHostAddress m_serveraddress;	//!< Resolved address of the login server
	QTcpSocket m_loginsocket;	//!< TCP socket used to login
	ushort m_listenport;		//!< Port where we are listening for profiles
	int m_ka_timerid;			//!< timer id for keep alive
	int m_try_timerid;			//!< timer id for try to reconnect
	QUdpSocket m_udpsocket;		//!< UDP socket used for keep alive
	QTcpServer m_listensocket;	//!< TCP socket listening for profiles
	QString m_sessionid;	//!< Session id obtained after a successfull login
	EngineState m_state;	//!< State of the engine (Logged/Not Logged)
	int m_pendingkeepalivemsg;	//!< number of keepalivemsg sent without response
	QString m_availstate;	//!< Availability state to send to the server
};

#endif

