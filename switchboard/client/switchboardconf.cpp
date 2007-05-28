#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include "switchboardconf.h"
#include "switchboardengine.h"
#include "switchboardwindow.h"

SwitchBoardConfDialog::SwitchBoardConfDialog(SwitchBoardEngine * engine,
                                             SwitchBoardWindow * window,
                                             QWidget * parent)
: QDialog(parent), m_engine(engine), m_window(window)
{
	int line = 0;
	setModal( true );
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle( tr("Configuration") );

	QVBoxLayout * vlayout = new QVBoxLayout( this );

	QGridLayout * layout = new QGridLayout();
	QLabel * lblhost = new QLabel( tr("Server host :"), this);
	m_host = new QLineEdit(m_engine->host(), this);
	layout->addWidget( lblhost, line, 0 );
	layout->addWidget( m_host, line, 1 );
	line++;

	QLabel * lblport = new QLabel( tr("Server port :"), this);
	m_port = new QLineEdit(QString::number(m_engine->port()), this);
	m_port->setInputMask("90000");
	layout->addWidget( lblport, line, 0 );
	layout->addWidget( m_port, line, 1 );
	line++;
	
#if 0
	QLabel * lblwidth = new QLabel( tr("Width :"), this );
	m_widthsb = new QSpinBox( this );
	m_widthsb->setRange( 1, 20 );
	m_widthsb->setValue( m_window->width() );
	layout->addWidget( lblwidth, line, 0 );
	layout->addWidget( m_widthsb, line, 1 );
	line++;
#endif

	m_autoconnect = new QCheckBox(tr("Autoconnect at startup"), this);
	m_autoconnect->setCheckState( m_engine->autoconnect()?Qt::Checked:Qt::Unchecked );
	layout->addWidget(m_autoconnect, line, 0, 1, 0);
	line++;

	QLabel * lblasterisk = new QLabel( tr("Asterisk server :"), this);
	layout->addWidget( lblasterisk, line, 0 );
	m_asterisk = new QLineEdit( m_engine->asterisk(), this );
	layout->addWidget( m_asterisk, line, 1 );
	line++;

	QLabel * lblproto = new QLabel( tr("Protocol :"), this );
	layout->addWidget( lblproto, line, 0 );
	m_protocombo = new QComboBox(this);
	m_protocombo->addItem("SIP");
	m_protocombo->addItem("IAX");
	m_protocombo->setCurrentIndex((m_engine->protocol()=="SIP")?0:1);
	layout->addWidget( m_protocombo, line, 1 );
	line++;

	QLabel * lblext = new QLabel( tr("Extension :"), this );
	layout->addWidget(lblext, line, 0);
	m_ext = new QLineEdit( m_engine->extension(), this );
//	m_ext->setInputMask("00000");
	layout->addWidget( m_ext, line, 1 );
	line++;

	QLabel * lblcontext = new QLabel( tr("Dial context :"), this );
	layout->addWidget(lblcontext, line, 0);
	m_context = new QLineEdit( m_engine->dialContext(), this );
	layout->addWidget( m_context, line, 1);
	line++;

	QLabel * lblpass = new QLabel( tr("Password :"), this );
	layout->addWidget(lblpass, line, 0);
	m_pass = new QLineEdit( m_engine->password(), this );
	m_pass->setEchoMode(QLineEdit::Password);
	layout->addWidget( m_pass, line, 1);

	vlayout->addLayout( layout );

	QDialogButtonBox * btnbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	connect( btnbox, SIGNAL(accepted()), this, SLOT(saveAndClose()) );
	connect( btnbox, SIGNAL(rejected()), this, SLOT(close()) );
	vlayout->addWidget(btnbox);
	
}

void SwitchBoardConfDialog::saveAndClose()
{
	m_engine->setAddress( m_host->text(), m_port->text().toUInt() );
	m_engine->setAutoconnect( m_autoconnect->checkState() == Qt::Checked );
	m_engine->setAsterisk( m_asterisk->text() );
	m_engine->setProtocol( m_protocombo->currentText() );
	m_engine->setExtension( m_ext->text() );
	m_engine->setDialContext( m_context->text() );
	m_engine->setPassword( m_pass->text() );
	m_engine->saveSettings();
	//m_window->setWidth( m_widthsb->value() );
	//m_window->saveSettings();
	close();
}

