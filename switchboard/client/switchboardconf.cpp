/*
XIVO switchboard : 
Copyright (C) 2007  Proformatique

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/

/* $Id$
 * $Revision$
   $Date$
*/

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
#include "loginengine.h"

SwitchBoardConfDialog::SwitchBoardConfDialog(SwitchBoardEngine * engine,
					     LoginEngine * loginengine,
                                             SwitchBoardWindow * window,
                                             QWidget * parent)
: QDialog(parent), m_engine(engine), m_loginengine(loginengine), m_window(window)
{
	int line = 0;
	setModal( true );
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle( tr("Configuration") );

	QVBoxLayout * vlayout = new QVBoxLayout( this );

	QGridLayout * layout = new QGridLayout();
	QLabel * lblhost = new QLabel( tr("Server host :"), this);
	m_serverhost = new QLineEdit(m_engine->host(), this);
	layout->addWidget( lblhost, line, 0 );
	layout->addWidget( m_serverhost, line, 1 );
	line++;

	QLabel * lblport = new QLabel( tr("Server port :"), this);
	m_sbport = new QLineEdit(QString::number(m_engine->sbport()), this);
	m_sbport->setInputMask("5003");
	layout->addWidget( lblport, line, 0 );
	layout->addWidget( m_sbport, line, 1 );
	line++;
	
	QLabel * lbllport = new QLabel( tr("Login port :"), this);
	m_loginport = new QLineEdit(QString::number(m_loginengine->loginPort()), this);
	m_loginport->setInputMask("5000");
	layout->addWidget( lbllport, line, 0 );
	layout->addWidget( m_loginport, line, 1 );
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
	layout->addWidget( m_ext, line, 1 );
	line++;

	QLabel * lblpass = new QLabel( tr("Password :"), this );
	layout->addWidget(lblpass, line, 0);
	m_passwd = new QLineEdit( m_loginengine->password(), this );
	m_passwd->setEchoMode(QLineEdit::Password);
	layout->addWidget( m_passwd, line, 1);
	line++;
	
	vlayout->addLayout( layout );

	QDialogButtonBox * btnbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	connect( btnbox, SIGNAL(accepted()), this, SLOT(saveAndClose()) );
	connect( btnbox, SIGNAL(rejected()), this, SLOT(close()) );
	vlayout->addWidget(btnbox);
	
}

void SwitchBoardConfDialog::saveAndClose()
{
	m_engine->setAddress( m_serverhost->text(), m_sbport->text().toUInt() );
	m_engine->setAutoconnect( m_autoconnect->checkState() == Qt::Checked );
	m_engine->setAsterisk( m_asterisk->text() );
	m_engine->setProtocol( m_protocombo->currentText() );
	m_engine->setExtension( m_ext->text() );
	m_loginengine->setLoginPort( m_loginport->text().toUInt() );
	m_loginengine->setPassword( m_passwd->text() );
	m_loginengine->saveSettings();
	m_engine->saveSettings();
	//m_window->setWidth( m_widthsb->value() );
	//m_window->saveSettings();
	close();
}

