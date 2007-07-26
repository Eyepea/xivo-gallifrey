/*
XIVO switchboard
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

class QTimer;
class QDateTime;

/*! \brief Class which handle connection with the Xivo CTI server
 */
class BaseEngine: public QObject
{
	Q_OBJECT
public:
	//! Enum for Engine state logged/not logged
	typedef enum {ENotLogged, ELogged } EngineState;
	
	BaseEngine(QObject * parent = 0);
	//! set address used to connect to the server
	void setAddress(const QString & host, quint16 port);
	quint16 sbport() const;
//	quint16 loginport() const;
	const QString & host() const;
	void setAutoconnect(bool b) { m_autoconnect = b;}; //! set m_autoconnect
	bool autoconnect() const {return m_autoconnect;};  //! get m_autoconnect
	bool trytoreconnect() const;		//!< try to reconnect flag
	void setTrytoreconnect(bool b);		//!< set try to reconnect flag
	uint trytoreconnectinterval() const;	//!< try to reconnect interval
	void setTrytoreconnectinterval(uint);	//!< set try to reconnect interval
	uint historysize() const;	//!< history size
	void setHistorySize(uint size);	//!< set history size

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
	//void setExtension(const QString & ext) { m_extension = ext; };
	//! get m_extension
	//const QString & extension() const { return m_extension; };
	//! set m_userid
	void setUserId(const QString & userid) { m_userid = userid; };
	//! get m_userid
	const QString & userId() const { return m_userid; };
	//! set m_dialcontext
	void setDialContext(const QString & context) { m_dialcontext = context; };
	//! get m_dialcontext
	const QString & dialContext() const { return m_dialcontext; };
	void setIsASwitchboard(bool);
	bool isASwitchboard();
	void deleteRemovables();
	void addRemovable(const QMetaObject * metaobject);
	bool isRemovable(const QMetaObject * metaobject);
protected:
	void timerEvent(QTimerEvent *event);
public slots:
	void start();
	void stop();
	void originateCall(const QString &, const QString &);
	void dialFullChannel(const QString &);
	void dialExtension(const QString &);
	void transferCall(const QString &, const QString &);
	void interceptCall(const QString &);
	void searchDirectory(const QString &);
	void requestHistory(const QString &, int);
        void transferToNumber(const QString &);
        void textEdited(const QString &);
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
	void logged();	//!< signal emitted when the state becomes ELogged
	void delogged();	//!< signal emitted when the state becomes ENotLogged
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
	//! we want to monitor a given peer (not the one given by the mouse's drag&drop).
	void monitorPeer(const QString &, const QString &);
private:
	void connectSocket();
	void loadSettings();	//!< load settings
	void sendCommand();
	void processHistory(const QStringList &);
	void startTryAgainTimer();	//!< Start the "try to reconnect" timer
	void stopTryAgainTimer();	//!< Stop the "try to reconnect" timer

	QTcpSocket * m_socket;	//!< socket to connect to the server
	int m_timer;	//!< timer id
	int m_historysize;
	QString m_serverhost;	//!< server host name
	quint16 m_sbport;	//!< port to connect to server
	//quint16 m_loginport;	//!< port to login to server
	bool m_autoconnect;	//!< Autoconnect to server at startup
	bool m_trytoreconnect;	//!< "try to reconnect" flag
	uint m_trytoreconnectinterval;	//!< Try to reconnect interval (in msec)
	int m_try_timerid;			//!< timer id for try to reconnect
	QString m_pendingcommand;	//!< command to be sent to the server.
	QHash<QString, QString> m_callerids;	//!< List of caller Ids
	// poste à utiliser pour les commandes "DIAL"
	QHostAddress m_serveraddress;	//!< Resolved address of the login server
	QString m_asterisk;		//!< asterisk server id of "my phone"
	QString m_protocol;		//!< protocol (SIP/IAX/ZAP...) for "my phone"
	QString m_extension;	//!< extension for "my phone"
	QString m_userid;		//!< userid
	QString m_dialcontext;	//!< Context of the phone, as returned by the xivo_daemon server
	QString m_sessionid;	//!< Session id obtained after a successful login
	QString m_capabilities;	//!< List of capabilities issued by the server after a successful login
        QString m_numbertodial;
	// GUI client capabilities
	QList<const QMetaObject *> m_removable;
	bool m_is_a_switchboard; 
};

#endif

