/*
XIVO customer information client : popup profile for incoming calls
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

/*! \fn main
 *
 * \brief program entry point
 *
 * Set some static Qt parametters for using QSettings,
 * instanciate a MainWidget window and a Engine object.
 *
 * \sa MainWidget, Engine
 */
int main(int argc, char * * argv)
{
	QString locale = QLocale::system().name();
	QCoreApplication::setOrganizationName("XivoCTI");
	QCoreApplication::setOrganizationDomain("proformatique.com");
	QCoreApplication::setApplicationName("XivoClient");
	QApplication app(argc, argv);
	QTranslator translator;
	translator.load(QString("kafiche_") + locale);
	app.installTranslator(&translator);
	Engine engine;
	MainWidget main(&engine);
	main.show();
	//main.dumpObjectTree();
	QObject::connect( &app, SIGNAL(lastWindowClosed()),
	         &engine, SLOT(stop()) );
	return app.exec();
}

