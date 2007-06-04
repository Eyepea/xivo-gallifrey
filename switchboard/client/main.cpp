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

/* $Revision $
   $Date$
*/

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

