#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QMainWindow>

class SwitchBoardEngine;

class MainWindow : public QMainWindow
{
//	Q_OBJECT
public:
	MainWindow(SwitchBoardEngine *);
};

#endif

