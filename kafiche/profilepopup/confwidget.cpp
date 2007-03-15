#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
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

	QLabel *lbllogin = new QLabel(tr("Login"), this);
	m_linelogin = new QLineEdit(m_engine->login(), this);

	QLabel *lblpasswd = new QLabel(tr("Password"), this);
	m_linepasswd = new QLineEdit(m_engine->passwd(), this);
	m_linepasswd->setEchoMode(QLineEdit::Password);

	gridlayout->addWidget(lblip, 0, 0);
	gridlayout->addWidget(m_lineip, 0, 1);
	gridlayout->addWidget(lblport, 1, 0);
	gridlayout->addWidget(m_lineport, 1, 1);
	gridlayout->addWidget(lbllogin, 2, 0);
	gridlayout->addWidget(m_linelogin, 2, 1);
	gridlayout->addWidget(lblpasswd, 3, 0);
	gridlayout->addWidget(m_linepasswd, 3, 1);

	m_autoconnect = new QCheckBox(tr("Autoconnect at startup"), this);
	m_autoconnect->setCheckState( m_engine->autoconnect()?Qt::Checked:Qt::Unchecked );
	gridlayout->addWidget(m_autoconnect, 4, 0, 1, 0);
	m_trytoreconnect = new QCheckBox(tr("Try to reconnect"), this);
	m_trytoreconnect->setCheckState( m_engine->trytoreconnect()?Qt::Checked:Qt::Unchecked );
	gridlayout->addWidget(m_trytoreconnect, 5, 0, 1, 0);

	QLabel * lbltablimit = new QLabel( tr("Tab limit"), this);
	gridlayout->addWidget(lbltablimit, 6, 0);
	m_tablimit_sbox = new QSpinBox(this);
	m_tablimit_sbox->setRange(0, 99);
	m_tablimit_sbox->setValue(m_mainwidget->tablimit());
	gridlayout->addWidget(m_tablimit_sbox, 6, 1);

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
	qDebug() << "ip =" << m_lineip->text();
	m_engine->setServerip( m_lineip->text() );
	qDebug() << "port =" << m_lineport->text().toUShort();
	m_engine->setServerport( m_lineport->text().toUShort() );
	qDebug() << "login =" << m_linelogin->text();
	m_engine->setLogin( m_linelogin->text() );
	qDebug() << "password =" << m_linepasswd->text();
	m_engine->setPasswd( m_linepasswd->text() );
	qDebug() << "autoconnect =" << m_autoconnect->checkState();
	m_engine->setAutoconnect( m_autoconnect->checkState() == Qt::Checked );
	qDebug() << "trytoreconnect =" << m_trytoreconnect->checkState();
	m_engine->setTrytoreconnect( m_trytoreconnect->checkState() == Qt::Checked );
	m_engine->saveSettings();
	m_mainwidget->setTablimit(m_tablimit_sbox->value());
	close();
}

