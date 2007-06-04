/* (c) 2007 Proformatique */
#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QFile>
#include "switchboardwindow.h"
#include "loginengine.h"
#include "switchboardengine.h"
#include "mainwindow.h"

int main(int argc, char * * argv)
{
//	QApplication::setStyle(new SwitchboardStyle());
	QCoreApplication::setOrganizationName("XivoCTI");
	QCoreApplication::setOrganizationDomain("proformatique.com");
	QCoreApplication::setApplicationName("XivoSwitchBoard");
	QApplication app(argc, argv);

	QFile qssFile("xivo.qss");
	if(qssFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		app.setStyleSheet( QString(qssFile.readAll()) );
		qssFile.close();
	}

	QTranslator qtTranslator;
	qtTranslator.load("qt_" + QLocale::system().name());
	app.installTranslator(&qtTranslator);

	QTranslator switchboardTranslator;
	switchboardTranslator.load(":/switchboard_" + QLocale::system().name());
	app.installTranslator(&switchboardTranslator);

	SwitchBoardEngine engine;
	LoginEngine login_engine;
	MainWindow mainwin(&engine, &login_engine);
	mainwin.show();
	//engine.startTimer(1000);
	return app.exec();
}

