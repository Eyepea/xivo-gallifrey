#include <QStatusBar>
#include <QMenuBar>
#include <QApplication>
#include <QDebug>
#include "mainwindow.h"
#include "switchboardengine.h"
#include "switchboardwindow.h"
#include "switchboardconf.h"

MainWindow::MainWindow(SwitchBoardEngine * engine)
: m_engine(engine)
{
	setWindowIcon(QIcon(":/xivoicon.png"));
	setWindowTitle("Xivo Switchboard");
	SwitchBoardWindow * widget = new SwitchBoardWindow(this);
	engine->setWindow(widget);
	widget->setEngine(engine);
	setCentralWidget(widget);
	//statusBar()->showMessage("test");
	connect(engine, SIGNAL(emitTextMessage(const QString &)),
	        statusBar(), SLOT(showMessage(const QString &)));

	QMenu * menu = menuBar()->addMenu(tr("&File"));
	
	QAction * start = new QAction(tr("S&tart"), this);
	connect(start, SIGNAL(triggered()), m_engine, SLOT(start()) );
	menu->addAction(start);

	QAction * stop = new QAction(tr("Sto&p"), this);
	connect(stop, SIGNAL(triggered()), m_engine, SLOT(stop()) );
	menu->addAction(stop);

	QAction * conf = new QAction(tr("&Configure"), this);
	connect(conf, SIGNAL(triggered()), this, SLOT(showConfDialog()));
	menu->addAction(conf);

	QAction * quit = new QAction(tr("&Quit"), this);
	connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));
	menu->addAction(quit);
}

void MainWindow::showConfDialog()
{
	SwitchBoardConfDialog * conf = new SwitchBoardConfDialog(m_engine, this);
	qDebug() << "<<  " << conf->exec();
}

