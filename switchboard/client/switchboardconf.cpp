#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include "switchboardconf.h"
#include "switchboardengine.h"

SwitchBoardConfDialog::SwitchBoardConfDialog(SwitchBoardEngine * engine,
                                             QWidget * parent)
: QDialog(parent), m_engine(engine)
{
	setModal( true );
	setWindowTitle( tr("Configuration") );

	QVBoxLayout * vlayout = new QVBoxLayout( this );

	QGridLayout * layout = new QGridLayout();
	QLabel * lblhost = new QLabel( tr("Server host :"), this);
	m_host = new QLineEdit(m_engine->host(), this);
	layout->addWidget( lblhost, 0, 0);
	layout->addWidget( m_host, 0, 1);
	QLabel * lblport = new QLabel( tr("Server port :"), this);
	m_port = new QLineEdit(QString::number(m_engine->port()), this);
	m_port->setInputMask("90000");
	layout->addWidget( lblport, 1, 0);
	layout->addWidget( m_port, 1, 1);
	
	vlayout->addLayout( layout );

	QDialogButtonBox * btnbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	connect( btnbox, SIGNAL(accepted()), this, SLOT(saveAndClose()) );
	connect( btnbox, SIGNAL(rejected()), this, SLOT(close()) );
	vlayout->addWidget(btnbox);
	
}

void SwitchBoardConfDialog::saveAndClose()
{
	m_engine->setAddress( m_host->text(), m_port->text().toUInt() );
	m_engine->saveSettings();
	close();
}

