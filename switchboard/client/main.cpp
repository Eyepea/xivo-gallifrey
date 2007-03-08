#include <QApplication>
#include "switchboardwindow.h"
#include "switchboardengine.h"
#include "mainwindow.h"

int main(int argc, char * * argv)
{
	QApplication app(argc, argv);
	SwitchBoardEngine engine;
	MainWindow mainwin(&engine);
	mainwin.show();
	engine.setAddress("192.168.0.159", 8080);
	engine.startTimer(10000);		//connectSocket();
	return app.exec();
}

