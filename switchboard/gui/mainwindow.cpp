/*
XIVO switchboard : 
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
   $Date$
*/

#include <QStatusBar>
#include <QMenuBar>
#include <QApplication>
#include <QMessageBox>
#include <QSplitter>
#include <QScrollArea>
#include <QLineEdit>
#include <QLabel>
#include <QSettings>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QDebug>
#include <QTabWidget>
#include "mainwindow.h"
#include "baseengine.h"
#include "loginengine.h"
#include "switchboardwindow.h"
#include "switchboardconf.h"
#include "callstackwidget.h"
#include "searchpanel.h"
#include "logwidget.h"
#include "dialpanel.h"
#include "directorypanel.h"
#include "displaymessages.h"
#include "servicepanel.h"
#include "popup.h"

/*! \brief Widget containing the CallStackWidget and a Title QLabel
 */
class LeftPanel : public QWidget
{
public:
	LeftPanel(QWidget *, QWidget * parent = 0);	//!< Constructor
	QLabel * titleLabel();	//!< getter for m_titleLabel
private:
	QLabel * m_titleLabel;	//!< Title label property
};

LeftPanel::LeftPanel(QWidget * bottomWidget,QWidget * parent)
: QWidget(parent)
{
	QVBoxLayout * layout = new QVBoxLayout(this);
	layout->setMargin(0);
	m_titleLabel = new QLabel("", this);
	layout->addWidget(m_titleLabel, 0, Qt::AlignCenter);
	layout->addWidget(bottomWidget, 1);
}

QLabel * LeftPanel::titleLabel()
{
	return m_titleLabel;
}

/*!
 * Construct the Widget with all subwidgets : a left panel for
 * displaying calls and a right panel for peers.
 * The geometry is restored from settings.
 */
MainWindow::MainWindow(BaseEngine * engine, LoginEngine * loginengine, QWidget * parent)
	: QMainWindow(parent), m_engine(engine), m_loginengine(loginengine)
{
	QPixmap redsquare(15,15);
	redsquare.fill(Qt::red);
	statusBar();	// This creates the status bar.
	m_status = new QLabel();
	m_status->setPixmap(redsquare);
	statusBar()->addPermanentWidget(m_status);
	setWindowIcon(QIcon(":/xivoicon.png"));
	setWindowTitle("XIVO Switchboard");

	m_splitter = new QSplitter(this);
	m_leftSplitter = new QSplitter(Qt::Vertical, m_splitter);

	/* (0, 0) "position" : Calls */
	QScrollArea * areaCalls = new QScrollArea(this);
	LeftPanel * leftPanel = new LeftPanel(areaCalls, m_leftSplitter);


	/* (0, 1) "position" : Tabs */
	m_svc_tabwidget = new QTabWidget(m_leftSplitter);

	DisplayMessagesPanel * lbl = new DisplayMessagesPanel(m_svc_tabwidget);
	m_svc_tabwidget->addTab(lbl, tr("Messages"));

	calls = new CallStackWidget(areaCalls);
	connect( calls, SIGNAL(changeTitle(const QString &)),
	         leftPanel->titleLabel(), SLOT(setText(const QString &)) );
	connect( m_engine, SIGNAL(updateCall(const QString &, const QString &, int, const QString &,
					     const QString &, const QString &, const QString &)),
		 calls, SLOT(addCall(const QString &, const QString &, int, const QString &,
				     const QString &, const QString &, const QString &)) );
	connect( m_engine, SIGNAL(callsUpdated()),
	         calls, SLOT(updateDisplay()) );
	connect( m_engine, SIGNAL(stopped()),
	         calls, SLOT(reset()) );
	connect( m_engine, SIGNAL(monitorPeer(const QString &, const QString &)),
	         calls, SLOT(monitorPeer(const QString &, const QString &)) );
	connect( calls, SIGNAL(hangUp(const QString &)),
		 m_engine, SLOT(hangUp(const QString &)) );

	m_middleSplitter = new QSplitter( Qt::Vertical, m_splitter);

	//QScrollArea * areaPeers = new QScrollArea(m_splitter);
	QScrollArea * areaPeers = new QScrollArea(m_middleSplitter);
	areaCalls->setWidgetResizable(true);
	areaPeers->setWidgetResizable(true);

 	m_widget = new SwitchBoardWindow(areaPeers);
 	m_widget->setEngine(engine);
	engine->addRemovable(m_widget->metaObject());
	connect( engine, SIGNAL(updatePeer(const QString &, const QString &,
	                                   const QString &, const QString &,
	                                   const QString &, const QString &,
					   const QStringList &, const QStringList &,
					   const QStringList &)),
	         m_widget, SLOT(updatePeer(const QString &, const QString &,
					   const QString &, const QString &,
					   const QString &, const QString &,
					   const QStringList &, const QStringList &,
					   const QStringList &)) );
	connect( engine, SIGNAL(stopped()),
	         m_widget, SLOT(removePeers()) );
	connect( engine, SIGNAL(removePeer(const QString &)),
	         m_widget, SLOT(removePeer(const QString &)) );
 	areaPeers->setWidget(m_widget);
 	areaCalls->setWidget(calls);
	
	DirectoryPanel * dirpanel = new DirectoryPanel(m_middleSplitter);
	connect( dirpanel, SIGNAL(searchDirectory(const QString &)),
	         engine, SLOT(searchDirectory(const QString &)) );
	connect( engine, SIGNAL(directoryResponse(const QString &)),
	         dirpanel, SLOT(setSearchResponse(const QString &)) );
	connect( dirpanel, SIGNAL(emitDial(const QString &)),
	         engine, SLOT(dialExtension(const QString &)) );
	connect( dirpanel, SIGNAL(transferCall(const QString &, const QString &)),
	         engine, SLOT(transferCall(const QString &, const QString &)) );
	connect( dirpanel, SIGNAL(originateCall(const QString &, const QString &)),
	         engine, SLOT(originateCall(const QString &, const QString &)) );
	connect( engine, SIGNAL(updateMyCalls(const QStringList &, const QStringList &, const QStringList &)),
	         dirpanel, SIGNAL(updateMyCalls(const QStringList &, const QStringList &, const QStringList &)) );
	connect( engine, SIGNAL(stopped()),
	         dirpanel, SLOT(stop()) );

	m_rightSplitter = new QSplitter(Qt::Vertical, m_splitter);

	SearchPanel * searchpanel = new SearchPanel(m_rightSplitter);
	searchpanel->setEngine(engine);
	connect( engine, SIGNAL(updatePeer(const QString &, const QString &,
	                                   const QString &, const QString &,
	                                   const QString &, const QString &,
					   const QStringList &, const QStringList &,
					   const QStringList &)),
	         searchpanel, SLOT(updatePeer(const QString &, const QString &,
					      const QString &, const QString &,
					      const QString &, const QString &,
					      const QStringList &, const QStringList &,
					      const QStringList &)) );
	connect( engine, SIGNAL(stopped()),
	         searchpanel, SLOT(removePeers()) );
	connect( engine, SIGNAL(removePeer(const QString &)),
	         searchpanel, SLOT(removePeer(const QString &)) );
	
	m_tabwidget = new QTabWidget(m_rightSplitter);
	connect( loginengine, SIGNAL(newProfile(Popup *)),
	         this, SLOT(showNewProfile(Popup *)) );

	DialPanel * dialpanel = new DialPanel(m_rightSplitter);
	connect( dialpanel, SIGNAL(emitDial(const QString &)),
	         m_engine, SLOT(dialExtension(const QString &)) );
        connect( dialpanel, SIGNAL(originateCall(const QString&, const QString&)),
	         m_engine, SLOT(originateCall(const QString&, const QString&)) );

	setCentralWidget(m_splitter);

	// restore splitter settings
	QSettings settings;
	m_tablimit = settings.value("display/tablimit", 5).toInt();
	m_splitter->restoreState(settings.value("display/splitterSizes").toByteArray());
	m_leftSplitter->restoreState(settings.value("display/leftSplitterSizes").toByteArray());
	m_middleSplitter->restoreState(settings.value("display/middleSplitterSizes").toByteArray());
	m_rightSplitter->restoreState(settings.value("display/rightSplitterSizes").toByteArray());

	restoreGeometry(settings.value("display/mainwingeometry").toByteArray());
 	connect(m_engine, SIGNAL(emitTextMessage(const QString &)),
 	        statusBar(), SLOT(showMessage(const QString &)));
	connect(m_engine, SIGNAL(emitTextMessage(const QString &)),
	        lbl, SLOT(addMessage(const QString &)));
	connect(m_engine, SIGNAL(started()),
	        this, SLOT(engineStarted()));
	connect(m_engine, SIGNAL(stopped()),
	        this, SLOT(engineStopped()));
	
	connect(m_loginengine, SIGNAL(logged()),
 		this, SLOT(loginengineStarted()) );
#if 0
	connect(m_loginengine, SIGNAL(logged()),
		m_engine, SLOT(start()) );
	connect(m_loginengine, SIGNAL(delogged()),
		m_engine, SLOT(stop()) );
#endif
	
	QMenu * menu = menuBar()->addMenu(tr("&File"));
	
	m_startact = new QAction(tr("S&tart"), this);
	m_startact->setStatusTip(tr("Start"));
	connect(m_startact, SIGNAL(triggered()),
		m_engine, SLOT(start()) );
	connect(m_startact, SIGNAL(triggered()),
		m_loginengine, SLOT(start()) );
	menu->addAction(m_startact);

	m_stopact = new QAction(tr("Sto&p"), this);
	m_stopact->setStatusTip(tr("Stop"));
	connect(m_stopact, SIGNAL(triggered()),
		m_engine, SLOT(stop()) );
	connect(m_stopact, SIGNAL(triggered()),
		m_loginengine, SLOT(stop()) );
	m_stopact->setDisabled(true);
	menu->addAction(m_stopact);

	QAction * conf = new QAction(tr("&Configure"), this);
	conf->setStatusTip(tr("Open the configuration dialog"));
	connect(conf, SIGNAL(triggered()), this, SLOT(showConfDialog()));
	menu->addAction(conf);

	QAction * quit = new QAction(tr("&Quit"), this);
	connect(quit, SIGNAL(triggered()), m_loginengine, SLOT(stop()) );
	connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));
	menu->addAction(quit);

	// Availability actions :
	m_availgrp = new QActionGroup( this );
	m_availgrp->setExclusive(true);

	m_avact_avail = new QAction( tr("&Available"), this );
	m_avact_avail->setCheckable(true);
	connect( m_avact_avail, SIGNAL(triggered()),
	         m_loginengine, SLOT(setAvailable()) );
	m_availgrp->addAction( m_avact_avail );
	m_avact_away = new QAction( tr("A&way"), this );
	m_avact_away->setCheckable(true);
	connect( m_avact_away, SIGNAL(triggered()),
	         m_loginengine, SLOT(setAway()) );
	m_availgrp->addAction( m_avact_away );
	m_avact_brb = new QAction( tr("&Be Right Back"), this );
	m_avact_brb->setCheckable(true);
	connect( m_avact_brb, SIGNAL(triggered()),
	         m_loginengine, SLOT(setBeRightBack()) );
	m_availgrp->addAction( m_avact_brb );
	m_avact_otl = new QAction( tr("&Out To Lunch"), this );
	m_avact_otl->setCheckable(true);
	connect( m_avact_otl, SIGNAL(triggered()),
	         m_loginengine, SLOT(setOutToLunch()) );
	m_availgrp->addAction( m_avact_otl );
	m_avact_dnd = new QAction( tr("&Do not disturb"), this );
	m_avact_dnd->setCheckable(true);
	connect( m_avact_dnd, SIGNAL(triggered()),
	         m_loginengine, SLOT(setDoNotDisturb()) );
	m_availgrp->addAction( m_avact_dnd );

	if(m_loginengine->getAvailState() == QString("berightback"))
		m_avact_brb->setChecked( true );
	else if(m_loginengine->getAvailState() == QString("donotdisturb"))
		m_avact_dnd->setChecked( true );
	else if(m_loginengine->getAvailState() == QString("away"))
		m_avact_away->setChecked( true );
	else if(m_loginengine->getAvailState() == QString("outtolunch"))
		m_avact_otl->setChecked( true );
	else
		m_avact_avail->setChecked( true );

	m_avail = menuBar()->addMenu(tr("&Availability"));
	m_avail->addActions( m_availgrp->actions() );
	m_avail->setEnabled( m_loginengine->enabled() );
	connect( m_loginengine, SIGNAL(availAllowChanged(bool)),
	         m_avail, SLOT(setEnabled(bool)) );

	QMenu * helpmenu = menuBar()->addMenu(tr("&Help"));
	helpmenu->addAction(tr("&About XIVO Switchboard"), this, SLOT(about()));
	helpmenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
}

/*! \brief Destructor
 *
 * The Geometry settings are saved for use by the new instance
 */
MainWindow::~MainWindow()
{
	QSettings settings;
	settings.setValue("display/splitterSizes", m_splitter->saveState());
	settings.setValue("display/leftSplitterSizes", m_leftSplitter->saveState());
	settings.setValue("display/middleSplitterSizes", m_middleSplitter->saveState());
	settings.setValue("display/rightSplitterSizes", m_rightSplitter->saveState());
	settings.setValue("display/mainwingeometry", saveGeometry());
}

/*! \brief show the Configuration Dialog
 *
 * create and execute a new SwitchBoardConfDialog
 */
void MainWindow::showConfDialog()
{
	SwitchBoardConfDialog * conf = new SwitchBoardConfDialog(m_engine, m_loginengine,
								 m_widget, this);
	qDebug() << "<<  " << conf->exec();
}

/*!
 * enable the "Stop" action and disable "Start" Action.
 * \sa engineStopped()
 */
void MainWindow::engineStarted()
{
	m_stopact->setEnabled(true);
	m_startact->setDisabled(true);
	m_loginengine->start();

	m_logwidget = new LogWidget(m_engine, m_svc_tabwidget);
	m_svc_tabwidget->insertTab(0, m_logwidget, tr("History"));

	connect( m_engine, SIGNAL(updateLogEntry(const QDateTime &, int, const QString &, int)),
	         m_logwidget, SLOT(addLogEntry(const QDateTime &, int, const QString &, int)) );
	connect( m_logwidget, SIGNAL(askHistory(const QString &, int)),
	         m_engine, SLOT(requestHistory(const QString &, int)) );
	connect( m_engine, SIGNAL(stopped()),
	         m_logwidget, SLOT(clear()) );
	connect( calls, SIGNAL(monitoredPeerChanged(const QString &)),
	         m_logwidget, SLOT(setPeerToDisplay(const QString &)) );

	m_featureswidget = new ServicePanel(m_svc_tabwidget);
	m_svc_tabwidget->insertTab(0, m_featureswidget, tr("Services"));

	connect( m_featureswidget, SIGNAL(voiceMailToggled(bool)),
		 m_loginengine, SLOT(setVoiceMail(bool)) );
	connect( m_loginengine, SIGNAL(voiceMailChanged(bool)),
		 m_featureswidget, SLOT(setVoiceMail(bool)) );
	
	connect( m_featureswidget, SIGNAL(callRecordingToggled(bool)),
		 m_loginengine, SLOT(setCallRecording(bool)) );
	connect( m_loginengine, SIGNAL(callRecordingChanged(bool)),
		 m_featureswidget, SLOT(setCallRecording(bool)) );
	
	connect( m_featureswidget, SIGNAL(callFilteringToggled(bool)),
		 m_loginengine, SLOT(setCallFiltering(bool)) );
	connect( m_loginengine, SIGNAL(callFilteringChanged(bool)),
		 m_featureswidget, SLOT(setCallFiltering(bool)) );
	
	connect( m_featureswidget, SIGNAL(dndToggled(bool)),
		 m_loginengine, SLOT(setDnd(bool)) );
	connect( m_loginengine, SIGNAL(dndChanged(bool)),
		 m_featureswidget, SLOT(setDnd(bool)) );
	
	connect( m_featureswidget, SIGNAL(uncondForwardChanged(bool, const QString &)),
		 m_loginengine, SLOT(setUncondForward(bool, const QString &)) );
	connect( m_loginengine, SIGNAL(uncondForwardChanged(bool, const QString &)),
		 m_featureswidget, SLOT(setUncondForward(bool, const QString &)) );
	connect( m_loginengine, SIGNAL(uncondForwardChanged(bool)),
		 m_featureswidget, SLOT(setUncondForward(bool)) );
	connect( m_loginengine, SIGNAL(uncondForwardChanged(const QString &)),
		 m_featureswidget, SLOT(setUncondForward(const QString &)) );
	
	connect( m_featureswidget, SIGNAL(forwardOnBusyChanged(bool, const QString &)),
		 m_loginengine, SLOT(setForwardOnBusy(bool, const QString &)) );
	connect( m_loginengine, SIGNAL(forwardOnBusyChanged(bool, const QString &)),
		 m_featureswidget, SLOT(setForwardOnBusy(bool, const QString &)) );
	connect( m_loginengine, SIGNAL(forwardOnBusyChanged(bool)),
		 m_featureswidget, SLOT(setForwardOnBusy(bool)) );
	connect( m_loginengine, SIGNAL(forwardOnBusyChanged(const QString &)),
		 m_featureswidget, SLOT(setForwardOnBusy(const QString &)) );
	
	connect( m_featureswidget, SIGNAL(forwardOnUnavailableChanged(bool, const QString &)),
		 m_loginengine, SLOT(setForwardOnUnavailable(bool, const QString &)) );
	connect( m_loginengine, SIGNAL(forwardOnUnavailableChanged(bool, const QString &)),
		 m_featureswidget, SLOT(setForwardOnUnavailable(bool, const QString &)) );
	connect( m_loginengine, SIGNAL(forwardOnUnavailableChanged(bool)),
		 m_featureswidget, SLOT(setForwardOnUnavailable(bool)) );
	connect( m_loginengine, SIGNAL(forwardOnUnavailableChanged(const QString &)),
		 m_featureswidget, SLOT(setForwardOnUnavailable(const QString &)) );
	//

	m_svc_tabwidget->setCurrentIndex(0);

	// set status icon to green
	QPixmap greensquare(15,15);
	greensquare.fill(Qt::green);
	m_status->setPixmap(greensquare);
}

/*!
 * disable the "Stop" action and enable "Start" Action.
 * \sa engineStarted()
 */
void MainWindow::engineStopped()
{
	m_stopact->setDisabled(true);
	m_startact->setEnabled(true);
	m_loginengine->stop();

	m_svc_tabwidget->removeTab(m_svc_tabwidget->indexOf(m_featureswidget));
	delete m_featureswidget;
	m_svc_tabwidget->removeTab(m_svc_tabwidget->indexOf(m_logwidget));
	delete m_logwidget;

	// set status icon to red
	QPixmap redsquare(15,15);
	redsquare.fill(Qt::red);
	m_status->setPixmap(redsquare);
}

/*!
 * enable the "Log off" action and disable "Login" Action.
 */
void MainWindow::loginengineStarted()
{
	m_engine->setDialContext(m_loginengine->dialContext());
	m_loginengine->askFeatures();
}

/*!
 * tablimit property defines the maximum
 * number of profile that can be displayed in the Tabbed
 * widget.
 *
 * \sa setTablimit
 * \sa m_tablimit
 */
int MainWindow::tablimit() const
{
	return m_tablimit;
}

/*!
 * \sa tablimit
 * \sa m_tablimit
 */
void MainWindow::setTablimit(int tablimit)
{
	QSettings settings;
	m_tablimit = tablimit;
	settings.setValue("display/tablimit", m_tablimit);
}

/*!
 * Display the new profile in the tabbed area
 * and show a message with the systray icon
 */
void MainWindow::showNewProfile(Popup * popup)
{
	qDebug() << "MainWindow::showNewProfile()";
	QTime currentTime = QTime::currentTime();
	QString currentTimeStr = currentTime.toString("hh:mm:ss");
	/*	if(m_systrayIcon)
	  {
	  m_systrayIcon->showMessage(tr("Incoming call"),
	  currentTimeStr + "\n"
	  + popup->message() );
	  }*/
	if(m_tabwidget)
	{
		int index = m_tabwidget->addTab(popup, currentTimeStr);
		qDebug() << "added tab" << index;
		m_tabwidget->setCurrentIndex(index);
		if(index >= m_tablimit)
		{
			// close the first widget
			m_tabwidget->widget(0)->close();
		}
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

/*! \brief Display the about box
 *
 * use QMessageBox::about() to display the application about box
 */
void MainWindow::about()
{
	QString applicationVersion("0.1");
	QString revision("$Revision$ 0");
	QString revdate("$Date$ 0 0");
	QMessageBox::about(this,
			   tr("About XIVO SwitchBoard"),
			   "<h3>XIVO Switchboard</h3>" +
			   tr("<p>This application displays the status of the"
			      " ongoing phone calls.</p>") +
			   "<p><b>" + tr("Version : ") + QString("</b>%1 (").arg(applicationVersion) +
			   "<b>svn : " + QString("</b>%1 @ %2 %3)</p>").arg(revision.split(" ")[1],
									    revdate.split(" ")[1],
									    revdate.split(" ")[2]) +
			   "<hr><p>(C) 2007 <a href=http://www.proformatique.com><b>Proformatique</b></a></p>"
			   "<p>67 rue Voltaire 92800 Puteaux FRANCE</p>"
			   "<p><b>E-mail : </b>technique@proformatique.com</p>"
			   "<p>(+33 / 0) 1.41.38.99.60</p>" +
			   "<p><b>" + tr("Authors : ") + "</b>Thomas Bernard, Corentin Le Gall</p>" +
			   "<hr><p><b>" + tr("License : ") + "</b>" +
			   "<a href=http://www.gnu.org/licenses/gpl-2.0.txt>GNU General Public Licence v.2</a></p>");
}

