/* XIVO switchboard
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

/* $Id$ */
#ifndef __MAINWIDGET_H__
#define __MAINWIDGET_H__

#include <QWidget>
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QTabWidget>
#include "servicepanel.h"
#include "logwidget.h"
#include "callstackwidget.h"
#include "popup.h"

class QAction;
class QActionGroup;
class QCloseEvent;
class QLabel;
class QSplitter;

class BaseEngine;
class LogWidget;
class ServicePanel;
class SwitchBoardWindow;

/*! \brief Main window splitted to display peers and calls
 */
class MainWidget : public QMainWindow
{
	Q_OBJECT
public:
	//! Constructor
	MainWidget(BaseEngine *, QWidget * parent=0);
	int tablimit() const;	//!< getter for m_tablimit
	void setTablimit(int);	//!< setter for m_tablimit
	//! Destructor
	virtual ~MainWidget();
private slots:
	void showConfDialog();
	void engineStopped();
	void engineStarted();
	void loginengineStarted();
	//	void systrayActivated(QSystemTrayIcon::ActivationReason);
	//	void systrayMsgClicked();
	void showNewProfile(Popup *);	//!< display a Profile widget
	void about();
private:
        void createActions();

	QSplitter * m_splitter;	//!< Splitter to separate right/left panels
	QSplitter * m_leftSplitter;	//!< Vertical splitter on the left
	QSplitter * m_middleSplitter;	//!< vertical splitter in the middle
	QSplitter * m_rightSplitter;	//!< Vertical splitter on the right
	BaseEngine * m_engine;	//!< Engine
	SwitchBoardWindow * m_widget;	//!< Widget to display peers
	QTabWidget * m_tabwidget;	//!< Area to display profiles
	QTabWidget * m_svc_tabwidget;	//!< Area to display messages, services and histories
	ServicePanel * m_featureswidget;
	LogWidget * m_logwidget;
	CallStackWidget * calls;

	int m_tablimit;		//!< Maximum number of tabs in m_tabwidget

	QMenu * m_avail;	//!< Availability submenu
	QAction * m_loginact;	//!< "Log in" Action
	QAction * m_logoffact;	//!< "Log off" Action
	QAction * m_startact;	//!< "Start" Action
	QAction * m_stopact;	//!< "Stop" Action
	QActionGroup * m_availgrp;	//!< Availability action group
	QAction * m_avact_avail;	//!< "Available" action
	QAction * m_avact_brb;		//!< "Be right back" action
	QAction * m_avact_dnd;		//!< "Do not disturb" action
	QAction * m_avact_otl;		//!< "out to lunch" action
	QAction * m_avact_away;		//!< "away" action

	QLabel * m_status;	//!< status indicator
};

#endif

