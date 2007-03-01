#include <QApplication>
#include "switchboardwindow.h"

int main(int argc, char * * argv)
{
	QApplication app(argc, argv);
	SwitchBoardWindow mainwin;
	mainwin.show();
	return app.exec();
}

