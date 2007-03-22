#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include "switchboardconf.h"

SwitchBoardConfDialog::SwitchBoardConfDialog(QWidget * parent)
: QDialog(parent)
{
	setModal( true );
	setWindowTitle( tr("Configuration") );

	QVBoxLayout * vlayout = new QVBoxLayout( this );

	QGridLayout * layout = new QGridLayout();
	QLabel * lblhost = new QLabel( tr("Server host :"), this);
	QLineEdit * host = new QLineEdit(this);
	layout->addWidget( lblhost, 0, 0);
	layout->addWidget( host, 0, 1);
	QLabel * lblport = new QLabel( tr("Server port :"), this);
	QLineEdit * port = new QLineEdit(this);
	layout->addWidget( lblport, 1, 0);
	layout->addWidget( port, 1, 1);
	
	vlayout->addLayout( layout );

	QHBoxLayout * hlayout = new QHBoxLayout();
	QPushButton * btncancel = new QPushButton( tr("&Cancel"), this );
	QPushButton * btnok = new QPushButton( tr("&Ok"), this );
	hlayout->addWidget( btncancel );
	hlayout->addWidget( btnok );
	
	vlayout->addLayout( hlayout );
}

