/*
kafiche client : popup profile for incoming calls
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
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include "confwidget.h"
#include "mainwidget.h"

ConfWidget::ConfWidget(Engine *engine, MainWidget *parent)
: QDialog(parent), m_engine(engine)
{
	m_mainwidget = parent;
	// the object will be destroyed when closed
	setAttribute(Qt::WA_DeleteOnClose);
	setModal(true);
	QVBoxLayout *vlayout = new QVBoxLayout(this);

	setWindowTitle("Configuration");

	/* grid layout for the editable values */
	QGridLayout *gridlayout = new QGridLayout();

	QLabel *lblip = new QLabel(tr("Server Host"), this);
	m_lineip = new QLineEdit(m_engine->serverip(), this);

	QLabel *lblport = new QLabel(tr("Server Port"), this);
	m_lineport = new QLineEdit(QString::number(m_engine->serverport()), this);
	m_lineport->setInputMask("90000");

	QLabel *lblast = new QLabel(tr("Asterisk Id name"), this);
	m_lineast = new QLineEdit(m_engine->serverast(), this);

	QLabel *lblproto = new QLabel(tr("Protocol"), this);
	m_protocombo = new QComboBox(this);
	m_protocombo->addItem(QString("SIP"));
	m_protocombo->addItem(QString("IAX"));
	if( m_engine->login().left(3) == QString("sip") )
		m_protocombo->setCurrentIndex(0);
	else
		m_protocombo->setCurrentIndex(1);

	QLabel *lbllogin = new QLabel(tr("Login"), this);
	m_linelogin = new QLineEdit(m_engine->login().mid(3), this);

	QLabel *lblpasswd = new QLabel(tr("Password"), this);
	m_linepasswd = new QLineEdit(m_engine->passwd(), this);
	m_linepasswd->setEchoMode(QLineEdit::Password);

	gridlayout->addWidget(lblip, 0, 0);
	gridlayout->addWidget(m_lineip, 0, 1);
	gridlayout->addWidget(lblport, 1, 0);
	gridlayout->addWidget(m_lineport, 1, 1);
	gridlayout->addWidget(lblast, 2, 0);
	gridlayout->addWidget(m_lineast, 2, 1);
	gridlayout->addWidget(lblproto, 3, 0);
	gridlayout->addWidget(m_protocombo, 3, 1);
	gridlayout->addWidget(lbllogin, 4, 0);
	gridlayout->addWidget(m_linelogin, 4, 1);
	gridlayout->addWidget(lblpasswd, 5, 0);
	gridlayout->addWidget(m_linepasswd, 5, 1);

	m_autoconnect = new QCheckBox(tr("Autoconnect at startup"), this);
	m_autoconnect->setCheckState( m_engine->autoconnect()?Qt::Checked:Qt::Unchecked );
	gridlayout->addWidget(m_autoconnect, 6, 0, 1, 0);
	gridlayout->addWidget( new QLabel( tr("Keep alive interval"), this), 7, 0);
	m_kainterval_sbox = new QSpinBox(this);
	m_kainterval_sbox->setRange(1, 120);
	m_kainterval_sbox->setValue(m_engine->keepaliveinterval() / 1000);
	gridlayout->addWidget( m_kainterval_sbox, 7, 1);
	m_trytoreconnect = new QCheckBox(tr("Try to reconnect"), this);
	m_trytoreconnect->setCheckState( m_engine->trytoreconnect()?Qt::Checked:Qt::Unchecked );
	gridlayout->addWidget(m_trytoreconnect, 8, 0, 1, 0);
	gridlayout->addWidget( new QLabel( tr("Try to reconnect interval"), this), 9, 0);
	m_tryinterval_sbox = new QSpinBox(this);
	m_tryinterval_sbox->setRange(1, 120);
	m_tryinterval_sbox->setValue(m_engine->trytoreconnectinterval() / 1000);
	gridlayout->addWidget( m_tryinterval_sbox, 9, 1);

	QLabel * lbltablimit = new QLabel( tr("Tab limit"), this);
	gridlayout->addWidget(lbltablimit, 10, 0);
	m_tablimit_sbox = new QSpinBox(this);
	m_tablimit_sbox->setRange(0, 99);
	m_tablimit_sbox->setValue(m_mainwidget->tablimit());
	gridlayout->addWidget(m_tablimit_sbox, 10, 1);

	QPushButton *btnok = new QPushButton("&Ok", this);	// some default ok button should exist :)
	connect( btnok, SIGNAL(clicked()), this, SLOT(saveAndClose()) );
	QPushButton *btncancel = new QPushButton("&Cancel", this);
	connect( btncancel, SIGNAL(clicked()), this, SLOT(close()) );

	QHBoxLayout *hlayout = new QHBoxLayout();
	hlayout->addWidget(btnok);
	hlayout->addWidget(btncancel);

	vlayout->addLayout(gridlayout);
	vlayout->addLayout(hlayout);
}

void ConfWidget::saveAndClose()
{
	// send the conf stuff to the engine !
	//qDebug() << "ip =" << m_lineip->text();
	m_engine->setServerip( m_lineip->text() );
	//qDebug() << "port =" << m_lineport->text().toUShort();
	m_engine->setServerport( m_lineport->text().toUShort() );
	m_engine->setServerAst( m_lineast->text() );
	//qDebug() << "login =" << m_linelogin->text();
	//qDebug() << "protocol =" << m_protocombo->currentText().toLower();
	m_engine->setLogin( m_protocombo->currentText().toLower()
	                    + m_linelogin->text() );
	//m_engine->setLogin( m_linelogin->text() );
	//qDebug() << "password =" << m_linepasswd->text();
	m_engine->setPasswd( m_linepasswd->text() );
	//qDebug() << "autoconnect =" << m_autoconnect->checkState();
	m_engine->setAutoconnect( m_autoconnect->checkState() == Qt::Checked );
	//qDebug() << "trytoreconnect =" << m_trytoreconnect->checkState();
	m_engine->setTrytoreconnect( m_trytoreconnect->checkState() == Qt::Checked );
	m_engine->setKeepaliveinterval( m_kainterval_sbox->value()*1000 );
	m_engine->setTrytoreconnectinterval( m_tryinterval_sbox->value()*1000 );
	m_engine->saveSettings();
	m_mainwidget->setTablimit(m_tablimit_sbox->value());
	close();
}

