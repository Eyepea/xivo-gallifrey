#include <QStatusBar>
#include <QMenuBar>
#include <QApplication>
#include "mainwindow.h"
#include "switchboardengine.h"
#include "switchboardwindow.h"

MainWindow::MainWindow(SwitchBoardEngine * engine)
{
	SwitchBoardWindow * widget = new SwitchBoardWindow(this);
	engine->setWindow(widget);
	widget->setEngine(engine);
	setCentralWidget(widget);
	statusBar()->showMessage("test");

	QAction * quit = new QAction("&Quit", this);
	connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));
	menuBar()->addMenu("&File")->addAction(quit);
}

