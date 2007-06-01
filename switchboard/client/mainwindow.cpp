/* $Id$ */
/* Copyright (C) 2007 Proformatique */

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
#include "mainwindow.h"
#include "switchboardengine.h"
#include "switchboardwindow.h"
#include "switchboardconf.h"
#include "callstackwidget.h"
#include "searchpanel.h"
#include "logwidget.h"
#include "dialpanel.h"
#include "directorypanel.h"
#include "displaymessages.h"

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
MainWindow::MainWindow(SwitchBoardEngine * engine)
: m_engine(engine)
{
	statusBar();	// This creates the status bar.
	setWindowIcon(QIcon(":/xivoicon.png"));
	setWindowTitle("Xivo Switchboard");

	m_splitter = new QSplitter(this);
	m_leftSplitter = new QSplitter(Qt::Vertical, m_splitter);

	QScrollArea * areaCalls = new QScrollArea(this);
	LeftPanel * leftPanel = new LeftPanel(areaCalls, m_leftSplitter);
	//QScrollArea * areaLog = new QScrollArea(m_leftSplitter);
	//areaLog->setWidgetResizable(true);
	//LogWidget * logwidget = new LogWidget(m_engine, areaLog);
	LogWidget * logwidget = new LogWidget(m_engine, m_leftSplitter);
	//areaLog->setWidget(logwidget);
	connect( engine, SIGNAL(updateLogEntry(const QDateTime &, int, const QString &, int)),
	         logwidget, SLOT(addLogEntry(const QDateTime &, int, const QString &, int)) );

 	//QScrollArea * dialArea = new QScrollArea(leftsplitter);
 	//dialArea->setWidgetResizable(false);

	CallStackWidget * calls = new CallStackWidget(areaCalls);
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
	connect( calls, SIGNAL(hangUp(const QString &)),
		 m_engine, SLOT(hangUp(const QString &)) );

	connect( calls, SIGNAL(monitoredPeerChanged(const QString &)),
	         logwidget, SLOT(setPeerToDisplay(const QString &)) );
	connect( logwidget, SIGNAL(askHistory(const QString &, int)),
	         m_engine, SLOT(requestHistory(const QString &, int)) );

	m_middleSplitter = new QSplitter( Qt::Vertical, m_splitter);

	//QScrollArea * areaPeers = new QScrollArea(m_splitter);
	QScrollArea * areaPeers = new QScrollArea(m_middleSplitter);
	areaCalls->setWidgetResizable(true);
	areaPeers->setWidgetResizable(true);

 	m_widget = new SwitchBoardWindow(areaPeers);
 	m_widget->setEngine(engine);
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

	DisplayMessagesPanel * lbl = new DisplayMessagesPanel(m_rightSplitter);
	
	DialPanel * dialpanel = new DialPanel(m_rightSplitter);
	connect( dialpanel, SIGNAL(emitDial(const QString &)),
	         engine, SLOT(dialExtension(const QString &)) );
	
	setCentralWidget(m_splitter);

	// restore splitter settings
	QSettings settings;
	m_splitter->restoreState(settings.value("display/splitterSizes").toByteArray());
	m_leftSplitter->restoreState(settings.value("display/leftSplitterSizes").toByteArray());
	m_middleSplitter->restoreState(settings.value("display/middleSplitterSizes").toByteArray());
	m_rightSplitter->restoreState(settings.value("display/rightSplitterSizes").toByteArray());

	restoreGeometry(settings.value("display/mainwingeometry").toByteArray());
 	connect(m_engine, SIGNAL(emitTextMessage(const QString &)),
 	        statusBar(), SLOT(showMessage(const QString &)));
	connect(m_engine, SIGNAL(emitTextMessage(const QString &)),
	        lbl, SLOT(updateMessage(const QString &)));
	connect(m_engine, SIGNAL(started()),
	        this, SLOT(engineStarted()));
	connect(m_engine, SIGNAL(stopped()),
	        this, SLOT(engineStopped()));

	QMenu * menu = menuBar()->addMenu(tr("&File"));
	
	m_startact = new QAction(tr("S&tart"), this);
	m_startact->setStatusTip(tr("Start"));
	connect(m_startact, SIGNAL(triggered()), m_engine, SLOT(start()) );
	menu->addAction(m_startact);

	m_stopact = new QAction(tr("Sto&p"), this);
	m_stopact->setStatusTip(tr("Stop"));
	connect(m_stopact, SIGNAL(triggered()), m_engine, SLOT(stop()) );
	m_stopact->setDisabled(true);
	menu->addAction(m_stopact);

	QAction * conf = new QAction(tr("&Configure"), this);
	conf->setStatusTip(tr("Open the configuration dialog"));
	connect(conf, SIGNAL(triggered()), this, SLOT(showConfDialog()));
	menu->addAction(conf);

	QAction * quit = new QAction(tr("&Quit"), this);
	connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));
	menu->addAction(quit);

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

	if(m_engine->getAvailState() == QString("berightback"))
		m_avact_brb->setChecked( true );
	else if(m_engine->getAvailState() == QString("donotdisturb"))
		m_avact_dnd->setChecked( true );
	else if(m_engine->getAvailState() == QString("away"))
		m_avact_away->setChecked( true );
	else if(m_engine->getAvailState() == QString("outtolunch"))
		m_avact_otl->setChecked( true );
	else
		m_avact_avail->setChecked( true );

	QMenu * avail = menuBar()->addMenu(tr("&Availability"));
	avail->addActions( m_availgrp->actions() );

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
	SwitchBoardConfDialog * conf = new SwitchBoardConfDialog(m_engine, m_widget, this);
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
}

/*!
 * disable the "Stop" action and enable "Start" Action.
 * \sa engineStarted()
 */
void MainWindow::engineStopped()
{
	m_stopact->setDisabled(true);
	m_startact->setEnabled(true);
}

/*! \brief Display the about box
 *
 * use QMessageBox::about() to display the application about box
 */
void MainWindow::about()
{
	QString applicationVersion("0.1");
	QMessageBox::about(this,
			   tr("About XIVO SwitchBoard"),
	                   tr("<h3>XIVO Switchboard</h3>"
			      "<p>This application displays the status of the"
			      " ongoing phone calls.</p>"
			      "<p>Version : %1</p>"
			      "<p>(C) 2007 <b>Proformatique</b> "
			      "<a href=\"http://www.proformatique.com\">"
			      "http://www.proformatique.com</a></p>"
			      "<p>67 rue Voltaire 92800 Puteaux FRANCE</p>"
			      "<p>E-mail : technique@proformatique.com</p>"
			      "<p>(+33/0)1.41.38.99.60</p>"
			      "<p>Author : Thomas Bernard</p>").arg(applicationVersion) );
}

