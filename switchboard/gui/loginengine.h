/* XIVO switchboard
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

/* $Id$ */
#ifndef __LOGINENGINE_H__
#define __LOGINENGINE_H__
#include <QHash>
#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QUdpSocket>

class QTimer;
class QDateTime;

/*! \brief login engine
 *
 * Connect to presence server and advertise state
 * (available, away, gone for lunch, do not disturb,
 * be right back).
 */
class LoginEngine: public QObject
{
	Q_OBJECT
public:
	//! Enum for Engine state logged/not logged
	typedef enum {ENotLogged, ELogged } EngineState;
	
	LoginEngine(QObject * parent = 0);
	const QString & host() const;
	//! set m_autoconnect
	void setAutoconnect(bool b) { m_autoconnect = b;};
	//! get m_autoconnect
	bool autoconnect() const {return m_autoconnect;};
	void saveSettings();	//!< save settings
	
	void setAddress(const QString & host) { m_serverhost = host; };	//! set m_serverhost
	const QString & address() const { return m_serverhost; };	//! get m_serverhost
	void setAsterisk(const QString & ast) { m_asterisk = ast; };	//! set m_asterisk
	const QString & asterisk() const { return m_asterisk; };	//! get m_asterisk
	//! set m_protocol
	void setProtocol(const QString & proto) { m_protocol = proto; };
	//! get m_protocol
	const QString & protocol() const { return m_protocol; };
	//! set m_loginport
	void setLoginPort(const quint16 & loginport) { m_loginport = loginport; };
	//! get m_loginport
	const quint16 & loginPort() const { return m_loginport; };

	void setUserId(const QString & userid) { m_userid = userid; }; //! set m_userid
	const QString & userId() const { return m_userid; };           //! get m_userid
	//! set dial context ?
	void setDialContext(const QString & context) { m_dialcontext = context; };
	//! get dial context
	const QString & dialContext() const { return m_dialcontext; };
	//! set m_passwd
	void setPassword(const QString & pass) { m_passwd = pass; };
	//! get m_passwd
	const QString & password() const { return m_passwd; };
	//! set m_availstate
	void setAvailstate(const QString & availstate) { m_availstate = availstate; };
	//! get m_availstate
	const QString & availstate() const { return m_availstate; };
	
	const EngineState state() const;	//!< Engine state (Logged/Not Logged)
	void setState(EngineState state);	//!< see state()
	bool trytoreconnect() const;		//!< try to reconnect flag
	void setTrytoreconnect(bool b);		//!< set try to reconnect flag
	uint trytoreconnectinterval() const;	//!< try to reconnect interval
	void setTrytoreconnectinterval(uint);	//!< set try to reconnect interval
	//! returns availability status
	const QString & getAvailState() const {return m_availstate;}
	//! set m_enabled
	void setEnabled(bool b) { 
		if(b != m_enabled) {
			m_enabled = b;
			enabledChanged(b);
		}
	};
	//! get m_enabled
	bool enabled() { return m_enabled; };
private:
	//void connectSocket();
	void loadSettings();	//!< load settings
protected:
	void timerEvent(QTimerEvent *event);
signals:
	void enabledChanged(bool);	//!< signal 
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
	void started();		//!< emited when the engine is started
	void stopped();		//!< emited when the engine is stopped
	void emitTextMessage(const QString &);	//!< some message have to be emited
private:
	void stopKeepAliveTimer();	//!< Stop the keep alive timer if running
	void startTryAgainTimer();	//!< Start the "try to reconnect" timer
	void stopTryAgainTimer();	//!< Stop the "try to reconnect" timer
	void setAvailState(const QString &);	//!< set Availability state

	QTcpSocket * m_loginsocket;	//!< socket to login to the server
	QUdpSocket * m_udpsocket;		//!< UDP socket used for keep alive
	int m_timer;	//!< timer id
	QString m_serverhost;	//!< server host name
	quint16 m_loginport;	//!< port to login to server
	bool m_autoconnect;	//!< Autoconnect to server at startup ?
	// poste à utiliser pour les commandes "DIAL"
	QHostAddress m_serveraddress;	//!< Resolved address of the login server
	QString m_asterisk;		//!< asterisk id
	QString m_protocol;		//!< protocol (SIP/IAX)
	QString m_userid;	//!< phone userid
	bool m_enabled;		//!< is enabled
	QString m_passwd;	//!< password for account
	QString m_dialcontext;	//!< Context of the phone, as returned by the xivo_daemon server
	QString m_availstate;	//!< Availability state to send to the server
	QString m_sessionid;	//!< Session id obtained after a successful login
	QString m_capabilities;	//!< List of capabilities issued by the server after a successful login
	EngineState m_state;	//!< State of the engine (Logged/Not Logged)
	uint m_keepaliveinterval;	//!< Keep alive interval (in msec)
	bool m_trytoreconnect;	//!< "try to reconnect" flag
	uint m_trytoreconnectinterval;	//!< Try to reconnect interval (in msec)
	int m_ka_timerid;			//!< timer id for keep alive
	int m_try_timerid;			//!< timer id for try to reconnect
	int m_pendingkeepalivemsg;	//!< number of keepalivemsg sent without response
};

#endif

