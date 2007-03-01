#include <QGridLayout>
#include <QPushButton>
#include "switchboardwindow.h"
#include "peerwidget.h"

SwitchBoardWindow::SwitchBoardWindow(QWidget * parent)
: QWidget(parent)
{
	QGridLayout * layout = new QGridLayout(this);
	PeerWidget * peer1 = new PeerWidget("Thomas", this);
	layout->addWidget( peer1, 0, 0 );
	layout->addWidget( new PeerWidget("Sylvain", this), 1, 0 );
	layout->addWidget( new PeerWidget("Corentin", this), 0, 1 );
	layout->addWidget( new PeerWidget("Adrien", this), 1, 1 );
	QPushButton * btnred = new QPushButton("&red", this);
	layout->addWidget( btnred, 0, 2 );
	connect( btnred, SIGNAL(clicked()), peer1, SLOT(setRed()) );
	QPushButton * btngreen = new QPushButton("&green", this);
	layout->addWidget( btngreen, 1, 2 );
	connect( btngreen, SIGNAL(clicked()), peer1, SLOT(setGreen()) );
	QPushButton * btnorange = new QPushButton("&orange", this);
	layout->addWidget( btnorange, 2, 2 );
	connect( btnorange, SIGNAL(clicked()), peer1, SLOT(setOrange()) );
}

