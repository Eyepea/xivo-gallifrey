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
#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QMainWindow>

class QAction;
class QActionGroup;
class QCloseEvent;
class QSplitter;
class LoginEngine;
class SwitchBoardEngine;
class SwitchBoardWindow;

/*! \brief Main window splitted to display peers and calls
 */
class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	//! Constructor
	MainWindow(SwitchBoardEngine *, LoginEngine *);
	//! Destructor
	virtual ~MainWindow();
private slots:
	void showConfDialog();
	void engineStopped();
	void engineStarted();
	void loginengineStarted();
	void about();
private:
	QSplitter * m_splitter;	//!< Splitter to separate right/left panels
	QSplitter * m_leftSplitter;	//!< Vertical splitter on the left
	QSplitter * m_middleSplitter;	//!< vertical splitter in the middle
	QSplitter * m_rightSplitter;	//!< Vertical splitter on the right
	SwitchBoardEngine * m_engine;	//!< Engine
	LoginEngine * m_loginengine;	//!< Login Engine
	SwitchBoardWindow * m_widget;	//!< Widget to display peers

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
};

#endif

