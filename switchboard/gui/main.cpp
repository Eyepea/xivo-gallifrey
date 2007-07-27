/*
XIVO switchboard : 
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

/* $Id$
   $Date$
*/

#include <QApplication>
#include <QFile>
#include <QLocale>
#include <QTranslator>
#include "switchboardwindow.h"
#include "baseengine.h"
#include "mainwindow.h"

/*! \fn main
 *
 * \brief program entry point
 *
 * Set some static Qt parametters for using QSettings,
 * instanciate a MainWidget window and a BaseEngine object.
 *
 * \sa MainWidget, BaseEngine
 */
int main(int argc, char * * argv)
{
	QString locale = QLocale::system().name();
//	QApplication::setStyle(new SwitchboardStyle());
	QCoreApplication::setOrganizationName("Xivo");
	QCoreApplication::setOrganizationDomain("xivo.fr");
	QCoreApplication::setApplicationName("XivoSwitchBoard");
	QApplication app(argc, argv);

	QFile qssFile("xivo.qss");
	if(qssFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		app.setStyleSheet( QString(qssFile.readAll()) );
		qssFile.close();
	}

	QTranslator qtTranslator;
	qtTranslator.load(QString(":/switchboard_") + locale);
	app.installTranslator(&qtTranslator);

	BaseEngine engine;
        engine.setIsASwitchboard(true);

	MainWindow main(&engine);
	main.show();
	//main.dumpObjectTree();
	//engine.startTimer(1000);
	return app.exec();
}

