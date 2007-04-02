#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QMainWindow>

class QAction;
class SwitchBoardEngine;
class SwitchBoardWindow;

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(SwitchBoardEngine *);
private slots:
	void showConfDialog();
	void engineStopped();
	void engineStarted();
	void about();
private:
	SwitchBoardEngine * m_engine;
	SwitchBoardWindow * m_widget;
	QAction * m_startact;
	QAction * m_stopact;
};

#endif

