#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include "switchboardwindow.h"
#include "switchboardengine.h"
#include "mainwindow.h"

int main(int argc, char * * argv)
{
	QCoreApplication::setOrganizationName("Proformatique");
	QCoreApplication::setOrganizationDomain("proformatique.com");
	QCoreApplication::setApplicationName("SwitchBoard");
	QApplication app(argc, argv);

	QTranslator qtTranslator;
	qtTranslator.load("qt_" + QLocale::system().name());
	app.installTranslator(&qtTranslator);

	QTranslator switchboardTranslator;
	switchboardTranslator.load("switchboard_" + QLocale::system().name());
	app.installTranslator(&switchboardTranslator);

	SwitchBoardEngine engine;
	MainWindow mainwin(&engine);
	mainwin.show();
	engine.startTimer(1000);
	return app.exec();
}

