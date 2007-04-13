#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QMainWindow>

class QAction;
class QCloseEvent;
class QSplitter;
class SwitchBoardEngine;
class SwitchBoardWindow;

/*! \brief Main window splitted to display peers and calls
 */
class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	//! Constructor
	MainWindow(SwitchBoardEngine *);
	//! Destructor
	virtual ~MainWindow();
private slots:
	void showConfDialog();
	void engineStopped();
	void engineStarted();
	void about();
private:
	QSplitter * m_splitter;	//!< Splitter to separate right/left panels
	SwitchBoardEngine * m_engine;	//!< Engine
	SwitchBoardWindow * m_widget;	//!< Widget to display peers
	QAction * m_startact;	//!< "Start" Action
	QAction * m_stopact;	//!< "Stop" Action
};

#endif

