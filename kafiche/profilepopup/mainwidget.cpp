#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSystemTrayIcon>
#include <QMenu>
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

	//qDebug() << "QSystemTrayIcon::isSystemTrayAvailable()="
	//         << QSystemTrayIcon::isSystemTrayAvailable();
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
	setWindowTitle(QString("KaFiche"));
}

/*! \brief create and show the system tray icon
 *
 * Create the system tray icon, show it and connect its
 * activated() signal to some slot 
 */
void MainWidget::createSystrayIcon()
{
	// the icon is read from the resources.
	m_systrayIcon = new QSystemTrayIcon(QIcon(":/xivoicon.png"), this);
	QMenu * menu = new QMenu(QString("SystrayMenu"), this);
	menu->addAction( "&Config", this, SLOT(popupConf()) );
	menu->addAction( "&Quit", qApp, SLOT(quit()) );
	m_systrayIcon->setContextMenu( menu );
	m_systrayIcon->show();
	//connect( m_systrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
	//         this, SLOT(show()) );
	connect( m_systrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
	         this, SLOT(systrayActivated(QSystemTrayIcon::ActivationReason)) );
// QSystemTrayIcon::ActivationReason
	//qDebug() << "QSystemTrayIcon::supportsMessages() = "
	//         << QSystemTrayIcon::supportsMessages();
}

// === SLOTS implementations ===

/*! \brief popup a new configuration window
 *
 * create a new configuration widget/dialog which
 * will enable the user to modify connection options for
 * m_engine, and then show it. */
void MainWidget::popupConf()
{
	// TODO : prevent the user from opening several confWidget
	// show, bring to foreground if one is allready existing.
	ConfWidget * conf = new ConfWidget(m_engine, this);
	conf->show();
}

/*! \brief process clicks to the systray icon
 *
 * This slot is connected to the activated() signal of the
 * System Tray icon. It currently toggle the visibility
 * of the MainWidget on a simple left click. */
void MainWidget::systrayActivated(QSystemTrayIcon::ActivationReason reason)
{
	qDebug() << "MainWidget::systrayActivated() " << reason;
	qDebug() << "visible=" << isVisible() << " hidden=" << isHidden();
	qDebug() << "active=" << isActiveWindow();
	// QSystemTrayIcon::DoubleClick
	// QSystemTrayIcon::Trigger
	if (reason == QSystemTrayIcon::Trigger)
	{
		if( isVisible() && !isActiveWindow() )
		{
			activateWindow();
			raise();
		}
		else
		{
			// Toggle visibility
			setVisible(!isVisible());
			if( isVisible() )
			{
				activateWindow();
				raise();
			}
		}
	}
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

