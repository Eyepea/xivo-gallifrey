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
	void loginengineStopped();
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

	QAction * m_loginact;	//!< "Log in" Action
	QAction * m_logoffact;	//!< "Log off" Action
	QAction * m_startact;	//!< "Start" Action
	QAction * m_stopact;	//!< "Stop" Action
	QActionGroup * m_availgrp;
	QAction * m_avact_avail;
	QAction * m_avact_brb;
	QAction * m_avact_dnd;
	QAction * m_avact_otl;
	QAction * m_avact_away;
};

#endif

