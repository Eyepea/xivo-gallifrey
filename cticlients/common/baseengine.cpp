/* XIVO CTI clients
 * Copyright (C) 2007, 2008  Proformatique
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Linking the Licensed Program statically or dynamically with other
 * modules is making a combined work based on the Licensed Program. Thus,
 * the terms and conditions of the GNU General Public License version 2
 * cover the whole combination.
 *
 * In addition, as a special exception, the copyright holders of the
 * Licensed Program give you permission to combine the Licensed Program
 * with free software programs or libraries that are released under the
 * GNU Library General Public License version 2.0 or GNU Lesser General
 * Public License version 2.1 or any later version of the GNU Lesser
 * General Public License, and with code included in the standard release
 * of OpenSSL under a version of the OpenSSL license (with original SSLeay
 * license) which is identical to the one that was published in year 2003,
 * or modified versions of such code, with unchanged license. You may copy
 * and distribute such a system following the terms of the GNU GPL
 * version 2 for the Licensed Program and the licenses of the other code
 * concerned, provided that you include the source code of that other code
 * when and as the GNU GPL version 2 requires distribution of source code.
*/

/* $Revision$
 * $Date$
 */

#include <QBuffer>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>
#include <QTcpSocket>
#include <QTime>
#include <QTimerEvent>

#include "baseengine.h"
#include "popup.h"
#include "xivoconsts.h"

/*! \brief Constructor.
 *
 * Construct the BaseEngine object and load settings.
 * The TcpSocket Object used to communicate with the server
 * is created and connected to the right slots/signals
 *
 * This constructor initialize the UDP socket and
 * the TCP listening socket.
 * It also connects signals with the right slots.
 */
BaseEngine::BaseEngine(QSettings * settings,
                       QObject * parent)
        : QObject(parent),
	  m_serverhost(""), m_loginport(0), m_sbport(0),
          m_company(""), m_protocol(""), m_userid(""), m_phonenumber(""), m_passwd(""),
          m_checked_presence(false), m_checked_cinfo(false),
          m_sessionid(""), m_state(ENotLogged),
          m_pendingkeepalivemsg(0)
{
	m_ka_timerid = 0;
	m_try_timerid = 0;
	m_timer = -1;
	m_sbsocket     = new QTcpSocket(this);
        m_faxsocket    = new QTcpSocket(this);
        m_settings = settings;
	loadSettings();
	deleteRemovables();

        /*  QTcpSocket signals :
            void connected ()
            void disconnected ()
            void error ( QAbstractSocket::SocketError socketError )
            void hostFound ()
            void stateChanged ( QAbstractSocket::SocketState socketState )
        */
        /* Signals inherited from QIODevice :
           void aboutToClose ()
           void bytesWritten ( qint64 bytes )
           void readyRead ()
        */

	// Socket for TCP connections
        m_sbsocket->setProperty("socket", "tcp_nat");
	connect( m_sbsocket, SIGNAL(connected()),
                 this, SLOT(socketConnected()));
	connect( m_sbsocket, SIGNAL(disconnected()),
                 this, SLOT(socketDisconnected()));
	connect( m_sbsocket, SIGNAL(hostFound()),
                 this, SLOT(socketHostFound()));
	connect( m_sbsocket, SIGNAL(error(QAbstractSocket::SocketError)),
                 this, SLOT(socketError(QAbstractSocket::SocketError)));
	connect( m_sbsocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                 this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
	connect( m_sbsocket, SIGNAL(readyRead()),
                 this, SLOT(socketReadyRead()));

        m_faxsocket->setProperty("socket", "fax");
        connect( m_faxsocket, SIGNAL(connected()),
                 this, SLOT(socketConnected()) );
	connect( m_faxsocket, SIGNAL(disconnected()),
                 this, SLOT(socketDisconnected()));
	connect( m_faxsocket, SIGNAL(hostFound()),
	         this, SLOT(socketHostFound()) );

	if(m_autoconnect)
		start();
}

/*! \brief Destructor
 */
BaseEngine::~BaseEngine()
{
        qDebug() << "BaseEngine::~BaseEngine()";
}

QSettings * BaseEngine::getSettings()
{
        return m_settings;
}

/*!
 * Load Settings from the registery/configuration file
 * Use default values when settings are not found.
 */
void BaseEngine::loadSettings()
{
        //qDebug() << "BaseEngine::loadSettings()";
	m_systrayed = m_settings->value("display/systrayed", false).toBool();

//         QStringList usergroups = m_settings->childGroups();
//         for(int i = 0 ; i < usergroups.size(); i++) {
//                 QString groupname = usergroups[i];
//                 if (groupname.startsWith("engine")) {
//                         qDebug() << "valid engine name =" << groupname;
//                 }
//         }

        m_settings->beginGroup("engine");
	m_serverhost = m_settings->value("serverhost", "192.168.0.254").toString();
	m_loginport  = m_settings->value("loginport", 5000).toUInt();
	m_sbport     = m_settings->value("serverport", 5003).toUInt();

        m_checked_presence = m_settings->value("fct_presence", true).toBool();
        m_checked_cinfo    = m_settings->value("fct_cinfo",    true).toBool();
        m_checked_autourl  = m_settings->value("fct_autourl",  true).toBool();

	m_userid      = m_settings->value("userid").toString();
	m_company     = m_settings->value("company").toString();
	m_passwd      = m_settings->value("passwd").toString();
	m_loginkind   = m_settings->value("loginkind", 0).toUInt();
	m_keeppass    = m_settings->value("keeppass", 0).toUInt();
	m_phonenumber = m_settings->value("phonenumber").toString();
        m_protocol = "sip";

	m_autoconnect = m_settings->value("autoconnect", false).toBool();
	m_trytoreconnect = m_settings->value("trytoreconnect", false).toBool();
	m_trytoreconnectinterval = m_settings->value("trytoreconnectinterval", 20*1000).toUInt();
	m_keepaliveinterval = m_settings->value("keepaliveinterval", 20*1000).toUInt();

	m_historysize = m_settings->value("historysize", 8).toUInt();
	m_contactssize = m_settings->value("contactssize", 45).toUInt();
	m_contactscolumns = m_settings->value("contactscolumns", 2).toUInt();
        m_checked_lastconnwins = m_settings->value("lastconnwins", false).toBool();

	m_availstate = m_settings->value("availstate", "available").toString();
        m_settings->endGroup();
}

/*!
 * Save Settings to the registery/configuration file
 */
void BaseEngine::saveSettings()
{
        //qDebug() << "BaseEngine::saveSettings()";
	m_settings->setValue("display/systrayed", m_systrayed);

        // m_settings->beginGroup("engine." + m_userid);
        m_settings->beginGroup("engine");
	m_settings->setValue("serverhost", m_serverhost);
	m_settings->setValue("loginport",  m_loginport);
	m_settings->setValue("serverport", m_sbport);

	m_settings->setValue("fct_presence", m_checked_presence);
	m_settings->setValue("fct_cinfo",    m_checked_cinfo);
	m_settings->setValue("fct_autourl",  m_checked_autourl);

	m_settings->setValue("userid",     m_userid);
	m_settings->setValue("company",    m_company);
	m_settings->setValue("passwd",     m_passwd);
	m_settings->setValue("loginkind",  m_loginkind);
	m_settings->setValue("keeppass",   m_keeppass);
	m_settings->setValue("phonenumber", m_phonenumber);

	m_settings->setValue("autoconnect", m_autoconnect);
	m_settings->setValue("trytoreconnect", m_trytoreconnect);
	m_settings->setValue("trytoreconnectinterval", m_trytoreconnectinterval);
	m_settings->setValue("keepaliveinterval", m_keepaliveinterval);

	m_settings->setValue("historysize", m_historysize);
	m_settings->setValue("contactssize", m_contactssize);
	m_settings->setValue("contactscolumns", m_contactscolumns);
        m_settings->setValue("lastconnwins", m_checked_lastconnwins);

	m_settings->setValue("availstate", m_availstate);
        m_settings->endGroup();
}

/*!
 *
 */
void BaseEngine::setCheckedPresence(bool b) {
	if(b != m_checked_presence) {
		m_checked_presence = b;
		if((state() == ELogged) && m_enabled_presence) {
			availAllowChanged(b);
                }
	}
}

/*!
 *
 */
bool BaseEngine::checkedPresence() {
        return m_checked_presence;
}

/*!
 *
 */
void BaseEngine::setCheckedCInfo(bool b) {
	if(b != m_checked_cinfo)
		m_checked_cinfo = b;
}

/*!
 *
 */
bool BaseEngine::checkedCInfo() {
        return m_checked_cinfo;
}

/*!
 *
 */
void BaseEngine::setCheckedAutoUrl(bool b) {
	if(b != m_checked_autourl)
		m_checked_autourl = b;
}

/*!
 *
 */
bool BaseEngine::checkedAutoUrl() {
        return m_checked_autourl;
}

/*!
 *
 */
void BaseEngine::setEnabledCInfo(bool b) {
        m_enabled_cinfo = b;
}

/*!
 *
 */
bool BaseEngine::enabledCInfo() {
        return m_enabled_cinfo;
}


void BaseEngine::config_and_start(const QString & login,
                                  const QString & pass,
                                  const QString & phonenum)
{
        m_userid = login;
        m_passwd = pass;
        // if phonenum's size is 0, no login as agent
        m_phonenumber = phonenum;
        saveSettings();
        start();
}


/*! \brief Starts the connection to the server
 * This method starts the login process by connection
 * to the server.
 */
void BaseEngine::start()
{
	qDebug() << "BaseEngine::start()" << m_serverhost << m_loginport << m_checked_presence << m_checked_cinfo;

	// (In case the TCP sockets were attempting to connect ...) aborts them first
	m_sbsocket->abort();

        connectSocket();
}

/*! \brief Closes the connection to the server
 * This method disconnect the engine from the server
 */
void BaseEngine::stop()
{
	qDebug() << "BaseEngine::stop()";

	m_sbsocket->disconnectFromHost();

	stopKeepAliveTimer();
	stopTryAgainTimer();
	setState(ENotLogged);
	m_sessionid = "";

        m_users.clear();
}

/*! \brief initiate connection to the server
 */
void BaseEngine::connectSocket()
{
        qDebug() << "BaseEngine::connectSocket()" << m_serverhost << m_sbport;
        m_sbsocket->connectToHost(m_serverhost, m_sbport);
}

bool BaseEngine::lastconnwins() const
{
        return m_checked_lastconnwins;
}

void BaseEngine::setLastConnWins(bool b)
{
        m_checked_lastconnwins = b;
}

const QStringList & BaseEngine::getCapabilities() const
{
        return m_capabilities;
}

const QString & BaseEngine::getCapaDisplay() const
{
        return m_capadisplay;
}

const QString & BaseEngine::getCapaApplication() const
{
        return m_capaappli;
}

const QStringList & BaseEngine::getCapaFeatures() const
{
        return m_capafeatures;
}

/*!
 * sets the availability state and call keepLoginAlive() if needed
 *
 * \sa setAvailable()
 * \sa setAway()
 * \sa setBeRightBack()
 * \sa setOutToLunch()
 * \sa setDoNotDisturb()
 */
void BaseEngine::setAvailState(const QString & newstate, bool comesFromServer)
{
	if(m_availstate != newstate) {
		m_availstate = newstate;
		m_settings->setValue("engine/availstate", m_availstate);
                if (comesFromServer)
                        changesAvailChecks();
                keepLoginAlive();
                sendCommand("availstate " + m_availstate);
	}
}

const QString & BaseEngine::getAvailState() const
{
        return m_availstate;
}

void BaseEngine::setAvailable()
{
	setAvailState("available", false);
}

void BaseEngine::setAway()
{
	setAvailState("away", false);
}

void BaseEngine::setBeRightBack()
{
	setAvailState("berightback", false);
}

void BaseEngine::setOutToLunch()
{
	setAvailState("outtolunch", false);
}

void BaseEngine::setDoNotDisturb()
{
	setAvailState("donotdisturb", false);
}

bool BaseEngine::hasFunction(const QString & func)
{
        return m_capabilities.contains("func-" + func);
}

/*! \brief send a command to the server 
 * The m_pendingcommand is sent on the socket.
 *
 * \sa m_pendingcommand
 */
void BaseEngine::sendTCPCommand()
{
        // qDebug() << "BaseEngine::sendTCPCommand()" << m_pendingcommand;
	m_sbsocket->write((m_pendingcommand + "\r\n"/*"\n"*/).toAscii());
}

void BaseEngine::sendCommand(const QString & command)
{
        if(m_sbsocket->state() == QAbstractSocket::ConnectedState) {
                m_pendingcommand = command;
                sendTCPCommand();
        }
}

/*! \brief parse history command response
 *
 * parse the history command response from the server and
 * trigger the update of the call history panel.
 *
 * \sa Logwidget
 */
void BaseEngine::processHistory(const QStringList & histlist)
{
	for(int i=0; i + 7 <= histlist.size(); i += 7) {
		// DateTime; CallerID; duration; Status?; peer; IN/OUT
		//qDebug() << histlist[i+0] << histlist[i+1]
		//         << histlist[i+2] << histlist[i+3]
		//         << histlist[i+4] << histlist[i+5];
		QDateTime dt = QDateTime::fromString(histlist[i+0], Qt::ISODate);
		int duration = histlist[i+2].toInt();
		QString peer = histlist[i+4];
                QString direction = histlist[i+5];
                QString techdef = histlist[i+6];
		//qDebug() << dt << callerid << duration << peer << direction;
		updateLogEntry(dt, duration, peer, direction, techdef);
	}
}


void BaseEngine::monitoredPeerChanged(const QString & peerinfo)
{
        setPeerToDisplay(peerinfo);
}


/*! \brief called when the socket is first connected
 */
void BaseEngine::socketConnected()
{
        QString socname = this->sender()->property("socket").toString();
	qDebug() << "BaseEngine::socketConnected()" << socname;
        if(socname == "tcp_nat") {
                stopTryAgainTimer();
                /* do the login/identification ? */
                setMyClientId();
                m_pendingcommand = "login userid=" + m_userid + ";company=" + m_company + ";";

                if(m_loginkind > 0)
                        m_pendingcommand += "loginkind=agent;phonenumber=" + m_phonenumber + ";";
                else
                        m_pendingcommand += "loginkind=user;";
                
                if(m_checked_presence)
                        m_pendingcommand += "state=" + m_availstate + ";";
                else
                        m_pendingcommand += "state=" + __nopresence__ + ";";
                m_pendingcommand += "ident="       + m_clientid                 + ";";
                m_pendingcommand += "passwd="      + m_passwd                   + ";";
                m_pendingcommand += "version="     + __current_client_version__ + ";";
                m_pendingcommand += "xivoversion=" + __xivo_version__           + ";";
                if(m_checked_lastconnwins)
                        m_pendingcommand += "lastconnwins=true";
                else
                        m_pendingcommand += "lastconnwins=false";
                // login <asterisk> <techno> <id>
                sendTCPCommand();
        } else if(socname == "fax") {
                if(m_faxsize > 0) {
                        QString stp = m_faxid + "\n";
                        m_faxsocket->write(stp.toAscii(), stp.size());
                        m_faxsocket->write(* m_faxdata);
                }
                m_faxsocket->close();
                m_faxsize = 0;
                m_faxid = "";
                delete m_faxdata;
        }
}

/*! \brief called when the socket is disconnected from the server
 */
void BaseEngine::socketDisconnected()
{
        QString socname = this->sender()->property("socket").toString();
	qDebug() << "BaseEngine::socketDisconnected()" << socname;
        if(socname == "tcp_nat") {
                setState(ENotLogged); // calls delogged();
                emitTextMessage(tr("Connection lost with XIVO Daemon"));
                startTryAgainTimer();
                //removePeers();
                //connectSocket();
        }
}

/*! \brief cat host found socket signal
 *
 * This slot is connected to the hostFound() signal of the m_sbsocket
 */
void BaseEngine::socketHostFound()
{
        QString socname = this->sender()->property("socket").toString();
        qDebug() << "BaseEngine::socketHostFound()" << socname;
}

/*! \brief catch socket errors
 */
void BaseEngine::socketError(QAbstractSocket::SocketError socketError)
{
	switch(socketError) {
	case QAbstractSocket::ConnectionRefusedError:
		emitTextMessage(tr("Connection refused"));
		if(m_timer != -1) {
			killTimer(m_timer);
		 	m_timer = -1;
                }
		//m_timer = startTimer(2000);
		break;
        case QAbstractSocket::RemoteHostClosedError:
                popupError("connection_closed");
                break;
	case QAbstractSocket::HostNotFoundError:
		emitTextMessage(tr("Host not found"));
		break;
	case QAbstractSocket::UnknownSocketError:
		emitTextMessage(tr("Unknown socket error"));
		break;
	default:
                qDebug() << "BaseEngine::socketError()" << socketError;
		break;
	}
}

/*! \brief receive signals of socket state change
 *
 * useless...
 */
void BaseEngine::socketStateChanged(QAbstractSocket::SocketState socketState)
{
	qDebug() << "BaseEngine::socketStateChanged(" << socketState << ")";
	if(socketState == QAbstractSocket::ConnectedState) {
		if(m_timer != -1)
		{
			killTimer(m_timer);
			m_timer = -1;
		}
		//startTimer(3000);
	}
}


UserInfo * BaseEngine::findUserFromPhone(const QString & astid,
                                         const QString & context,
                                         const QString & tech,
                                         const QString & phonenum)
{
        foreach(UserInfo * uinfo, m_users)
                if(uinfo->hasPhone(astid, context, tech + "/" + phonenum))
                        return uinfo;
        return NULL;
}

UserInfo * BaseEngine::findUserFromAgent(const QString & astid,
                                         const QString & agentnum)
{
        foreach(UserInfo * uinfo, m_users)
                if(uinfo->hasAgent(astid, agentnum))
                        return uinfo;
        return NULL;
}

void BaseEngine::updatePeerAndCallerid(const QStringList & liststatus)
{
	const int nfields0 = 7; // 0th order size (per-phone/line informations)
	const int nfields1 = 6;  // 1st order size (per-channel informations)
	QStringList chanIds;
	QStringList chanStates;
	QStringList chanOthers;
	QStringList chanPeers;

	// liststatus[0] is a dummy field, only used for debug on the daemon side
	// p/(asteriskid)/(context)/(protocol)/(phoneid)/(phonenum)
        // ("ful", "xivo", "default", "SIP", "147", "Timeout", "0")

	if(liststatus.count() < nfields0) { // not valid
		qDebug() << "BaseEngine::updatePeerAndCallerid() : Bad data from the server (not enough arguments) :" << liststatus;
		return;
	}

	//<who>:<asterisk_id>:<tech(SIP/IAX/...)>:<phoneid>:<numero>:<contexte>:<dispo>:<etat SIP/XML>:<etat VM>:<etat Queues>: <nombre de liaisons>:
        QString astid    = liststatus[1];
	QString context  = liststatus[2];
        QString tech     = liststatus[3];
	QString phoneid  = liststatus[4];
	QString hintstatus = liststatus[5];

        UserInfo * ui = findUserFromPhone(astid, context, tech, phoneid);
        if(ui) {
                qDebug() << liststatus;
                ui->updatePhoneStatus(tech + "/" + phoneid, hintstatus);
        }

	int nchans = liststatus[nfields0 - 1].toInt();
	if(liststatus.size() == nfields0 + nfields1 * nchans) {
		for(int i = 0; i < nchans; i++) {
			//  <channel>:<etat du channel>:<nb de secondes dans cet etat>:<to/from>:<channel en liaison>:<numero en liaison>
			int refn = nfields0 + nfields1 * i;
			QString displayedNum;
			
			hintstatus = liststatus[refn + 1];
			chanIds << ("c/" + astid + "/" + context + "/" + liststatus[refn]);
			chanStates << liststatus[refn + 1];
			chanPeers << liststatus[refn + 4];
                        QString peerid = liststatus[refn + 5];

			if((peerid == "") ||
			   (peerid == "<Unknown>") ||
			   (peerid == "<unknown>") ||
			   (peerid == "anonymous") ||
			   (peerid == "(null)"))
				displayedNum = tr("Unknown Number");
			else
				displayedNum = peerid;

			chanOthers << displayedNum;
                        if(ui)
                                updateCall("c/" + astid + "/" + context + "/" + liststatus[refn],
                                           liststatus[refn + 1],
                                           liststatus[refn + 2].toInt(), liststatus[refn + 3],
                                           liststatus[refn + 4], displayedNum,
                                           ui->userid());
		}
	}

        if(ui) {
                // updateAgentPresence(ui->agentid(), InstMessAvail);
                updatePeer(ui, hintstatus,
                           chanIds, chanStates, chanOthers, chanPeers);
        }

        // used mainly for transfer on directory numbers
        if( (m_asterisk == astid) && (m_userid == liststatus[3]) && (m_dialcontext == context))
                updateMyCalls(chanIds, chanStates, chanOthers);
}


void BaseEngine::removePeerAndCallerid(const QStringList & liststatus)
{
	const int nfields0 = 10; // 0th order size (per-phone/line informations)
	if(liststatus.count() < nfields0) { // not valid
		qDebug() << "BaseEngine::removePeerAndCallerid() : Bad data from the server :" << liststatus;
		return;
	}

        QString astid   = liststatus[1];
	QString context = liststatus[5];
	QString phonenum = liststatus[4];
        QString tech = "";
        UserInfo * ui = findUserFromPhone(astid, context, tech, phonenum);
        if(ui) {
                removePeer(ui->userid());
                m_users.remove(ui->userid());
        }
}


void BaseEngine::popupDestroyed(QObject * obj)
{
	qDebug() << "BaseEngine::popupDestroyed()" << obj;
	//obj->dumpObjectTree();
}

void BaseEngine::addToDataBase(const QString & dbdetails)
{
        qDebug() << "BaseEngine::addToDataBase()" << dbdetails;
        if (dbdetails.size() > 0)
                sendCommand("database " + dbdetails);
}

void BaseEngine::profileToBeShown(Popup * popup)
{
        qDebug() << "BaseEngine::profileToBeShown()";
	newProfile( popup );
}

void BaseEngine::DisplayFiche(const QString & fichecontent, bool qtui)
{
        QBuffer * inputstream = new QBuffer(this);
        inputstream->open(QIODevice::ReadWrite);
        inputstream->write(fichecontent.toUtf8());
        inputstream->close();
        // Get Data and Popup the profile if ok
        Popup * popup = new Popup(inputstream, qtui, m_checked_autourl, m_users[m_company + "/" + m_userid]);
        connect( popup, SIGNAL(destroyed(QObject *)),
                 this, SLOT(popupDestroyed(QObject *)) );
        connect( popup, SIGNAL(save(const QString &)),
                 this, SLOT(addToDataBase(const QString &)) );
        connect( popup, SIGNAL(wantsToBeShown(Popup *)),
                 this, SLOT(profileToBeShown(Popup *)) );
}



bool BaseEngine::parseCommand(const QStringList & listitems)
{
        // qDebug() << "BaseEngine::parseCommand listitems[0].toLower() =" << listitems[0].toLower() << listitems.size();
        if((listitems[0].toLower() == QString("phones-list")) && (listitems.size() == 2)) {
                QStringList listpeers = listitems[1].split(";");
                for(int i = 1 ; i < listpeers.size() - 1; i++) {
                        QStringList liststatus = listpeers[i].split(":");
                        updatePeerAndCallerid(liststatus);
                }

                callsUpdated();
                peersReceived();

        } else if((listitems[0].toLower() == QString("phones-update")) && (listitems.size() == 2)) {
                QStringList liststatus = listitems[1].split(":");
                // qDebug() << liststatus;
                updatePeerAndCallerid(liststatus);
                callsUpdated();

        } else if((listitems[0].toLower() == QString("phones-add")) && (listitems.size() == 2)) {
                QStringList listpeers = listitems[1].split(";");
                for(int i = 1 ; i < listpeers.size() - 1; i++) {
                        QStringList liststatus = listpeers[i].split(":");
                        updatePeerAndCallerid(liststatus);
                }

                if(listpeers[0] == "0") {
                        qDebug() << "phones-add completed";
                } else {
                        sendCommand("phones-add " + listpeers[0]);
                }

        } else if((listitems[0] == QString("users-list")) && (listitems.size() == 2)) {
                QStringList listpeers = listitems[1].split(";");
                for(int i = 1 ; i < listpeers.size() ; i += 9) {
                        qDebug() << listpeers[i] << listpeers[i+1] << listpeers[i+2]
                                 << listpeers[i+3] << listpeers[i+4] << listpeers[i+5]
                                 << listpeers[i+6] << listpeers[i+7] << listpeers[i+8];
                        QString iduser = listpeers[i+1] + "/" + listpeers[i];
                        m_users[iduser] = new UserInfo(iduser);
                        m_users[iduser]->setFullName(listpeers[i+2]);
                        m_users[iduser]->setNumber(listpeers[i+6]);
                        m_users[iduser]->setPhones(listpeers[i+4], listpeers[i+5], listpeers[i+7]);
                        m_users[iduser]->setAgent(listpeers[i+8]);

                        newUser(m_users[iduser]);
                }

                QString fullid_mine = m_company + "/" + m_userid;
                QString fullname_mine = "No One";
                if(m_users.contains(fullid_mine)) {
                        fullname_mine = m_users[fullid_mine]->fullname();
                        localUserInfoDefined(m_users[fullid_mine]);
                }

                setPeerToDisplay(fullid_mine);

                // Who do we monitor ?
                // First look at the last monitored one
                QString fullid_watched = m_settings->value("monitor/peer").toString();
                QString fullname_watched = "";
                // If there was nobody, let's watch ourselves.
                if(fullid_watched.isEmpty()) {
                        fullid_watched = fullid_mine;
                        fullname_watched = fullname_mine;
                } else {
                        if(m_users.contains(fullid_watched))
                                fullname_watched = m_users[fullid_watched]->fullname();
                        // If the CallerId value is empty, fallback to ourselves.
                        if(fullname_watched.isEmpty()) {
                                fullid_watched = fullid_mine;
                                fullname_watched = fullname_mine;
                        }
                }
                
                monitorPeer(fullid_watched, fullname_watched);
                emitTextMessage(tr("Received status for %1 users").arg(m_users.size()));

//         } else if((listitems[0] == QString("users-change")) && (listitems.size() == 2)) {
                
        } else if((listitems[0].toLower() == QString("phones-del")) && (listitems.size() == 2)) {
                QStringList listpeers = listitems[1].split(";");
                for(int i = 1 ; i < listpeers.size() - 1; i++) {
                        QStringList liststatus = listpeers[i].split(":");
                        removePeerAndCallerid(liststatus);
                }

                if(listpeers[0] == "0") {
                        qDebug() << "phones-del completed";
                } else {
                        sendCommand("phones-del " + listpeers[0]);
                }

        } else if((listitems[0].toLower() == QString("phones-signal-deloradd")) && (listitems.size() == 2)) {
                QStringList listpeers = listitems[1].split(";");
                qDebug() << listpeers;
//                 emitTextMessage(tr("New phone list on %1 : - %2 + %3 = %4 total").arg(listpeers[0],
//                                                                                       listpeers[1],
//                                                                                       listpeers[2],
//                                                                                       listpeers[3]));
                if(listpeers[1].toInt() > 0)
                        sendCommand("phones-del");
                if(listpeers[2].toInt() > 0)
                        sendCommand("phones-add");

        } else if(listitems[0].toLower() == "history") {
                processHistory(listitems[1].split(";"));
        } else if(listitems[0].toLower() == "directory-response") {
                directoryResponse(listitems[1]);
        } else if(listitems[0].toLower() == QString("message")) {
                QTime currentTime = QTime::currentTime();
                QStringList message = listitems[1].split("::");
                // message[0] : emitter name
                if(message.size() == 2)
                        emitTextMessage(message[0] + tr(" said : ") + message[1]);
                else
                        emitTextMessage(tr("Unknown") + tr(" said : ") + listitems[1]);
        } else if((listitems[0].toLower() == QString("featuresupdate")) && (listitems.size() == 2)) {
                QStringList featuresupdate_list = listitems[1].split(";");
                qDebug() << featuresupdate_list;
                if(featuresupdate_list.size() == 5)
                        if((m_monitored_asterisk == featuresupdate_list[0]) &&
                           (m_monitored_context  == featuresupdate_list[1]) &&
                           (m_monitored_userid   == featuresupdate_list[2]))
                                initFeatureFields(featuresupdate_list[3], featuresupdate_list[4]);
        } else if((listitems[0].toLower() == QString("featuresget")) && (listitems.size() == 2)) {
                if(listitems[1] != "KO") {
                        QStringList features_list = listitems[1].split(";");
                        resetFeatures();
                        if(features_list.size() > 1)
                                for(int i=0; i<features_list.size()-1; i+=2)
                                        initFeatureFields(features_list[i], features_list[i+1]);
                        emitTextMessage(tr("Received Services Data for ") + m_monitored_asterisk + "/" + m_monitored_userid);
                } else
                        emitTextMessage(tr("Could not retrieve the Services data.") + " " + tr("Maybe Asterisk is down."));

        } else if((listitems[0].toLower() == QString("parkedcall")) && (listitems.size() == 2)) {
                parkingEvent(listitems[0], listitems[1]);
        } else if(listitems[0].toLower() == QString("unparkedcall")) {
                parkingEvent(listitems[0], listitems[1]);
        } else if(listitems[0].toLower() == QString("parkedcalltimeout")) {
                parkingEvent(listitems[0], listitems[1]);
        } else if(listitems[0].toLower() == QString("parkedcallgiveup")) {
                parkingEvent(listitems[0], listitems[1]);

        } else if(listitems[0].toLower() == QString("queues-list")) {
                newQueueList(listitems[1]);

        } else if(listitems[0].toLower() == QString("agents-list")) {
                qDebug() << listitems;
                newAgentList(listitems[1]);

        } else if(listitems[0].toLower() == QString("agent-status")) {
                qDebug() << listitems;
                QStringList liststatus = listitems[1].split(";");
                // if status = "for me" => update me
                changeWatchedAgentSignal(liststatus);

        } else if(listitems[0].toLower() == QString("queue-status")) {
                qDebug() << listitems;
                QStringList liststatus = listitems[1].split(";");
                changeWatchedQueueSignal(liststatus);

        } else if(listitems[0].toLower() == QString("update-agents")) {
                QStringList liststatus = listitems[1].split(":");
                if(liststatus.size() > 1) {
                        QStringList newstatuses = liststatus[1].split("/");
                        qDebug() << newstatuses;
                        QString astid = newstatuses[1];
                        QString agentnum = newstatuses[2];
                        UserInfo * ui = findUserFromAgent(astid, agentnum);
                        if(ui)
                                updatePeerAgent(ui->userid(), liststatus[1]);
                        else // (useful ?) in order to transfer the replies to unmatched agents
                                updatePeerAgent("", liststatus[1]);
                }

        } else if(listitems[0].toLower() == QString("update-queues")) {
                setQueueStatus(listitems[1]);

        } else if(listitems[0].toLower() == QString("featuresput")) {
                QString ret = listitems[1].split(";")[0];
                if(ret == "OK") {
                        featurePutIsOK();
                        emitTextMessage("");
                } else {
                        featurePutIsKO();
                        emitTextMessage(tr("Could not modify the Services data.") + " " + tr("Maybe Asterisk is down."));
                }
        } else if(listitems[0].toLower() == QString("faxsend")) {
                quint16 port_fax = listitems[1].toInt();
                m_faxsocket->connectToHost(m_serverhost, port_fax);
        } else if(listitems[0].toLower() == QString("faxsent")) {
                ackFax(listitems[1]);
        } else if((listitems[0] != "") && (listitems[0] != "______") && (listitems[0] != "phones-noupdate"))
                qDebug() << "unknown command" << listitems[0];

        return true;
}

void BaseEngine::agentAction(const QString & action)
{
        sendCommand("agent " + action);
}

void BaseEngine::sendFaxCommand(const QString & filename, const QString & number,
                                Qt::CheckState hide)
{
        QFile * qf = new QFile(filename);
        qf->open(QIODevice::ReadOnly);
        m_faxdata = new QByteArray();
        m_faxdata->append(qf->readAll());
        qf->close();
        m_faxsize = m_faxdata->size();
        if(m_faxdata->size() > 0) {
                m_faxid = "size=" + QString::number(m_faxsize) +
                        " number=" + number +
                        " hide=" + QString::number(hide) +
                        " astid=" + m_asterisk +
                        " context=" + m_dialcontext;
                sendCommand("faxsend " + m_faxid);
        } else
                ackFax("ko;file");
}

void BaseEngine::popupError(const QString & errorid)
{
        QString errormsg = QString(tr("Server has sent an Error."));
        if(errorid.toLower() == "asterisk_name")
                errormsg = tr("The XIVO Id <%1> is unknown by the XIVO CTI Server.").arg(m_asterisk);

        else if(errorid.toLower() == "connection_refused")
                errormsg = tr("You are not allowed to connect to the Server.");

        else if(errorid.toLower() == "number_of_arguments")
                errormsg = tr("The number of arguments sent is incorrect.\n"
                              "Maybe a version issue ?");

        else if(errorid.toLower() == "user_not_found")
                errormsg = tr("Your registration name <%1> is not known on XIVO Id <%2>.").arg(m_userid, m_asterisk);

        else if(errorid.toLower() == "session_expired")
                errormsg = tr("Your session has expired.");

        else if(errorid.toLower() == "login_passwd")
                errormsg = tr("You entered a wrong login / password.");

        else if(errorid.toLower() == "no_keepalive_from_server")
                errormsg = tr("The server did not reply to the last keepalive.");

        else if(errorid.toLower() == "connection_closed")
                errormsg = tr("The server has just closed the connection.");

        else if(errorid.toLower() == "server_stopped")
                errormsg = tr("The server has just been stopped.");

        else if(errorid.toLower() == "server_reloaded")
                errormsg = tr("The server has just been reloaded.");

        else if(errorid.toLower() == "already_connected")
                errormsg = tr("You are already connected.");

        else if(errorid.toLower() == "uninit_phone")
                errormsg = tr("Your phone <%1> has not been provisioned on XIVO Id <%2>.").arg(m_userid, m_asterisk);

        else if(errorid.toLower() == "no_capability")
                errormsg = tr("No capability allowed.");

        else if(errorid.startsWith("xcusers:")) {
                QStringList userslist = errorid.split(":")[1].split(";");
                errormsg = tr("Max number (%1) of XIVO Clients already reached.").arg(userslist[0]);
        }
        else if(errorid.startsWith("sbusers:")) {
                QStringList userslist = errorid.split(":")[1].split(";");
                errormsg = tr("Max number (%1) of XIVO Switchboards already reached.").arg(userslist[0]);
        }
        else if(errorid.startsWith("missing:")) {
                errormsg = tr("Missing Argument(s)");
        }
        else if(errorid.startsWith("version_client:")) {
                QStringList versionslist = errorid.split(":")[1].split(";");
                if(versionslist.size() >= 2)
                        errormsg = tr("Your client version (%1) is too old for this server.\n"
                                      "Please upgrade it to %2 at least.").arg(__current_client_version__,
                                                                               versionslist[1]);
                else
                        errormsg = tr("Your client version (%1) is too old for this server.\n"
                                      "Please upgrade it.").arg(__current_client_version__);
        }
        else if(errorid.startsWith("version_server:")) {
                QStringList versionslist = errorid.split(":")[1].split(";");
                if(versionslist.size() >= 2)
                        errormsg = tr("Your server version (%1) is too old for this client.\n"
                                      "Please upgrade it to %2 at least.").arg(versionslist[0], __current_client_version__);
                else
                        errormsg = tr("Your server version (%1) is too old for this client.\n"
                                      "Please upgrade it.").arg(versionslist[0]);
        }

        // logs a message before sending any popup that would block
        emitTextMessage(tr("Error") + " : " + errormsg);
        if(! m_trytoreconnect)
                QMessageBox::critical(NULL, tr("XIVO CTI Error"), errormsg);
}


/*! \brief called when data are ready to be read on the socket.
 *
 * Read and process the data from the server.
 */
void BaseEngine::socketReadyRead()
{
        //qDebug() << "BaseEngine::socketReadyRead()";
	//QByteArray data = m_sbsocket->readAll();

	while(m_sbsocket->canReadLine()) {
		QByteArray data  = m_sbsocket->readLine();
                // qDebug() << "BaseEngine::socketReadyRead() data.size() = " << data.size();
		QString line     = QString::fromUtf8(data);
		QStringList list = line.trimmed().split("=");

		if(line.startsWith("<?xml") || line.startsWith("<ui version=")) {
                        // we get here when receiving a customer info in tcp mode
                        qDebug() << "BaseEngine::socketReadyRead() (Customer Info)" << line.size();
                        bool qtui = false;
                        if(line.startsWith("<ui version="))
                                qtui = true;
                        DisplayFiche(line, qtui);

                } else if (list.size() == 2) {
                        if(list[0].toLower() == "loginok") {
				QStringList params = list[1].split(";");
                                m_version_server = -1;
                                QHash<QString, QString> params_list;
                                for(int i = 0 ; i < params.size(); i++) {
                                        QStringList params_couple = params[i].split(":");
                                        if(params_couple.size() == 2)
                                                params_list[params_couple[0]] = params_couple[1];
                                }
                                m_dialcontext    = params_list["context"];
                                m_extension      = params_list["phonenum"];
                                m_capabilities   = params_list["capas"].split(",");
                                m_capadisplay    = params_list["capadisp"].replace("-", ":");
                                m_capaappli      = params_list["capaappli"];
                                m_version_server = params_list["version"].toInt();
                                m_xivover_server = params_list["xivoversion"];
                                m_forced_state   = params_list["state"];
                                m_capafeatures   = params_list["capas_features"].split(",");
                                
                                qDebug() << m_capadisplay << m_capaappli;
                                
                                if(m_version_server < REQUIRED_SERVER_VERSION) {
                                        stop();
                                        popupError("version_server:" + QString::number(m_version_server) + ";" + QString::number(REQUIRED_SERVER_VERSION));
                                } else if((m_capabilities.size() == 1) && (m_capabilities[0].size() == 0)) {
                                        stop();
                                        popupError("no_capability");
                                } else {
                                        setState(ELogged); // calls logged()
                                        setAvailState(m_forced_state, true);
                                        m_ka_timerid = startTimer(m_keepaliveinterval);
                                        askCallerIds();
                                }
			} else if(list[0].toLower() == "loginko") {
                                stop();
                                popupError(list[1]);
			} else
                                parseCommand(list);
                }
	}
}

/*! \brief transfers to the typed number
 */
void BaseEngine::transferToNumber(const QString & chan)
{
        if(m_numbertodial.size() > 0) {
                qDebug() << "BaseEngine::transferToNumber()" << chan << m_numbertodial;
                transferCall(chan, m_numbertodial);
        }
}

/*! \brief send an originate command to the server
 */
void BaseEngine::textEdited(const QString & text)
{
        m_numbertodial = text;
}

void BaseEngine::copyNumber(const QString & dst)
{
        pasteToDialPanel(dst);
}

/*! \brief send an originate command to the server
 */
void BaseEngine::originateCall(const QString & src, const QString & dst)
{
	qDebug() << "BaseEngine::originateCall()" << src << dst;
        sendCommand("originate " + src + " " + dst);
}

/*! \brief send a transfer call command to the server
 */
void BaseEngine::transferCall(const QString & src, const QString & dst)
{
	qDebug() << "BaseEngine::transferCall()" << src << dst;
	QStringList dstlist = dst.split("/");
	if(dstlist.size() >= 6)
                sendCommand("transfer " + src + " "
                            + dstlist[0] + "/" + dstlist[1] + "/" + m_dialcontext + "/"
                            + dstlist[3] + "/" + dstlist[4] + "/" + dstlist[5]);
	else
                sendCommand("transfer " + src + " p/" + m_asterisk + "/"
                            + m_dialcontext + "/" + "/" + "/" + dst);
}

/*! \brief send an attended transfer call command to the server
 */
void BaseEngine::atxferCall(const QString & src, const QString & dst)
{
	qDebug() << "BaseEngine::atxferCall()" << src << dst;
	QStringList dstlist = dst.split("/");
	if(dstlist.size() >= 6)
                sendCommand("atxfer " + src + " "
                            + dstlist[0] + "/" + dstlist[1] + "/" + m_dialcontext + "/"
                            + dstlist[3] + "/" + dstlist[4] + "/" + dstlist[5]);
	else
                sendCommand("atxfer " + src + " p/" + m_asterisk + "/"
                            + m_dialcontext + "/" + "/" + "/" + dst);
}

/*! \brief send a transfer call command to the server
 */
void BaseEngine::parkCall(const QString & src)
{
	qDebug() << "BaseEngine::parkCall()" << src;
        QStringList srclist = src.split("/");
        sendCommand("transfer " + src + " p/" + srclist[1] + "/"
                    + srclist[2] + "/" + "/" + "/special:parkthecall");
}

/*! \brief intercept a call (a channel)
 *
 * The channel is transfered to "Me"
 *
 * \sa transferCall
 */
void BaseEngine::interceptCall(const QString & src)
{
	qDebug() << "BaseEngine::interceptCall()" << src;
        sendCommand("transfer " + src + " p/"
                    + m_asterisk + "/" + m_dialcontext + "/"
                    + m_protocol + "/" + "/" + m_extension);
}

/*! \brief hang up a channel
 *
 * send a hang up command to the server
 */
void BaseEngine::hangUp(const QString & channel)
{
	qDebug() << "BaseEngine::hangUp()" << channel;
	sendCommand("hangup " + channel);
}

/*! \brief hang up a channel
 *
 * send a hang up command to the server
 */
void BaseEngine::simpleHangUp(const UserInfo * ui, const QString & channel)
{
	qDebug() << "BaseEngine::simpleHangUp()" << ui << channel;
	sendCommand("simplehangup " + channel);
}

/*! \brief pick up a channel
 *
 * send a pick up command to the server
 */
void BaseEngine::pickUp(const UserInfo * ui, const QString & channel)
{
	qDebug() << "BaseEngine::pickUp()" << ui << channel;
	sendCommand("pickup " + channel);
}

/*! \brief send the directory search command to the server
 *
 * \sa directoryResponse()
 */
void BaseEngine::searchDirectory(const QString & text)
{
	qDebug() << "BaseEngine::searchDirectory()" << text;
        sendCommand("directory-search " + text);
}

/*! \brief ask history for an extension 
 */
void BaseEngine::requestHistory(const QString & peer, int mode)
{
	/* mode = 0 : Out calls
	 * mode = 1 : In calls
	 * mode = 2 : Missed calls */
	if(mode >= 0) {
                // qDebug() << "BaseEngine::requestHistory()" << peer;
                sendCommand("history " + peer + " " + QString::number(m_historysize) + " " + QString::number(mode));
        }
}

// === Getter and Setters ===
/* \brief set server address
 *
 * Set server host name and server port
 */
void BaseEngine::setAddress(const QString & host, quint16 port)
{
        qDebug() << "BaseEngine::setAddress()" << port;
	m_serverhost = host;
	m_sbport = port;
}

/*! \brief get server IP address */
const QString & BaseEngine::serverip() const
{
	return m_serverhost;
}

/*! \brief get server port */
const quint16 BaseEngine::sbPort() const
{
	return m_sbport;
}

const QString & BaseEngine::company() const
{
        return m_company;
}

void BaseEngine::setServerip(const QString & serverip)
{
	m_serverhost = serverip;
}

void BaseEngine::setCompany(const QString & companyname)
{
	m_company = companyname;
}

const quint16 BaseEngine::loginPort() const
{
	return m_loginport;
}

void BaseEngine::setLoginPort(const quint16 & port)
{
	m_loginport = port;
}

const QString & BaseEngine::userId() const
{
	return m_userid;
}

void BaseEngine::setUserId(const QString & userid)
{
	m_userid = userid;
}

const QString & BaseEngine::phonenumber() const
{
	return m_phonenumber;
}

void BaseEngine::setPhonenumber(const QString & phonenumber)
{
	m_phonenumber = phonenumber;
}

const int BaseEngine::loginkind() const
{
	return m_loginkind;
}

void BaseEngine::setLoginKind(const int loginkind)
{
	m_loginkind = loginkind;
}

const int BaseEngine::keeppass() const
{
	return m_keeppass;
}

void BaseEngine::setKeepPass(const int keeppass)
{
	m_keeppass = keeppass;
}

const QString & BaseEngine::password() const
{
	return m_passwd;
}

void BaseEngine::setPassword(const QString & passwd)
{
	m_passwd = passwd;
}

const QString & BaseEngine::phoneNum() const
{
	return m_extension;
}

const QString & BaseEngine::dialContext() const
{
	return m_dialcontext;
}

const QString & BaseEngine::fullName() const
{
	return m_fullname;
}

void BaseEngine::setDialContext(const QString & dialcontext)
{
	m_dialcontext = dialcontext;
}

void BaseEngine::setTrytoreconnect(bool b)
{
	m_trytoreconnect = b;
}

bool BaseEngine::trytoreconnect() const
{
	return m_trytoreconnect;
}

void BaseEngine::initFeatureFields(const QString & field, const QString & value)
{
        //        qDebug() << field << value;
        bool isenabled = (value.split(":")[0] == "1");
	if((field == "enablevoicemail") || (field == "vm"))
		voiceMailChanged(isenabled);
	else if((field == "enablednd") || (field == "dnd"))
		dndChanged(isenabled);
	else if(field == "callfilter")
		callFilteringChanged(isenabled);
	else if(field == "callrecord")
		callRecordingChanged(isenabled);
	else if(field == "unc")
		uncondForwardUpdated(isenabled, value.split(":")[1]);
	else if(field == "busy")
		forwardOnBusyUpdated(isenabled, value.split(":")[1]);
	else if(field == "rna")
		forwardOnUnavailableUpdated(isenabled, value.split(":")[1]);
}

void BaseEngine::stopKeepAliveTimer()
{
	if( m_ka_timerid > 0 )
	{
		killTimer(m_ka_timerid);
		m_ka_timerid = 0;
	}
}

void BaseEngine::stopTryAgainTimer()
{
	if( m_try_timerid > 0 )
	{
		killTimer(m_try_timerid);
		m_try_timerid = 0;
	}
}

void BaseEngine::startTryAgainTimer()
{
	if( m_try_timerid == 0 && m_trytoreconnect )
	{
		m_try_timerid = startTimer(m_trytoreconnectinterval);
	}
}

void BaseEngine::setHistorySize(uint size)
{
	m_historysize = size;
}

uint BaseEngine::historySize() const
{
	return m_historysize;
}

void BaseEngine::setContactsSize(uint size)
{
	m_contactssize = size;
}

uint BaseEngine::contactsSize() const
{
	return m_contactssize;
}

void BaseEngine::setContactsColumns(uint columns)
{
	m_contactscolumns = columns;
}

uint BaseEngine::contactsColumns() const
{
	return m_contactscolumns;
}

uint BaseEngine::trytoreconnectinterval() const
{
	return m_trytoreconnectinterval;
}

/*!
 * Setter for property m_trytoreconnectinterval
 * Restart timer if the value changed.
 *
 * \sa trytoreconnectinterval
 */
void BaseEngine::setTrytoreconnectinterval(uint i)
{
	if( m_trytoreconnectinterval != i )
	{
		m_trytoreconnectinterval = i;
		if(m_try_timerid > 0)
		{
			killTimer(m_try_timerid);
			m_try_timerid = startTimer(m_trytoreconnectinterval);
		}
	}
}

/*! \brief implement timer event
 *
 * does nothing
 */
void BaseEngine::timerEvent(QTimerEvent * event)
{
	int timerId = event->timerId();
        //qDebug() << "BaseEngine::timerEvent() timerId=" << timerId << m_ka_timerid << m_try_timerid;
	if(timerId == m_ka_timerid) {
                keepLoginAlive();
                event->accept();
        } else if(timerId == m_try_timerid) {
                emitTextMessage(tr("Attempting to reconnect to server"));
		start();
		event->accept();
	} else {
		event->ignore();
	}
}

void BaseEngine::deleteRemovables()
{
	m_removable.clear();
}

void BaseEngine::addRemovable(const QMetaObject * metaobject)
{
	m_removable.append(metaobject);
}

bool BaseEngine::isRemovable(const QMetaObject * metaobject)
{
	for(int i = 0; i < m_removable.count() ; i++)
		if (metaobject == m_removable[i])
			return true;
	return false;
}

void BaseEngine::featurePutVoiceMail(bool b)
{
        sendCommand("featuresput " + m_monitored_asterisk + " " + m_monitored_context + " " + m_monitored_userid + " enablevoicemail " + QString(b ? "1" : "0"));
}

void BaseEngine::featurePutCallRecording(bool b)
{
	sendCommand("featuresput " + m_monitored_asterisk + " " + m_monitored_context + " " + m_monitored_userid + " callrecord " + QString(b ? "1" : "0"));
}

void BaseEngine::featurePutCallFiltering(bool b)
{
	sendCommand("featuresput " + m_monitored_asterisk + " " + m_monitored_context + " " + m_monitored_userid + " callfilter " + QString(b ? "1" : "0"));
}

void BaseEngine::featurePutDnd(bool b)
{
	sendCommand("featuresput " + m_monitored_asterisk + " " + m_monitored_context + " " + m_monitored_userid + " enablednd " + QString(b ? "1" : "0"));
}

void BaseEngine::featurePutForwardOnUnavailable(bool b, const QString & dst)
{
	sendCommand("featuresput " + m_monitored_asterisk + " " + m_monitored_context + " " + m_monitored_userid + " enablerna " + QString(b ? "1" : "0"));
	sendCommand("featuresput " + m_monitored_asterisk + " " + m_monitored_context + " " + m_monitored_userid + " destrna " + dst);
}

void BaseEngine::featurePutForwardOnBusy(bool b, const QString & dst)
{
	sendCommand("featuresput " + m_monitored_asterisk + " " + m_monitored_context + " " + m_monitored_userid + " enablebusy " + QString(b ? "1" : "0"));
	sendCommand("featuresput " + m_monitored_asterisk + " " + m_monitored_context + " " + m_monitored_userid + " destbusy " + dst);
}

void BaseEngine::featurePutUncondForward(bool b, const QString & dst)
{
	sendCommand("featuresput " + m_monitored_asterisk + " " + m_monitored_context + " " + m_monitored_userid + " enableunc " + QString(b ? "1" : "0"));
	sendCommand("featuresput " + m_monitored_asterisk + " " + m_monitored_context + " " + m_monitored_userid + " destunc " + dst);
}

void BaseEngine::askFeatures(const QString & peer)
{
        qDebug() << "BaseEngine::askFeatures()" << peer << m_asterisk << m_dialcontext << m_userid;
        m_monitored_asterisk = m_asterisk;
        m_monitored_context  = m_dialcontext;
        m_monitored_userid   = m_userid;
        QStringList peerp = peer.split("/");
        if(peerp.size() == 6) {
                m_monitored_asterisk = peerp[1];
                m_monitored_context  = peerp[2];
                m_monitored_userid   = peerp[5];
        }
        sendCommand("featuresget " + m_monitored_asterisk + " " + m_monitored_context + " " + m_monitored_userid);
}

void BaseEngine::askCallerIds()
{
        qDebug() << "BaseEngine::askCallerIds()";
        sendCommand("users-list");
        sendCommand("agents-list");
	sendCommand("phones-list");
        sendCommand("queues-list");
        sendCommand("agents-status");
}

void BaseEngine::setSystrayed(bool b)
{
	m_systrayed = b;
}

bool BaseEngine::systrayed() const
{
	return m_systrayed;
}

void BaseEngine::setAutoconnect(bool b)
{
	m_autoconnect = b;
}

bool BaseEngine::autoconnect() const
{
	return m_autoconnect;
}

uint BaseEngine::keepaliveinterval() const
{
	return m_keepaliveinterval;
}

/*!
 * Setter for the m_keepaliveinterval property.
 * if the value is changed, existing timer is restarted.
 *
 * \sa keepaliveinterval
 */
void BaseEngine::setKeepaliveinterval(uint i)
{
	if(i != m_keepaliveinterval)
	{
		m_keepaliveinterval = i;
		if(m_ka_timerid > 0)
		{
			killTimer(m_ka_timerid);
			m_ka_timerid = startTimer(m_keepaliveinterval);
		}
	}
}

const BaseEngine::EngineState BaseEngine::state() const
{
	return m_state;
}

/*!
 * setter for the m_state property.
 * If the state is becoming ELogged, the
 * signal logged() is thrown.
 * If the state is becoming ENotLogged, the
 * signal delogged() is thrown.
 */
void BaseEngine::setState(EngineState state)
{
	if(state != m_state) {
		m_state = state;
		if(state == ELogged) {
                        m_enabled_presence = m_capabilities.contains("func-presence");
			stopTryAgainTimer();
			if(m_checked_presence && m_enabled_presence)
                                availAllowChanged(true);
			logged();
		} else if(state == ENotLogged) {
                        m_enabled_presence = false;
			availAllowChanged(false);
			delogged();
		}
	}
}

void BaseEngine::changeWatchedAgentSlot(const QString & astagentid)
{
        qDebug() << "BaseEngine::changeWatchedAgentSlot()" << astagentid;
        sendCommand("agent-status " + astagentid);
        // changeWatchedAgentSignal(agentid);
}

void BaseEngine::changeWatchedQueueSlot(const QString & astqueueid)
{
        qDebug() << "BaseEngine::changeWatchedQueueSlot()" << astqueueid;
        sendCommand("queue-status " + astqueueid);
        // changeWatchedQueueSignal(queueid);
}

void BaseEngine::setOSInfos(const QString & osname)
{
        m_osname = osname;
}

/*!
 * Builds a string defining who is the client (SB or XC @ osname)
*/
void BaseEngine::setMyClientId()
{
        m_clientid = "undef@" + m_osname;
}

/*!
 * Send a keep alive message to the login server.
 * The message is sent in a datagram through m_udpsocket
 */ 
void BaseEngine::keepLoginAlive()
{
	//qDebug() << "BaseEngine::keepLoginAlive()";
	// got to disconnected state if more than xx keepalive messages
	// have been left without response.
	if(m_pendingkeepalivemsg > 1) {
		qDebug() << "m_pendingkeepalivemsg" << m_pendingkeepalivemsg << "=> 0";
		stopKeepAliveTimer();
		setState(ENotLogged);
		m_pendingkeepalivemsg = 0;
                popupError("no_keepalive_from_server");
		startTryAgainTimer();
		return;
	}

        sendCommand("availstate " + m_availstate);
}

/*!
 * Send a keep alive message to the login server.
 * The message is sent in a datagram through m_udpsocket
 */ 
void BaseEngine::sendMessage(const QString & txt)
{
	sendCommand("message " + txt);
}
