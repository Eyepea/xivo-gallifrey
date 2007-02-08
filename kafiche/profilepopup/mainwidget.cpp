#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSystemTrayIcon>
#include <QDebug>
#include "mainwidget.h"
#include "confwidget.h"

/*! \brief Constructor
 *
 * This Constructor creates the 3 buttons in a
 * vertical box layout and connect signals with slots.
 */
MainWidget::MainWidget(Engine *engine, QWidget *parent)
: QWidget(parent), m_engine(engine), m_systrayIcon(0)
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	QPushButton *btnconf = new QPushButton("&Configure", this);
	layout->addWidget(btnconf);
	connect( btnconf, SIGNAL(clicked()), this, SLOT(popupConf()) );

	m_btnstart = new QPushButton("&Start", this);
	m_btnstart->setDefault(true);
	layout->addWidget(m_btnstart);
	connect( m_btnstart, SIGNAL(clicked()), m_engine, SLOT(start()) );

	QPushButton *btnquit = new QPushButton("&Quit", this);
	layout->addWidget(btnquit);
	connect( btnquit, SIGNAL(clicked()), qApp, SLOT(quit()) );

	qDebug() << "QSystemTrayIcon::isSystemTrayAvailable()="
	         << QSystemTrayIcon::isSystemTrayAvailable();
	QPushButton *btnsystray = new QPushButton("To S&ystray", this);
	btnsystray->setEnabled( QSystemTrayIcon::isSystemTrayAvailable() );
	layout->addWidget(btnsystray);
	connect( btnsystray, SIGNAL(clicked()), this, SLOT(hide()) );

	if( QSystemTrayIcon::isSystemTrayAvailable() )
		createSystrayIcon();

	connect( engine, SIGNAL(logged()), this, SLOT(disableStartButton()) );
	connect( engine, SIGNAL(delogged()), this, SLOT(enableStartButton()) );

	//setWindowFlags(Qt::Dialog);	
	layout->setSizeConstraint(QLayout::SetFixedSize);	// remove minimize and maximize button
	setWindowTitle(QString("hop"));
}

/*! \brief create and show the system tray icon
 *
 * Create the system tray icon, show it and connect its
 * activated() signal to some slot 
 */
void MainWidget::createSystrayIcon()
{
	m_systrayIcon = new QSystemTrayIcon(QIcon("xivoicon.png"), this);
	// add a menu ?
	// m_systrayIcon->setContextMenu()
	m_systrayIcon->show();
	connect( m_systrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
	         this, SLOT(show()) );
}

// === SLOTS implementations ===

/*! \brief popup a new configuration window
 *
 * create a new configuration widget/dialog which
 * will enable the user to modify connection options for
 * m_engine, and then show it. */
void MainWidget::popupConf()
{
	ConfWidget * conf = new ConfWidget(m_engine, this);
	conf->show();
}

//! Enable the Start Button
void MainWidget::enableStartButton()
{
	m_btnstart->setEnabled(true);
}

//! Disable the Start Button
void MainWidget::disableStartButton()
{
	m_btnstart->setEnabled(false);
}

void MainWidget::hideEvent(QHideEvent *event)
{
	// called when minimized
	//qDebug() << "MainWidget::hideEvent(" << event << ")";
	// if systray available
	setVisible(false);
}
/*
void MainWidget::closeEvent(QCloseEvent *event)
{
	qDebug() << "MainWidget::closeEvent()";
}

void MainWidget::changeEvent(QEvent *event)
{
	qDebug() << "MainWidget::changeEvent() eventtype=" << event->type();
}
*/

