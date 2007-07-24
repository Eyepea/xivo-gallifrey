/*
XIVO customer information client : popup profile for incoming calls
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

#ifndef __BASEENGINE_H__
#define __BASEENGINE_H__

// QT includes.
#include <QHash>
#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QUdpSocket>
#include <QTime>
#include <QTimer>

class Popup;

//! Profile popup engine
/*! The engine object is containing all the code to
 *  handle network connection and requests */
class BaseEngine: public QObject
{
	Q_OBJECT
public:
	//! Enum for BaseEngine state logged/not logged
	typedef enum {ENotLogged, ELogged } EngineState;
	//! Constructor
	BaseEngine(QObject *parent = 0);
	void loadSettings();		//!< load server settings
	void saveSettings();		//!< save server settings

	// setter/getter for properties
	const QString & serverip() const;	//!< Host of the login server
	void setServerip(const QString &);	//!< see serverip()
	const QString & serverast() const;	//!< id Name of the Asterisk server
	void setServerAst(const QString &);	//!< see serverast()

	ushort serverport() const;			//!< TCP port for connection to server
	void setServerport(ushort);			//!< see serverport()
	const QString & protocol() const;      	//!< protocol of the user to identify to the server
	void setProtocol(const QString &);     	//!< see protocol()
	const QString & userid() const;		//!< userid to identify to the server
	void setUserId(const QString &);       	//!< see userid()
	const QString & passwd() const;		//!< password to identify to the sever
	void setPasswd(const QString &);	//!< see passwd()
	const EngineState state() const;	//!< Engine state (Logged/Not Logged)
	void setState(EngineState state);	//!< see state()
	bool autoconnect() const;			//!< auto connect flag
	void setAutoconnect(bool b);		//!< set auto connect flag
	bool trytoreconnect() const;		//!< try to reconnect flag
	void setTrytoreconnect(bool b);		//!< set try to reconnect flag
	uint trytoreconnectinterval() const;	//!< try to reconnect interval
	void setTrytoreconnectinterval(uint);	//!< set try to reconnect interval
	uint keepaliveinterval() const;		//!< keep alive interval
	void setKeepaliveinterval(uint);	//!< set keep alive interval
	bool tcpmode() const { return m_tcpmode; };	//!< get tcp mode flag
	void setTcpmode(bool b) { m_tcpmode = b; };	//!< set tcp mode flag
	const QString & getAvailState() const {return m_availstate;} //!< returns availability status
	void sendMessage(const QString &);      //!< Sends an instant message
	const QString & getCapabilities() const {return m_capabilities;} //!< returns capabilities
	uint historysize() const;	//!< history size
	void setHistorySize(uint size);	//!< set history size
	//! set m_enabled
	void setEnabled(bool b);
	//! get m_enabled
	bool enabled() { return m_enabled; };
	void setIsASwitchboard(bool);
	bool isASwitchboard();
	void deleteRemovables();
	void addRemovable(const QMetaObject * metaobject);
	bool isRemovable(const QMetaObject * metaobject);
signals:
	void availAllowChanged(bool);	//!< signal 
	void logged();	//!< signal emitted when the state becomes ELogged
	void delogged();	//!< signal emitted when the state becomes ENotLogged
	void newProfile(Popup *);	//!< signal emitted when a new profile has to be shown
	//! a log entry has to be updated.
	void updateLogEntry(const QDateTime &, int, const QString &, int);
	//! the directory search response has been received.
	void directoryResponse(const QString &);
	void voiceMailChanged(bool);
	void callRecordingChanged(bool);
	void callFilteringChanged(bool);
	void dndChanged(bool);
	void uncondForwardChanged(bool, const QString &);
	void uncondForwardChanged(bool);
	void uncondForwardChanged(const QString &);
	void forwardOnBusyChanged(bool, const QString &);
	void forwardOnBusyChanged(bool);
	void forwardOnBusyChanged(const QString &);
	void forwardOnUnavailableChanged(bool, const QString &);
	void forwardOnUnavailableChanged(bool);
	void forwardOnUnavailableChanged(const QString &);
	void updatePeer(const QString & ext,
	                const QString & name,
			const QString & imavail,
			const QString & sipstatus,
			const QString & vmstatus,
			const QString & queuestatus,
			const QStringList & chanIds,
			const QStringList & chanStates,
			const QStringList & chanOthers);
public slots:
	void start();	//!< start the connection process.
	void stop();	//!< stop the engine
	void setAvailable();	//!< set user status as "available"
	void setAway();			//!< set user status as "away"
	void setBeRightBack();	//!< set user status as "be right back"
	void setOutToLunch();	//!< set user status as "out to lunch"
	void setDoNotDisturb();	//!< set user status as "do not disturb"
	void searchDirectory(const QString &);
	void requestHistory(const QString &, int);
	void requestPeers(void);
	void sendCommand(const QString &);
	void setVoiceMail(bool);
	void setCallRecording(bool);
	void setCallFiltering(bool);
	void setDnd(bool);
	void setUncondForward(bool, const QString &);
	void setForwardOnBusy(bool, const QString &);
	void setForwardOnUnavailable(bool, const QString &);
	void askFeatures();
	void askPeers();
	void askCallerIds();
private slots:
	void identifyToTheServer();	//!< perform the first login step
	void processLoginDialog();	//!< perform the following login steps
	void handleProfilePush();	//!< called when receiving a profile
	void serverHostFound();		//!< called when the host name resolution succeded
	void keepLoginAlive();		//!< Send a UDP datagram to keep session alive
	void readKeepLoginAliveDatagrams();	//!< handle the responses to keep alive
	void popupDestroyed(QObject *);	//!< know when a profile widget is destroyed *DEBUG*
	void profileToBeShown(Popup *);	//!< a new profile must be displayed

	void dialExtension(const QString &);
	void dialFullChannel(const QString &);
	void updatePeers(const QStringList &);
protected:
	void timerEvent(QTimerEvent *);	//!< recieve timer events
private:
	void initListenSocket();	//!< initialize the socket listening to profile
	void stopKeepAliveTimer();	//!< Stop the keep alive timer if running
	void startTryAgainTimer();	//!< Start the "try to reconnect" timer
	void stopTryAgainTimer();	//!< Stop the "try to reconnect" timer
	void setAvailState(const QString &);	//!< set Availability state
	void processHistory(const QStringList &);
	void initFeatureFields(const QString &, const QString &);
	void updateCallerids(const QStringList &);

	// parameters to connect to server
	QString m_serverip;		//!< Host to the login server
	ushort m_loginport;	//!< TCP port (UDP port for keep alive is +1)
	bool m_tcpmode;	//!< use a unique outgoing TCP connection for everything
	QString m_asterisk;		//!< Host to the login server
	QString m_protocol;		//!< User Protocol's login
	QString m_userid;		//!< User Id
	bool m_enabled;		       	//!< is enabled
	QString m_passwd;		//!< User password
	uint m_keepaliveinterval;	//!< Keep alive interval (in msec)
	// 
	bool m_autoconnect;		//!< Auto connect flag
	bool m_trytoreconnect;	//!< "try to reconnect" flag
	uint m_trytoreconnectinterval;	//!< Try to reconnect interval (in msec)
	//
	int m_historysize;
	QHostAddress m_serveraddress;	//!< Resolved address of the login server
	QTcpSocket * m_loginsocket;	//!< TCP socket used to login
	QUdpSocket * m_udpsocket;      	//!< UDP socket used for keep alive
	QTcpServer * m_listensocket;	//!< TCP socket listening for profiles
	ushort m_listenport;		//!< Port where we are listening for profiles
	int m_ka_timerid;			//!< timer id for keep alive
	int m_try_timerid;			//!< timer id for try to reconnect
	QString m_sessionid;	//!< Session id obtained after a successful login
	QString m_capabilities;	//!< List of capabilities issued by the server after a successful login
	int m_version;	//!< Version issued by the server after a successful login
	QString m_dialcontext;	//!< Context of the phone, as returned by the xivo_daemon server
	EngineState m_state;	//!< State of the engine (Logged/Not Logged)
	int m_pendingkeepalivemsg;	//!< number of keepalivemsg sent without response
	QString m_availstate;	//!< Availability state to send to the server
	QHash<QString, QString> m_callerids;	//!< List of caller Ids
	// GUI client capabilities
	QList<const QMetaObject *> m_removable;
	bool m_is_a_switchboard;
};

#endif

