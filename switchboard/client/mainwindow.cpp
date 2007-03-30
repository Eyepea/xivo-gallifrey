#include <QStatusBar>
#include <QMenuBar>
#include <QApplication>
#include <QMessageBox>
#include <QSplitter>
#include <QScrollArea>
#include <QLabel>
#include <QDebug>
#include "mainwindow.h"
#include "switchboardengine.h"
#include "switchboardwindow.h"
#include "switchboardconf.h"
#include "callstackwidget.h"

MainWindow::MainWindow(SwitchBoardEngine * engine)
: m_engine(engine)
{
// va falloir réorganiser les communications entre le "Engine"
// et les objets d'affichage.
	setWindowIcon(QIcon(":/xivoicon.png"));
	setWindowTitle("Xivo Switchboard");

	QSplitter * splitter = new QSplitter(this);

	QScrollArea * areaCalls = new QScrollArea(splitter);
	CallStackWidget * calls = new CallStackWidget(areaCalls);
	connect( m_engine, SIGNAL(updateCall(const QString &, const QString &, const QString &, const QString &)), calls, SLOT(addCall(const QString &, const QString &, const QString &, const QString &)) );
	// test
	calls->addCall("1234", "Mr Test", "SIP/123-ABCD", "test");
	//calls->addCall("1235", "Mme Test", "SIP/123-ABCD2");
	//calls->addCall("1236", "M gloups", "SIP/123-ABCD3");
	//calls->addCall("1237", "Dr Test", "SIP/123-ABCD4");
	//calls->addCall("1238", "zozo grop", "SIP/123-ABCD5");
	//calls->addCall("1239", "palop", "SIP/123-ABCD6");
	//areaCalls->setWidget(calls);

	QScrollArea * areaPeers = new QScrollArea(splitter);
	areaPeers->setWidgetResizable(true);
	m_peerswidget = new SwitchBoardWindow(areaPeers);
	engine->setWindow(m_peerswidget);
	m_peerswidget->setEngine(engine);
	areaPeers->setWidget(m_peerswidget);
	qDebug() << areaCalls->widget();
	qDebug() << areaPeers->widget();

	//QLabel * test2 = new QLabel("Queues", splitter);

	//splitter->addWidget(areaCalls);
	//splitter->addWidget(calls);
	//splitter->addWidget(areaPeers);
	//splitter->addWidget(test2);

	setCentralWidget(splitter);
	//statusBar()->showMessage("test");
	connect(m_engine, SIGNAL(emitTextMessage(const QString &)),
	        statusBar(), SLOT(showMessage(const QString &)));
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

	QMenu * helpmenu = menuBar()->addMenu(tr("&Help"));

	helpmenu->addAction(tr("&About"), this, SLOT(about()));
	helpmenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
}

void MainWindow::showConfDialog()
{
	SwitchBoardConfDialog * conf = new SwitchBoardConfDialog(m_engine, m_peerswidget, this);
	//qDebug() << "<<  " << conf->exec();
}

void MainWindow::engineStarted()
{
	m_stopact->setEnabled(true);
	m_startact->setDisabled(true);
}

void MainWindow::engineStopped()
{
	m_stopact->setDisabled(true);
	m_startact->setEnabled(true);
}

void MainWindow::about()
{
	QString applicationVersion("0.1");
	QMessageBox::about(this, tr("About XIVO SwitchBoard"),
	                   tr("<h3>About</h3>"
					      "<p>To be continued</p>") );
}

