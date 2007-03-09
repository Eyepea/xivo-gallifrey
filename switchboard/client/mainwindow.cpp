#include <QStatusBar>
#include <QMenuBar>
#include <QApplication>
#include "mainwindow.h"
#include "switchboardengine.h"
#include "switchboardwindow.h"

MainWindow::MainWindow(SwitchBoardEngine * engine)
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

	QAction * quit = new QAction("&Quit", this);
	connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));
	menuBar()->addMenu("&File")->addAction(quit);
}

