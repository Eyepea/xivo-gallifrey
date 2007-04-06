#include <QStatusBar>
#include <QMenuBar>
#include <QApplication>
#include <QMessageBox>
#include <QSplitter>
#include <QScrollArea>
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

class RightPanel : public QWidget
{
public:
	RightPanel(QWidget *, QWidget * parent = 0);
	QLabel * titleLabel();
private:
	QLabel * m_titleLabel;
};

RightPanel::RightPanel(QWidget * bottomWidget,QWidget * parent)
: QWidget(parent)
{
	QVBoxLayout * layout = new QVBoxLayout(this);
	layout->setMargin(0);
	m_titleLabel = new QLabel("test", this);
	layout->addWidget(m_titleLabel, 0, Qt::AlignCenter);
	layout->addWidget(bottomWidget, 1);
}

QLabel * RightPanel::titleLabel()
{
	return m_titleLabel;
}

MainWindow::MainWindow(SwitchBoardEngine * engine)
: m_engine(engine)
{
// va falloir réorganiser les communications entre le "Engine"
// et les objets d'affichage.
	setWindowIcon(QIcon(":/xivoicon.png"));
	setWindowTitle("Xivo Switchboard");

	m_splitter = new QSplitter(this);
	//QScrollArea * areaCalls = new QScrollArea(m_splitter);
	QScrollArea * areaCalls = new QScrollArea(this);
	RightPanel * rightPanel = new RightPanel(areaCalls, m_splitter);

	CallStackWidget * calls = new CallStackWidget(areaCalls);
	connect( calls, SIGNAL(changeTitle(const QString &)),
	         rightPanel->titleLabel(), SLOT(setText(const QString &)) );
	connect( m_engine, SIGNAL(updateCall(const QString &, const QString &, const int &, const QString &,
					     const QString &, const QString &, const QString &)),
		 calls, SLOT(addCall(const QString &, const QString &, const int &, const QString &,
				     const QString &, const QString &, const QString &)) );
	connect( m_engine, SIGNAL(callsUpdated()),
	         calls, SLOT(updateDisplay()) );
	connect( m_engine, SIGNAL(updateTime()),
		 calls, SLOT(updateTime()) );
	connect( calls, SIGNAL(hangUp(const QString &)),
		 m_engine, SLOT(hangUp(const QString &)) );

	connect( calls, SIGNAL(selectForMonitoring(const QString &)),
	         m_engine, SLOT(selectAsMonitored(const QString &)) );

	QScrollArea * areaPeers = new QScrollArea(m_splitter);
	areaCalls->setWidgetResizable(true);
	areaPeers->setWidgetResizable(true);

 	m_widget = new SwitchBoardWindow(areaPeers);
 	engine->setWindow(m_widget);
 	m_widget->setEngine(engine);
 	areaPeers->setWidget(m_widget);
 	areaCalls->setWidget(calls);
	setCentralWidget(m_splitter);

	// restore splitter settings
	QSettings settings;
	m_splitter->restoreState(settings.value("display/splitterSizes").toByteArray());

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

void MainWindow::closeEvent(QCloseEvent * event)
{
	qDebug() << "MainWindow::closeEvent()" << event << event->type();
	QSettings settings;
	settings.setValue("display/splitterSizes", m_splitter->saveState());
}

void MainWindow::showConfDialog()
{
	SwitchBoardConfDialog * conf = new SwitchBoardConfDialog(m_engine, m_widget, this);
	qDebug() << "<<  " << conf->exec();
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

