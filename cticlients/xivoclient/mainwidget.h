/* XIVO client
 * Copyright (C) 2007, 2008  Proformatique
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

#ifndef __MAINWIDGET_H__
#define __MAINWIDGET_H__

#include <QCheckBox>
#include <QDateTime>
#include <QHash>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QSettings>
#include <QSplitter>
#include <QSystemTrayIcon>
#include <QVBoxLayout>
#include <QWidget>

#include "baseengine.h"
#include "directorypanel.h"

class QAction;
class QActionGroup;
class QClipboard;
class QCloseEvent;
class QDateTime;
class QDockWidget;
class QEvent;
class QKeyEvent;
class QLabel;
class QScrollArea;
class QSystemTrayIcon;
class QTabWidget;
class QVBoxLayout;
class QWidget;
class QUrl;

class BaseEngine;
class CallStackWidget;
class ConfigWidget;
class DirectoryPanel;
class LeftPanel;
class SwitchBoardWindow;
#ifdef USE_OUTLOOK
class OutlookPanel;
#endif

/*! \brief Main window class
 */
class MainWidget : public QMainWindow
{
	Q_OBJECT
public:
	//! Constructor
	MainWidget(BaseEngine *,
                   const QString &,
                   QWidget * parent=0);
	//! Destructor
        virtual ~MainWidget();

	void setForceTabs(bool);//!< setter for m_forcetabs
	void setAppearance(const QStringList &);	//!< dock options
	void clearAppearance();
public slots:
        void dispurl(const QUrl &url);
        void customerInfoPopup(const QString &,
                               const QHash<QString, QString> &,
                               const QString &);
private slots:
        void clipselection();
        void clipdata();
	void showConfDialog();
	void showLogin();
	void hideLogin();
	void engineStopped();
	void engineStarted();
        void systrayActivated(QSystemTrayIcon::ActivationReason);
        void systrayMsgClicked();
        void checksAvailState();
	void about();
        void newParkEvent();
	void affTextChanged();
        void config_and_start();
        void logintextChanged(const QString &);
        void loginKindChanged(int);
        void confUpdated();
        void updatePresence(const QMap<QString, QVariant> &);
signals:
        void functionKeyPressed(int);
        void pasteToDialPanel(const QString &);
protected:
	void showEvent(QShowEvent *);	//!< Catch show events
	void hideEvent(QHideEvent *);	//!< Catch hide events
	void closeEvent(QCloseEvent *);
	void changeEvent(QEvent *);
	void keyPressEvent(QKeyEvent *);
        void addPanel(const QString &, const QString &, QWidget *);
        void removePanel(const QString &, QWidget *);
        void connectDials(QWidget *);
        // bool event(QEvent *);
private:
	void createActions();	//!< Create Actions (for menus)
	void createMenus();		//!< Create Menus
	void createSystrayIcon();	//!< Create the systray Icon and its menu
        void savePositions() const;
        void updateAppliName();
        void clearPresence();

	BaseEngine * m_engine;	//!< Engine
	QSystemTrayIcon * m_systrayIcon;	//!< System Tray Icon
	QIcon m_icon;		//!< Icon Object
	QIcon m_icongrey;	//!< greyed Icon Object
	QWidget * m_wid;	//!< Main widget

        // Widgets for Xlets
	QTabWidget * m_tabwidget;	//!< Area to display messages, services and histories
        LeftPanel * m_leftpanel;
        QScrollArea * m_areaCalls;
	CallStackWidget * m_calls;
	DirectoryPanel * m_dirpanel;
#ifdef USE_OUTLOOK
        OutlookPanel * m_outlook;
#endif
        QHash<QString, QWidget *> m_xlet;

        QLabel * m_xivobg;

        ConfigWidget * m_config;

	bool m_forcetabs;    //!< Flag to allow the display of "unallowed" tabs, useful to test server-side capabilities
	bool m_presence;
	int m_cinfo_index;

        QString m_appliname;
        QHash<QString, QString> m_dockoptions;
        QStringList m_docknames;
        QStringList m_gridnames;
        QStringList m_tabnames;
        QStringList m_allnames;

        bool m_withsystray;
        bool m_loginfirst;

        QHash<QString, QDockWidget *> m_docks;

	// actions :
	QAction * m_cfgact;		//!< Configuration Action
	QAction * m_quitact;		//!< Quit Action
	QAction * m_connectact;		//!< "Connect" Action
	QAction * m_disconnectact;	//!< "Disconnect" Action
	QAction * m_systraymin;		//!< "Go to systray" action
	QAction * m_systraymax;		//!< "Go to systray" action
	QActionGroup * m_availgrp;	//!< Availability action group

	QGridLayout * m_gridlayout;
        QLabel * m_lab1;
        QLabel * m_lab2;
        QLabel * m_lab3;
        QLineEdit * m_qlab1;
        QLineEdit * m_qlab2;
        QLineEdit * m_qlab3;
        QPushButton * m_ack;
        QCheckBox * m_kpass;
        QCheckBox * m_loginkind;

	QMenu * m_avail;		//!< Availability submenu
	QHash<QString, QAction *> m_avact;	//!< Actions
	QLabel * m_status;	//!< status indicator

        QMenu * m_filemenu;
        QMenu * m_helpmenu;

        QDateTime m_launchDateTime;
        QSettings * m_settings;

        QClipboard * m_clipboard;
};


/*! \brief Widget containing the CallStackWidget and a Title QLabel
 */
class LeftPanel : public QWidget
{
public:
	LeftPanel(QWidget *, QWidget * parent = 0);	//!< Constructor
	QLabel * titleLabel();	//!< getter for m_titleLabel
private:
	QLabel * m_titleLabel;	//!< Title label property
};


#endif
