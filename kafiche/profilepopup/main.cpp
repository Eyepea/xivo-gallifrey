#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "mainwidget.h"
#include "engine.h"

/*! \mainpage Profile Popup
 *
 * \section intro_sec Introduction
 *
 * This Qt program is designed to run quietly in the background
 * waiting for incoming connections from the server.
 * When a profile is received, it is displayed.
 *
 * \sa main
 */

/*! \brief program entry point
 *
 * Set some static Qt parametters for using QSettings,
 * instanciate a MainWidget window and a Engine object.
 *
 * \sa MainWidget, Engine
 */
int main(int argc, char * * argv)
{
	QString locale = QLocale::system().name();
	QCoreApplication::setOrganizationName("Proformatique");
	QCoreApplication::setOrganizationDomain("proformatique.com");
	QCoreApplication::setApplicationName("Profile Popup");
	QApplication app(argc, argv);
	QTranslator translator;
	translator.load(QString("kafiche_") + locale);
	app.installTranslator(&translator);
	Engine engine;
	MainWidget main(&engine);
	main.show();
	//main.dumpObjectTree();
	return app.exec();
}

