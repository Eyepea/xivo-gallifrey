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

/* $Revision$
 * $Date$
 */

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QHideEvent>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QStatusBar>
#include <QSystemTrayIcon>
#include <QTabWidget>
#include <QTime>
#include <QVBoxLayout>

#include "baseengine.h"
#include "confwidget.h"
#include "dialpanel.h"
#include "directorypanel.h"
#include "identitydisplay.h"
#include "logwidget.h"
#include "mainwidget.h"
#include "popup.h"
#include "searchpanel.h"
#include "servicepanel.h"

const QString extraspace("  ");

/*! \brief Constructor
 *
 * This Constructor creates the 3 buttons in a
 * vertical box layout and connect signals with slots.
 */
//        : QMainWindow(parent, Qt::FramelessWindowHint),
MainWidget::MainWidget(BaseEngine * engine, QWidget * parent)
        : QMainWindow(parent),
          m_engine(engine), m_systrayIcon(0),
          m_icon(":/images/xivoicon.png"), m_icongrey(":/images/xivoicon-grey.png")
{
	QSettings settings;
	QPixmap redsquare(":/images/disconnected.png");
	statusBar();	// This creates the status bar.
	m_status = new QLabel();
	m_status->setPixmap(redsquare);
	statusBar()->addPermanentWidget(m_status);
	statusBar()->clearMessage();
	setWindowIcon(QIcon(":/images/xivoicon.png"));
	setWindowTitle("XIVO Client");
	//setWindowFlags(Qt::Dialog);
	//layout->setSizeConstraint(QLayout::SetFixedSize);	// remove minimize and maximize button

	createActions();
	createMenus();

	if ( QSystemTrayIcon::isSystemTrayAvailable() )
		createSystrayIcon();

	connect( m_engine, SIGNAL(logged()),
	         this, SLOT(engineStarted()));
	connect( m_engine, SIGNAL(delogged()),
                 this, SLOT(engineStopped()));
	connect( m_engine, SIGNAL(newProfile(Popup *)),
	         this, SLOT(showNewProfile(Popup *)) );
        connect( m_engine, SIGNAL(emitTextMessage(const QString &)),
                 statusBar(), SLOT(showMessage(const QString &)));

        // to be better defined
	resize(500, 400);
	restoreGeometry(settings.value("display/mainwingeometry").toByteArray());
	
	m_wid = new QWidget();
	m_mainlayout = new QVBoxLayout(m_wid);
        m_xivobg = new QLabel();
        m_xivobg->setPixmap(QPixmap(":/images/xivo-login.png"));
        m_xivobg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        m_mainlayout->addWidget(m_xivobg, 1, Qt::AlignHCenter | Qt::AlignVCenter);
	setCentralWidget(m_wid);
	m_tablimit = settings.value("display/tablimit", 5).toInt();

        //        m_xivobg2 = new QLabel();
        //        m_xivobg2->setPixmap(QPixmap(":/xivo-client.png"));
        //        m_mainlayout->addWidget(m_xivobg2, 0, Qt::AlignHCenter | Qt::AlignVCenter);
}

MainWidget::~MainWidget()
{
        savePositions();
}

void MainWidget::affTextChanged()
{
	QString txt = m_messagetosend->text();
	txt.replace(" ", "_");
	m_engine->sendMessage(txt.toUtf8());
	m_messagetosend->setText("");
}

void MainWidget::createActions()
{
	m_cfgact = new QAction(tr("Confi&gure"), this);
	m_cfgact->setStatusTip(tr("Configure account and connection options"));
	connect( m_cfgact, SIGNAL(triggered()),
		 this, SLOT(showConfDialog()) );

	m_quitact = new QAction(tr("&Quit"), this);
	m_quitact->setStatusTip(tr("Close the application"));
	connect( m_quitact, SIGNAL(triggered()),
		 m_engine, SLOT(stop()) );
	connect( m_quitact, SIGNAL(triggered()),
		 qApp, SLOT(quit()) );

	m_systrayact = new QAction(tr("To S&ystray"), this);
	m_systrayact->setStatusTip(tr("Go to the system tray"));
	connect( m_systrayact, SIGNAL(triggered()),
		 this, SLOT(hide()) );
	m_systrayact->setEnabled( QSystemTrayIcon::isSystemTrayAvailable() );

	m_connectact = new QAction(tr("&Connect"), this);
	m_connectact->setStatusTip(tr("Connect to the server"));
	connect( m_connectact, SIGNAL(triggered()),
		 m_engine, SLOT(start()) );

	m_disconnectact = new QAction(tr("&Disconnect"), this);
	m_disconnectact->setStatusTip(tr("Disconnect from the server"));
	connect( m_disconnectact, SIGNAL(triggered()),
		 m_engine, SLOT(stop()) );

	m_connectact->setEnabled(true);
	m_disconnectact->setEnabled(false);

	// Availability actions :
	m_availgrp = new QActionGroup( this );
	m_availgrp->setExclusive(true);

	m_avact_avail = new QAction( tr("&Available"), this );
	m_avact_avail->setCheckable(true);
	connect( m_avact_avail, SIGNAL(triggered()),
	         m_engine, SLOT(setAvailable()) );
	m_availgrp->addAction( m_avact_avail );
	m_avact_away = new QAction( tr("A&way"), this );
	m_avact_away->setCheckable(true);
	connect( m_avact_away, SIGNAL(triggered()),
	         m_engine, SLOT(setAway()) );
	m_availgrp->addAction( m_avact_away );
	m_avact_brb = new QAction( tr("&Be Right Back"), this );
	m_avact_brb->setCheckable(true);
	connect( m_avact_brb, SIGNAL(triggered()),
	         m_engine, SLOT(setBeRightBack()) );
	m_availgrp->addAction( m_avact_brb );
	m_avact_otl = new QAction( tr("&Out To Lunch"), this );
	m_avact_otl->setCheckable(true);
	connect( m_avact_otl, SIGNAL(triggered()),
	         m_engine, SLOT(setOutToLunch()) );
	m_availgrp->addAction( m_avact_otl );
	m_avact_dnd = new QAction( tr("&Do not disturb"), this );
	m_avact_dnd->setCheckable(true);
	connect( m_avact_dnd, SIGNAL(triggered()),
	         m_engine, SLOT(setDoNotDisturb()) );
	m_availgrp->addAction( m_avact_dnd );
        
        connect( m_engine, SIGNAL(changesAvailChecks()),
                 this, SLOT(checksAvailState()) );
        
        checksAvailState();
}

void MainWidget::checksAvailState()
{
	if (m_engine->getAvailState() == QString("berightback"))
		m_avact_brb->setChecked( true );
	else if (m_engine->getAvailState() == QString("donotdisturb"))
		m_avact_dnd->setChecked( true );
	else if (m_engine->getAvailState() == QString("away"))
		m_avact_away->setChecked( true );
	else if (m_engine->getAvailState() == QString("outtolunch"))
		m_avact_otl->setChecked( true );
	else
		m_avact_avail->setChecked( true );
}

void MainWidget::createMenus()
{
	QMenu * filemenu = menuBar()->addMenu("&XIVO Client");
	filemenu->addAction( m_cfgact );
	filemenu->addAction( m_systrayact );
	filemenu->addSeparator();
	filemenu->addAction( m_connectact );
	filemenu->addAction( m_disconnectact );
	filemenu->addSeparator();
	filemenu->addAction( m_quitact );

	m_avail = menuBar()->addMenu(tr("&Availability"));
	m_avail->addActions( m_availgrp->actions() );
	m_avail->setEnabled( false );
	connect( m_engine, SIGNAL(availAllowChanged(bool)),
	         m_avail, SLOT(setEnabled(bool)) );

	QMenu * helpmenu = menuBar()->addMenu(tr("&Help"));
	helpmenu->addAction(tr("&About XIVO Client"), this, SLOT(about()));
	helpmenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
}

/*!
 * tablimit property defines the maximum
 * number of profile that can be displayed in the Tabbed
 * widget.
 *
 * \sa setTablimit
 * \sa m_tablimit
 */
int MainWidget::tablimit() const
{
	return m_tablimit;
}

/*!
 * \sa tablimit
 * \sa m_tablimit
 */
void MainWidget::setTablimit(int tablimit)
{
	QSettings settings;
	m_tablimit = tablimit;
	settings.setValue("display/tablimit", m_tablimit);
}

/*! \brief create and show the system tray icon
 *
 * Create the system tray icon, show it and connect its
 * activated() signal to some slot 
 */
void MainWidget::createSystrayIcon()
{
	m_systrayIcon = new QSystemTrayIcon(m_icongrey, this);
	QMenu * menu = new QMenu(QString("SystrayMenu"), this);
        menu->addAction(m_cfgact);
        menu->addSeparator();
	menu->addMenu(m_avail);
	menu->addSeparator();
	menu->addAction(m_connectact);
	menu->addAction(m_disconnectact);
	menu->addSeparator();
	menu->addAction(m_quitact);
	m_systrayIcon->setContextMenu( menu );
	m_systrayIcon->show();
	//connect( m_systrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
	//         this, SLOT(show()) );
	connect( m_systrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
	         this, SLOT(systrayActivated(QSystemTrayIcon::ActivationReason)) );
	connect( m_systrayIcon, SIGNAL(messageClicked()),
	         this, SLOT(systrayMsgClicked()) );
// QSystemTrayIcon::ActivationReason
	//qDebug() << "QSystemTrayIcon::supportsMessages() = "
	//         << QSystemTrayIcon::supportsMessages();
}

// === SLOTS implementations ===

/*! \brief show the Configuration Dialog
 *
 * create and execute a new ConfWidget
 */
void MainWidget::showConfDialog()
{
        m_conf = new ConfWidget(m_engine, this);
	m_conf->exec();
}


/*! \brief process clicks to the systray icon
 *
 * This slot is connected to the activated() signal of the
 * System Tray icon. It currently toggle the visibility
 * of the MainWidget on a simple left click. */
void MainWidget::systrayActivated(QSystemTrayIcon::ActivationReason reason)
{
	qDebug() << "MainWidget::systrayActivated() reason=" << reason
	         << "visible=" << isVisible() << " hidden=" << isHidden()
	         << "active=" << isActiveWindow();
	// QSystemTrayIcon::DoubleClick
	// QSystemTrayIcon::Trigger
	if (reason == QSystemTrayIcon::Trigger)
	{
		if ( isVisible() && !isActiveWindow() )
		{
			showNormal();
			activateWindow();
			raise();
		}
		else
		{
#ifdef Q_WS_MAC
			// try to reduce potential problems under MacOS X
			setVisible(true);
#else
			// Toggle visibility
			setVisible(!isVisible());
#endif
			if ( isVisible() )
			{
				showNormal();
				activateWindow();
				raise();
			}
		}
	}
}

/*!
 * This slot implementation show, activate (and raise) the
 * window.
 */
void MainWidget::systrayMsgClicked()
{
	qDebug() << "MainWidget::systrayMsgClicked()";
	setVisible(true);
	activateWindow();
	raise();
}

/*!
 * enables the "Disconnect" action and disables the "Connect" Action.
 * sets the Green indicator
 * \sa engineStopped()
 */
void MainWidget::engineStarted()
{
	setForceTabs(false);
        QSettings settings;
	QStringList display_capas = QString("customerinfo,features,history,directory,peers,dial,presence").split(",");
	QStringList allowed_capas = m_engine->getCapabilities();
        qDebug() << "MainWidget::setConnected()" << m_engine->checkedPresence() << m_engine->checkedCInfo();

        m_mainlayout->removeWidget(m_xivobg);
        delete m_xivobg;

        if (m_forcetabs || allowed_capas.contains("peers")) {
                m_infowidget = new IdentityDisplay();
                connect( m_engine, SIGNAL(localUserDefined(const QString &)),
                         m_infowidget, SLOT(setUser(const QString &)));
                m_mainlayout->addWidget(m_infowidget, 0);
        }

	m_main_tabwidget = new QTabWidget();
        m_mainlayout->addWidget(m_main_tabwidget, 1);

	for(int j = 0; j < display_capas.size(); j++) {
		QString dc = display_capas[j];
		if (m_forcetabs || allowed_capas.contains(dc)) {
                        qDebug() << "adding" << dc;
			if (dc == QString("instantmessaging")) {
				m_messagetosend = new QLineEdit();
				connect( m_messagetosend, SIGNAL(returnPressed()),
					 this, SLOT(affTextChanged()) );
				m_main_tabwidget->addTab(m_messagetosend, extraspace + tr("&Messages") + extraspace);

			} else if (dc == QString("dial")) {
				m_dial = new DialPanel();
				connect( m_dial, SIGNAL(emitDial(const QString &)),
					 m_engine, SLOT(dialExtension(const QString &)) );
                                connect( m_engine, SIGNAL(pasteToDialPanel(const QString &)),
                                         m_dial, SLOT(setNumberToDial(const QString &)) );

				m_mainlayout->addWidget(m_dial, 0);

			} else if ((dc == QString("customerinfo")) && (m_engine->checkedCInfo())) {
				m_cinfo_tabwidget = new QTabWidget();
				m_cinfo_tabwidget->setObjectName("cinfo");
				m_main_tabwidget->addTab(m_cinfo_tabwidget, extraspace + tr("&Sheets") + extraspace);

			} else if (dc == QString("peers")) {
				m_peerswidget = new SearchPanel();
				m_main_tabwidget->addTab(m_peerswidget, extraspace + tr("&Contacts") + extraspace);

				connect( m_engine, SIGNAL(updatePeer(const QString &, const QString &,
								     const QString &, const QString &,
								     const QString &, const QString &,
								     const QStringList &, const QStringList &,
								     const QStringList &)),
					 m_peerswidget, SLOT(updatePeer(const QString &, const QString &,
									const QString &, const QString &,
									const QString &, const QString &,
									const QStringList &, const QStringList &,
									const QStringList &)) );
				connect( m_peerswidget, SIGNAL(askCallerIds()),
					 m_engine, SLOT(askCallerIds()) );

				m_peerswidget->setEngine(m_engine);

			} else if (dc == QString("features")) {
				m_featureswidget = new ServicePanel();
				m_main_tabwidget->addTab(m_featureswidget, extraspace + tr("S&ervices") + extraspace);

                                connect( m_engine, SIGNAL(disconnectFeatures()),
                                         m_featureswidget, SLOT(DisConnect()) );
                                connect( m_engine, SIGNAL(connectFeatures()),
                                         m_featureswidget, SLOT(Connect()) );
                                connect( m_engine, SIGNAL(resetFeatures()),
                                         m_featureswidget, SLOT(Reset()) );

				connect( m_featureswidget, SIGNAL(voiceMailToggled(bool)),
				         m_engine, SLOT(featurePutVoiceMail(bool)) );
				connect( m_engine, SIGNAL(voiceMailChanged(bool)),
				         m_featureswidget, SLOT(setVoiceMail(bool)) );

				connect( m_featureswidget, SIGNAL(callRecordingToggled(bool)),
				         m_engine, SLOT(featurePutCallRecording(bool)) );
				connect( m_engine, SIGNAL(callRecordingChanged(bool)),
				         m_featureswidget, SLOT(setCallRecording(bool)) );

				connect( m_featureswidget, SIGNAL(callFilteringToggled(bool)),
				         m_engine, SLOT(featurePutCallFiltering(bool)) );
				connect( m_engine, SIGNAL(callFilteringChanged(bool)),
				         m_featureswidget, SLOT(setCallFiltering(bool)) );

				connect( m_featureswidget, SIGNAL(dndToggled(bool)),
				         m_engine, SLOT(featurePutDnd(bool)) );
				connect( m_engine, SIGNAL(dndChanged(bool)),
				         m_featureswidget, SLOT(setDnd(bool)) );

				connect( m_featureswidget, SIGNAL(uncondForwardChanged(bool, const QString &)),
				         m_engine, SLOT(featurePutUncondForward(bool, const QString &)) );
				connect( m_engine, SIGNAL(uncondForwardUpdated(bool, const QString &)),
				         m_featureswidget, SLOT(setUncondForward(bool, const QString &)) );
				connect( m_engine, SIGNAL(uncondForwardUpdated(bool)),
				         m_featureswidget, SLOT(setUncondForward(bool)) );
				connect( m_engine, SIGNAL(uncondForwardUpdated(const QString &)),
				         m_featureswidget, SLOT(setUncondForward(const QString &)) );

				connect( m_featureswidget, SIGNAL(forwardOnBusyChanged(bool, const QString &)),
				         m_engine, SLOT(featurePutForwardOnBusy(bool, const QString &)) );
				connect( m_engine, SIGNAL(forwardOnBusyUpdated(bool, const QString &)),
				         m_featureswidget, SLOT(setForwardOnBusy(bool, const QString &)) );
				connect( m_engine, SIGNAL(forwardOnBusyUpdated(bool)),
				         m_featureswidget, SLOT(setForwardOnBusy(bool)) );
				connect( m_engine, SIGNAL(forwardOnBusyUpdated(const QString &)),
				         m_featureswidget, SLOT(setForwardOnBusy(const QString &)) );

				connect( m_featureswidget, SIGNAL(forwardOnUnavailableChanged(bool, const QString &)),
				         m_engine, SLOT(featurePutForwardOnUnavailable(bool, const QString &)) );
				connect( m_engine, SIGNAL(forwardOnUnavailableUpdated(bool, const QString &)),
				         m_featureswidget, SLOT(setForwardOnUnavailable(bool, const QString &)) );
				connect( m_engine, SIGNAL(forwardOnUnavailableUpdated(bool)),
				         m_featureswidget, SLOT(setForwardOnUnavailable(bool)) );
				connect( m_engine, SIGNAL(forwardOnUnavailableUpdated(const QString &)),
				         m_featureswidget, SLOT(setForwardOnUnavailable(const QString &)) );
				//
				m_engine->askFeatures("peer/to/define");
			} else if (dc == QString("directory")) {
				m_directory = new DirectoryPanel();

				connect( m_directory, SIGNAL(searchDirectory(const QString &)),
					 m_engine, SLOT(searchDirectory(const QString &)) );
				connect( m_directory, SIGNAL(emitDial(const QString &)),
					 m_engine, SLOT(dialFullChannel(const QString &)) );
				connect( m_directory, SIGNAL(copyNumber(const QString &)),
					 m_engine, SLOT(copyNumber(const QString &)) );
				//connect( m_directory, SIGNAL(transferCall(const QString &, const QString &)),
				//m_engine, SLOT(transferCall(const QString &, const QString &)) );
				//connect( m_directory, SIGNAL(originateCall(const QString &, const QString &)),
				//m_engine, SLOT(originateCall(const QString &, const QString &)) );
				
				connect( m_engine, SIGNAL(directoryResponse(const QString &)),
					 m_directory, SLOT(setSearchResponse(const QString &)) );
				//connect( m_engine, SIGNAL(updateMyCalls(const QStringList &, const QStringList &, const QStringList &)),
				//m_directory, SIGNAL(updateMyCalls(const QStringList &, const QStringList &, const QStringList &)) );
				//connect( m_engine, SIGNAL(stopped()),
				//m_directory, SLOT(stop()) );
				
				//			m_mainlayout->addWidget(m_directory, 0);
				m_main_tabwidget->addTab(m_directory, extraspace + tr("&Directory") + extraspace);
				
			} else if (dc == QString("history")) {
				m_history = new LogWidget(m_engine, this);
                                qDebug() << "MainWidget::setConnected()" << m_engine->phoneNum();
				m_history->setPeerToDisplay("p/" +
                                                            m_engine->serverast() + "/" +
                                                            m_engine->dialContext() +  "/" +
							    m_engine->protocol() + "/" +
                                                            m_engine->userId() + "/" +
                                                            m_engine->phoneNum());
				connect( m_history, SIGNAL(askHistory(const QString &, int)),
					 m_engine, SLOT(requestHistory(const QString &, int)) );
				connect( m_engine, SIGNAL(updateLogEntry(const QDateTime &, int, const QString &, int)),
					 m_history, SLOT(addLogEntry(const QDateTime &, int, const QString &, int)) );
				//			m_mainlayout->addWidget(m_history, 0);
				m_main_tabwidget->addTab(m_history, extraspace + tr("&History") + extraspace);
			}
		}
	}

        qDebug() << "display/lastfocusedtab =" << settings.value("display/lastfocusedtab");
        m_main_tabwidget->setCurrentIndex(settings.value("display/lastfocusedtab").toInt());

	m_cinfo_index = m_main_tabwidget->indexOf(m_cinfo_tabwidget);
	qDebug() << "the index of customer-info widget is" << m_cinfo_index;
	if (m_systrayIcon)
		m_systrayIcon->setIcon(m_icon);

	statusBar()->showMessage(tr("Connected"));
	m_connectact->setEnabled(false);
	m_disconnectact->setEnabled(true);
	// set status icon to green
	QPixmap greensquare(":/images/connected.png");
	m_status->setPixmap(greensquare);
}

/*!
 * disables the "Disconnect" action and enables the "Connect" Action.
 * sets the Red indicator
 * \sa engineStarted()
 */
void MainWidget::engineStopped()
{
        QSettings settings;
	QStringList display_capas = QString("customerinfo,features,history,directory,peers,dial,presence").split(",");
	QStringList allowed_capas = m_engine->getCapabilities();

        if (m_main_tabwidget->currentIndex() > -1)
                settings.setValue("display/lastfocusedtab", m_main_tabwidget->currentIndex());

	for(int j = 0; j < display_capas.size(); j++) {
	        QString dc = display_capas[j];
                if (m_forcetabs || allowed_capas.contains(dc)) {
			if (dc == QString("instantmessaging")) {
                                int index_instantmessaging = m_main_tabwidget->indexOf(m_messagetosend);
                                if (index_instantmessaging > -1) {
                                        qDebug() << "removing" << dc << index_instantmessaging;
                                        m_main_tabwidget->removeTab(index_instantmessaging);
                                        delete m_messagetosend;
                                }
			} else if (dc == QString("customerinfo")) {
                                int index_customerinfo = m_main_tabwidget->indexOf(m_cinfo_tabwidget);
                                if (index_customerinfo > -1) {
                                        qDebug() << "removing" << dc << index_customerinfo;
                                        m_main_tabwidget->removeTab(index_customerinfo);
                                        delete m_cinfo_tabwidget;
                                }
			} else if (dc == QString("peers")) {
                                int index_peers = m_main_tabwidget->indexOf(m_peerswidget);
                                if (index_peers > -1) {
                                        qDebug() << "removing" << dc << index_peers;
                                        m_peerswidget->removePeers();
                                        m_main_tabwidget->removeTab(index_peers);
                                        delete m_peerswidget;
                                }
			} else if (dc == QString("features")) {
                                int index_features = m_main_tabwidget->indexOf(m_featureswidget);
                                if (index_features > -1) {
                                        qDebug() << "removing" << dc << index_features;
                                        m_main_tabwidget->removeTab(index_features);
                                        delete m_featureswidget;
                                }
			} else if (dc == QString("directory")) {
                                int index_directory = m_main_tabwidget->indexOf(m_directory);
                                if (index_directory > -1) {
                                        qDebug() << "removing" << dc << index_directory;
                                        m_main_tabwidget->removeTab(index_directory);
                                        delete m_directory;
                                }
			} else if (dc == QString("history")) {
                                int index_history = m_main_tabwidget->indexOf(m_history);
                                if (index_history > -1) {
                                        qDebug() << "removing" << dc << index_history;
                                        m_main_tabwidget->removeTab(index_history);
                                        delete m_history;
                                }
			} else if (dc == QString("dial")) {
                                int index_dial = m_mainlayout->indexOf(m_dial);
                                if (index_dial > -1) {
                                        qDebug() << "removing" << dc << index_dial;
                                        m_mainlayout->removeWidget(m_dial);
                                        delete m_dial;
                                }
			}
		}
	}
        
        if (m_forcetabs || allowed_capas.contains("peers")) {
                m_mainlayout->removeWidget(m_infowidget);
                delete m_infowidget;
        }

        m_mainlayout->removeWidget(m_main_tabwidget);
        delete m_main_tabwidget;

        m_xivobg = new QLabel();
        m_xivobg->setPixmap(QPixmap(":/images/xivo-login.png"));
        m_mainlayout->addWidget(m_xivobg, 0, Qt::AlignHCenter | Qt::AlignVCenter);

	if (m_systrayIcon)
		m_systrayIcon->setIcon(m_icongrey);

	statusBar()->showMessage(tr("Disconnected"));
	m_connectact->setEnabled(true);
	m_disconnectact->setEnabled(false);
	// set status icon to red
	QPixmap redsquare(":/images/disconnected.png");
	m_status->setPixmap(redsquare);
}

void MainWidget::setForceTabs(bool force)
{
	m_forcetabs = force;
}

void MainWidget::savePositions() const
{
	qDebug() << "MainWidget::savePositions()";
        QSettings settings;
        settings.setValue("display/mainwingeometry", saveGeometry());
}

/*!
 * Display the new profile in the tabbed area
 * and show a message with the systray icon
 */
void MainWidget::showNewProfile(Popup * popup)
{
	QTime currentTime = QTime::currentTime();
	QString currentTimeStr = currentTime.toString("hh:mm:ss");
	if (m_systrayIcon)
	{
		m_systrayIcon->showMessage(tr("Incoming call"),
		                           currentTimeStr + "\n"
					   + popup->message() );
	}
	if (m_cinfo_tabwidget)
	{
		int index = m_cinfo_tabwidget->addTab(popup, extraspace + currentTimeStr + extraspace);
		qDebug() << "added tab" << index;
		m_cinfo_tabwidget->setCurrentIndex(index);
		if (m_cinfo_index > -1)
			m_main_tabwidget->setCurrentIndex(m_cinfo_index);
		if (index >= m_tablimit)
		{
			// close the first widget
			m_cinfo_tabwidget->widget(0)->close();
		}
                connect( popup, SIGNAL(emitDial(const QString &)),
                         m_engine, SLOT(dialExtension(const QString &)) );
		// show the window and give it the focus.
		setVisible(true);
		activateWindow();
		raise();
	}
	else
	{
		popup->show();
	}
}

void MainWidget::hideEvent(QHideEvent *event)
{
	// called when minimized
	//qDebug() << "MainWidget::hideEvent(" << event << ")";
	// if systray available
	qDebug() << "MainWidget::hideEvent : spontaneous="
	         << event->spontaneous() << " isMinimized()="
			 << isMinimized();
	//if (event->spontaneous())
	//	event->ignore();
	//else
		event->accept();
#ifndef Q_WS_MAC
	if ( QSystemTrayIcon::isSystemTrayAvailable() )
		setVisible(false);
#endif
}

/*! \brief Catch the Close event
 *
 * This method is called when the user click the upper right X of the
 * Window.
 * We ignore the event but hide the window (to minimize it to systray)
 */
void MainWidget::closeEvent(QCloseEvent *event)
{
	qDebug() << "MainWidget::closeEvent()";
	//event->accept();
#ifndef Q_WS_MAC
	if ( QSystemTrayIcon::isSystemTrayAvailable() )
		setVisible( false );
	else
#endif
		showMinimized();
	event->ignore();
}

#if 0
void MainWidget::changeEvent(QEvent *event)
{
	qDebug() << "MainWidget::changeEvent() eventtype=" << event->type();
	//if (event->type() == 105)
	//	event->accept();
		//event->ignore();
}
#endif

void MainWidget::dispurl(const QUrl &url)
{
        qDebug() << "MainWidget::dispurl()" << url;
}


/*! \brief Displays the about box
 *
 * use QMessageBox::about() to display
 * the version and informations about the application
 */
void MainWidget::about()
{
	QString applicationVersion("0.1");
        QString fetchlastone = "<a href=http://www.xivo.fr/download/xivo_cti_client/"
#if defined(Q_WS_X11)
                "linux"
#elif defined(Q_WS_WIN)
                "win32"
#elif defined(Q_WS_MAC)
                "macos"
#endif
                ">" + tr("last one") + "</a>";

        QMessageBox::about(this,
                           tr("About XIVO Client"),
                           "<h3>XIVO Client</h3>" +
                           tr("<p>This application allows a given phone user to :</p>"
			      "<p>* receive customer informations related to incoming calls</p>"
			      "<p>* manage her/his voicemail and transfers</p>"
			      "<p>* know her/his calls history</p>"
			      "<p>* access the phones and addresses' directory</p>"
			      "<p>* see her/his buddies</p>"
			      "<p>* originate a dial towards some number</p>") +
			   "<p><b>" + tr("Version : ") + QString("</b>%1 (").arg(applicationVersion) +
			   "<b>svn : " + QString("</b>%1 - %2)</p>").arg(SVNVER,
                                                                         fetchlastone) +
			   "<hr><p>(C) 2007 <a href=http://www.proformatique.com><b>Proformatique</b></a></p>"
			   "<p>67 rue Voltaire 92800 Puteaux FRANCE</p>"
			   "<p><b>E-mail : </b><a href=mailto:technique@proformatique.com>technique@proformatique.com</p>"
			   "<p>(+33 / 0) 1.41.38.99.60</p>" +
			   "<p><b>" + tr("Authors : ") + "</b>Thomas Bernard, Corentin Le Gall</p>" +
			   "<hr><p><b>" + tr("License : ") + "</b>" +
			   "<a href=http://www.gnu.org/licenses/gpl-2.0.txt>GNU General Public Licence v.2</a></p>");
}
