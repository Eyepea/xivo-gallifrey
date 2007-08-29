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
#ifndef __MAINWIDGET_H__
#define __MAINWIDGET_H__

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSystemTrayIcon>
#include <QMainWindow>
#include <QLineEdit>
#include "baseengine.h"
#include "dialpanel.h"
#include "logwidget.h"
#include "directorypanel.h"
#include "searchpanel.h"

class ConfWidget;
class IdentityDisplay;
class LogWidget;
class QActionGroup;
class QSystemTrayIcon;
class QTabWidget;

/*! \brief Main window class
 *
 * This widget contains buttons enabling the user
 * to popup the config window, to quit and to start
 * the login process.
 */
class MainWidget: public QMainWindow
{
	Q_OBJECT
public:
	//! Constructor
	MainWidget(BaseEngine *,
                   QWidget * parent=0);
	//! Destructor
        ~MainWidget();
	int tablimit() const;	//!< getter for m_tablimit
	void setTablimit(int);	//!< setter for m_tablimit
	void setForceTabs(bool);//!< setter for m_forcetabs
private slots:
	void popupConf();
	void setDisconnected();
	void setConnected();
	void systrayActivated(QSystemTrayIcon::ActivationReason);
	void systrayMsgClicked();
	void showNewProfile(Popup *);	//!< display a Profile widget
	void about();
	void affTextChanged();
protected:
	void hideEvent(QHideEvent *event);	//!< Catch hide events
	void closeEvent(QCloseEvent *event);
	//void changeEvent(QEvent *event);
private:
	void createActions();	//!< Create Actions (for menus)
	void createMenus();		//!< Create Menus
	void createSystrayIcon();	//!< Create the systray Icon and its menu
        void savePositions() const;
private:
	BaseEngine * m_engine;			//!< pointer to the BaseEngine used
	//QPushButton * m_btnstart;	//!< Start Button
	QSystemTrayIcon * m_systrayIcon;	//!< System Tray Icon
	//QIcon m_iconred;	//!< Icon object with red indicator
	//QIcon m_icongreen;	//!< Icon object with green indicator
	QIcon m_icon;		//!< Icon Object
	QIcon m_icongrey;	//!< greyed Icon Object

	QWidget * m_wid;	//!< Main widget
	QTabWidget * m_tabwidget;	//!< Area to display profiles
	QTabWidget * m_qtabwidget;	//!< Area to display function tabs
	QVBoxLayout * m_vboxwidgets;    //!< Panel of widgets
	QLabel * m_messagetosendlabel;    //!< Label for message for switchboards
	QLineEdit * m_messagetosend;    //!< Message to send to the connected switchboards
	DialPanel * m_dial;
	DirectoryPanel * m_directory;
	LogWidget * m_history;
        ConfWidget * m_conf;
        IdentityDisplay * m_infowidget;
	SearchPanel * m_peerswidget;
	QWidget * m_featureswidget;
	bool m_forcetabs;    //!< Flag to allow the display of "unallowed" tabs, useful to test server-side capabilities
	bool m_presence;

	int m_tablimit;		//!< Maximum number of tabs in m_tabwidget
	int m_cinfo_index;
	// actions :
	QAction * m_cfgact;		//!< Configuration Action
	QAction * m_quitact;	//!< Quit Action
	QAction * m_systrayact;	//!< "Go to systray" action
	QAction * m_connectact;	//!< "Connect" Action
	QAction * m_disconnectact;	//!< "Disconnect" Action
	QActionGroup * m_availgrp;	//!< group For Availability Actions
	QAction * m_avact_avail;	//!< Available Action
	QAction * m_avact_away;		//!< Away Action
	QAction * m_avact_brb;		//!< "Be Right Back" Action
	QAction * m_avact_otl;		//!< "Out To Lunch" Action
	QAction * m_avact_dnd;		//!< "Do not disturb" action
	QMenu * m_avail;

	QLabel * m_status;	//!< status indicator
};

#endif

