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
#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include "switchboardconf.h"
#include "baseengine.h"
#include "switchboardwindow.h"
#include "loginengine.h"
#include "mainwindow.h"

/*! \brief constructor
 */
SwitchBoardConfDialog::SwitchBoardConfDialog(BaseEngine * engine,
                                             LoginEngine * loginengine,
                                             SwitchBoardWindow * window,
                                             MainWindow * parent)
: QDialog(parent), m_engine(engine), m_loginengine(loginengine), m_window(window)
{
	m_mainwindow = parent;
	int line = 0;
	setModal( true );
	// the object will be destroyed when closed
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle( tr("Configuration") );

	QVBoxLayout * vlayout = new QVBoxLayout( this );
	/* grid layout for the editable values */
	QGridLayout * gridlayout = new QGridLayout();

	QLabel * lblhost = new QLabel( tr("Server host :"), this);
	m_serverhost = new QLineEdit(m_engine->host(), this);
	gridlayout->addWidget( lblhost, line, 0 );
	gridlayout->addWidget( m_serverhost, line, 1 );
	line++;

	QLabel * lblport = new QLabel( tr("Server port :"), this);
	m_sbport = new QLineEdit(QString::number(m_engine->sbport()), this);
	m_sbport->setInputMask("5003");
	gridlayout->addWidget( lblport, line, 0 );
	gridlayout->addWidget( m_sbport, line, 1 );
	line++;
	
	QLabel * lbllport = new QLabel( tr("Login port :"), this);
	m_loginport = new QLineEdit(QString::number(m_loginengine->loginPort()), this);
	m_loginport->setInputMask("5000");
	gridlayout->addWidget( lbllport, line, 0 );
	gridlayout->addWidget( m_loginport, line, 1 );
	line++;
	
	m_presence = new QCheckBox( tr("Presence reporting"), this );
	m_presence->setCheckState( m_loginengine->enabled()?Qt::Checked:Qt::Unchecked );
	gridlayout->addWidget( m_presence, line, 0, 1, 0);
	line++;

#if 0
	QLabel * lblwidth = new QLabel( tr("Width :"), this );
	m_widthsb = new QSpinBox( this );
	m_widthsb->setRange( 1, 20 );
	m_widthsb->setValue( m_window->width() );
	gridlayout->addWidget( lblwidth, line, 0 );
	gridlayout->addWidget( m_widthsb, line, 1 );
	line++;
#endif

	m_autoconnect = new QCheckBox(tr("Autoconnect at startup"), this);
	m_autoconnect->setCheckState( m_engine->autoconnect()?Qt::Checked:Qt::Unchecked );
	gridlayout->addWidget(m_autoconnect, line, 0, 1, 0);
	line++;

	m_trytoreconnect = new QCheckBox(tr("Try to reconnect"), this);
	m_trytoreconnect->setCheckState( m_engine->trytoreconnect()?Qt::Checked:Qt::Unchecked );
	gridlayout->addWidget(m_trytoreconnect, line++, 0, 1, 0);
	gridlayout->addWidget( new QLabel( tr("Try to reconnect interval"), this), line, 0);
	m_tryinterval_sbox = new QSpinBox(this);
	m_tryinterval_sbox->setRange(1, 120);
	m_tryinterval_sbox->setValue(m_engine->trytoreconnectinterval() / 1000);
	gridlayout->addWidget( m_tryinterval_sbox, line++, 1);

	QLabel * lbltablimit = new QLabel( tr("Tab limit"), this);
	gridlayout->addWidget(lbltablimit, line, 0);
	m_tablimit_sbox = new QSpinBox(this);
	m_tablimit_sbox->setRange(0, 99);
	m_tablimit_sbox->setValue(m_mainwindow->tablimit());
	gridlayout->addWidget(m_tablimit_sbox, line++, 1);

	gridlayout->addWidget( new QLabel( tr("History size"), this), line, 0);
	m_history_sbox = new QSpinBox(this);
	m_history_sbox->setRange(1, 20);
	m_history_sbox->setValue(m_engine->historysize());
	gridlayout->addWidget( m_history_sbox, line++, 1);

	QLabel * lblasterisk = new QLabel( tr("Asterisk server :"), this);
	gridlayout->addWidget( lblasterisk, line, 0 );
	m_asterisk = new QLineEdit( m_engine->asterisk(), this );
	gridlayout->addWidget( m_asterisk, line++, 1 );

	QLabel * lblproto = new QLabel( tr("Protocol :"), this );
	gridlayout->addWidget( lblproto, line, 0 );
	m_protocombo = new QComboBox(this);
	m_protocombo->addItem("SIP");
	m_protocombo->addItem("IAX");
	m_protocombo->setCurrentIndex((m_engine->protocol()=="SIP")?0:1);
	gridlayout->addWidget( m_protocombo, line++, 1 );

	QLabel * lblid = new QLabel( tr("User id :"), this );
	gridlayout->addWidget(lblid, line, 0);
	m_userid = new QLineEdit( m_engine->userId(), this );
	gridlayout->addWidget( m_userid, line, 1 );
	line++;

	QLabel * lblpass = new QLabel( tr("Password :"), this );
	gridlayout->addWidget(lblpass, line, 0);
	m_passwd = new QLineEdit( m_loginengine->password(), this );
	m_passwd->setEchoMode(QLineEdit::Password);
	gridlayout->addWidget( m_passwd, line, 1);
	line++;
	
	vlayout->addLayout( gridlayout );

	QDialogButtonBox * btnbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	//	QDialogButtonBox * btnbox = new QDialogButtonBox(Qt::Horizontal, this);
	//	QPushButton * cbutton = btnbox->addButton(tr("Cancel"), QDialogButtonBox::RejectRole);
	//	QPushButton * obutton = btnbox->addButton(tr("OK"), QDialogButtonBox::AcceptRole);
	//	obutton->setDefault(true);
	connect( btnbox, SIGNAL(accepted()), this, SLOT(saveAndClose()) );
	connect( btnbox, SIGNAL(rejected()), this, SLOT(close()) );
	btnbox->button(QDialogButtonBox::Ok)->setDefault(true);
	vlayout->addWidget(btnbox);
	
}

void SwitchBoardConfDialog::saveAndClose()
{
	m_loginengine->setServerip( m_serverhost->text() );
	m_loginengine->setUserId( m_userid->text() );
	m_loginengine->setAutoconnect( m_autoconnect->checkState() == Qt::Checked );
	m_loginengine->setTrytoreconnect( m_trytoreconnect->checkState() == Qt::Checked );
	m_loginengine->setTrytoreconnectinterval( m_tryinterval_sbox->value()*1000 );
	m_loginengine->setServerAst( m_asterisk->text() );
	m_loginengine->setProtocol( m_protocombo->currentText() );

	m_loginengine->setEnabled( m_presence->checkState() == Qt::Checked );
	m_loginengine->setLoginPort( m_loginport->text().toUInt() );
	m_loginengine->setPassword( m_passwd->text() );
	//m_window->setWidth( m_widthsb->value() );
	//m_window->saveSettings();

	m_engine->setAddress( m_serverhost->text(), m_sbport->text().toUInt() );
	m_engine->setUserId( m_userid->text() );
	m_engine->setAutoconnect( m_autoconnect->checkState() == Qt::Checked );
	m_engine->setTrytoreconnect( m_trytoreconnect->checkState() == Qt::Checked );
	m_engine->setTrytoreconnectinterval( m_tryinterval_sbox->value()*1000 );
	m_engine->setHistorySize( m_history_sbox->value() );
	m_engine->setAsterisk( m_asterisk->text() );
	m_engine->setProtocol( m_protocombo->currentText() );

	m_loginengine->saveSettings();
	m_engine->saveSettings();
	m_mainwindow->setTablimit(m_tablimit_sbox->value());
	close();
}

// 	//	settings.setValue("engine/serverhost", m_serverhost);
// 	//
// 	//	settings.setValue("engine/userid", m_userid);
// 	//	settings.setValue("engine/autoconnect", m_autoconnect);
// 	//	settings.setValue("engine/trytoreconnect", m_trytoreconnect);
// 	//	settings.setValue("engine/trytoreconnectinterval", m_trytoreconnectinterval);
// 	//	settings.setValue("engine/asterisk", m_asterisk);
// 	//	settings.setValue("engine/protocol", m_protocol);
	
// 	settings.setValue("engine/enabled", m_enabled);
// 	settings.setValue("engine/loginport", m_loginport);
// 	settings.setValue("engine/passwd", m_passwd);
// 	settings.setValue("engine/availstate", m_availstate);
// 	settings.setValue("engine/keepaliveinterval", m_keepaliveinterval);
