/* XIVO CTI Clients
 * Copyright (C) 2007-2009  Proformatique
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Linking the Licensed Program statically or dynamically with other
 * modules is making a combined work based on the Licensed Program. Thus,
 * the terms and conditions of the GNU General Public License version 2
 * cover the whole combination.
 *
 * In addition, as a special exception, the copyright holders of the
 * Licensed Program give you permission to combine the Licensed Program
 * with free software programs or libraries that are released under the
 * GNU Library General Public License version 2.0 or GNU Lesser General
 * Public License version 2.1 or any later version of the GNU Lesser
 * General Public License, and with code included in the standard release
 * of OpenSSL under a version of the OpenSSL license (with original SSLeay
 * license) which is identical to the one that was published in year 2003,
 * or modified versions of such code, with unchanged license. You may copy
 * and distribute such a system following the terms of the GNU GPL
 * version 2 for the Licensed Program and the licenses of the other code
 * concerned, provided that you include the source code of that other code
 * when and as the GNU GPL version 2 requires distribution of source code.
 */

/* $Revision$
 * $Date$
 */

#include <QApplication>
#include <QFile>
#include <QLocale>
#include <QSettings>
#include <QSplashScreen>
#include <QStyle>
#include <QSysInfo>
#include <QTranslator>

#include "baseengine.h"
#include "mainwidget.h"
#include "powerawareapplication.h"

/*! \fn main
 *
 * \brief program entry point
 *
 * Set some static Qt parameters for using QSettings,
 * instantiate a MainWidget window and a BaseEngine object.
 *
 * \sa MainWidget, BaseEngine
 */
int main(int argc, char ** argv)
{
    QString locale = QLocale::system().name();
    // QApplication::setStyle(new XIVOCTIStyle());
    QCoreApplication::setOrganizationName("XIVO");
    QCoreApplication::setOrganizationDomain("xivo.fr");
    QCoreApplication::setApplicationName("XIVO_Client");
    PowerAwareApplication app(argc, argv);
    QSettings * settings = new QSettings(QSettings::IniFormat,
                                         QSettings::UserScope,
                                         QCoreApplication::organizationName(),
                                         QCoreApplication::applicationName());
    qDebug() << "style" << app.style() << settings->fileName();
    if(argc > 1)
        settings->setValue("profile/default", argv[1]);
    QString qsskind = settings->value("display/qss", "none").toString();
    QFile qssFile(QString(":/common/%1.qss").arg(qsskind));
    QString qssStr;
    if(qssFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qssStr = qssFile.readAll();
        qssFile.close();
    }
    app.setStyleSheet(qssStr);
    app.setWindowIcon(QIcon(":/images/xivoicon.png"));
    
    QTranslator qtTranslator_xivo, qtTranslator_qt;
    QString forcelocale = settings->value("display/forcelocale", "").toString();
    if(forcelocale.size() > 0)
        locale = forcelocale;
    qtTranslator_xivo.load(QString(":/xivoclient_%1").arg(locale));
    qtTranslator_qt.load(QString(":/qt_%1").arg(locale));
    // QLibraryInfo::location(QLibraryInfo::TranslationsPath)
    app.installTranslator(&qtTranslator_xivo);
    app.installTranslator(&qtTranslator_qt);
    app.setQuitOnLastWindowClosed(false);
    
    BaseEngine * engine = new BaseEngine(settings);
    
    QString info_osname;
    QString info_endianness;
    if(QSysInfo::ByteOrder == 0)
        info_endianness = "BE";
    else
        info_endianness = "LE";
#if defined(Q_WS_X11)
    info_osname = QString("X11-%1-%2").arg(info_endianness).arg(app.applicationPid());
#elif defined(Q_WS_WIN)
    info_osname = QString("WIN-%1-0x%2-%3").arg(info_endianness).arg(QSysInfo::WindowsVersion, 2, 16, QChar('0')).arg(app.applicationPid());
#elif defined(Q_WS_MAC)
    info_osname = QString("MAC-%1-0x%2-%3").arg(info_endianness).arg(QSysInfo::MacintoshVersion, 2, 16, QChar('0')).arg(app.applicationPid());
#else
    info_osname = QString("unknown-%1-%2").arg(info_endianness).arg(app.applicationPid());
#endif
    qDebug() << "main() osname=" << info_osname << "locale=" << locale;
    
    MainWidget main(engine, info_osname);
    
    //main.dumpObjectTree();
    app.setProperty("stopper", "lastwindow");
    
    // setting this connection breeds the following behaviour :
    //  * exit of config window when systray-only => disconnects from server
    // there seemed to be a case when this was useful however ...
    //    we let this commented until a relevant use case is met again
    // QObject::connect( &app, SIGNAL(lastWindowClosed()),
    // engine, SLOT(stop()) );
    
    QObject::connect( &app, SIGNAL(standBy()),
                      engine, SLOT(stop()) );
    QObject::connect( &app, SIGNAL(resume()),
                      engine, SLOT(start()) );
    QObject::connect( &app, SIGNAL(powerEvent(int)),
                      engine, SLOT(powerEvent(int)) );
    
    //engine.startTimer(1000);
    return app.exec();
}
