/*
XIVO customer information client : popup profile for incoming calls
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
#include <QApplication>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QTabWidget>
#include <QAction>
#include <QHideEvent>
#include <QLabel>
#include <QTime>
#include <QMessageBox>
#include <QDebug>
#include <QVBoxLayout>
#include "mainwidget.h"
#include "confwidget.h"
#include "popup.h"

/*! \brief Constructor
 *
 * This Constructor creates the 3 buttons in a
 * vertical box layout and connect signals with slots.
 */
MainWidget::MainWidget(Engine *engine, QWidget *parent)
: QMainWindow(parent), m_engine(engine), m_systrayIcon(0),
  m_icon(":/xivoicon.png"), m_icongrey(":/xivoicon-grey.png")
{
	createActions();
	createMenus();
	if( QSystemTrayIcon::isSystemTrayAvailable() )
		createSystrayIcon();

	connect( engine, SIGNAL(logged()), this, SLOT(setConnected()) );
	connect( engine, SIGNAL(delogged()), this, SLOT(setDisconnected()) );
	connect( engine, SIGNAL(newProfile(Popup *)),
	         this, SLOT(showNewProfile(Popup *)) );

	
	//setWindowFlags(Qt::Dialog);
	//layout->setSizeConstraint(QLayout::SetFixedSize);	// remove minimize and maximize button
	setWindowTitle(QString("Xivo Client"));
	setWindowIcon(QIcon(":/xivoicon.png"));
	statusBar()->clearMessage();
	

	m_tabwidget = new QTabWidget();


	QWidget * wid = new QWidget();
	QVBoxLayout * vbox = new QVBoxLayout(wid);
	QLabel * labelwhat = new QLabel(tr("Tell the Switchboard :"));
	m_messagetosend = new QLineEdit();
	connect( m_messagetosend, SIGNAL(returnPressed()),
		 this, SLOT(affTextChanged()) );
	vbox->addWidget(m_tabwidget, 1);
	vbox->addWidget(labelwhat, 0);
	vbox->addWidget(m_messagetosend, 0);
	wid->show();
	setCentralWidget(wid);
	//setCentralWidget(m_tabwidget);
	
	
	QSettings settings;
	m_tablimit = settings.value("display/tablimit", 5).toInt();
}


void MainWidget::affTextChanged()
{
	QString txt = m_messagetosend->text();
	txt.replace(" ", "_");
	m_engine->sendMessage(txt.toUtf8());
	m_messagetosend->setText("");
}


void MainWidget::createActions()
{
	m_cfgact = new QAction(tr("&Configuration"), this);
	m_cfgact->setStatusTip(tr("Configure account and connection options"));
	connect( m_cfgact, SIGNAL(triggered()), this, SLOT(popupConf()) );

	m_quitact = new QAction(tr("&Quit"), this);
	m_quitact->setStatusTip(tr("Close the application"));
	connect( m_quitact, SIGNAL(triggered()), m_engine, SLOT(stop()) );
	connect( m_quitact, SIGNAL(triggered()), qApp, SLOT(quit()) );

	m_systrayact = new QAction(tr("To S&ystray"), this);
	m_systrayact->setStatusTip(tr("Go to the system tray"));
	connect( m_systrayact, SIGNAL(triggered()), this, SLOT(hide()) );
	m_systrayact->setEnabled( QSystemTrayIcon::isSystemTrayAvailable() );

	m_connectact = new QAction(tr("&Connect"), this);
	m_connectact->setStatusTip(tr("Connect to the server"));
	connect( m_connectact, SIGNAL(triggered()), m_engine, SLOT(start()) );
	m_disconnectact = new QAction(tr("&Disconnect"), this);
	m_disconnectact->setStatusTip(tr("Disconnect from the server"));
	connect( m_disconnectact, SIGNAL(triggered()), m_engine, SLOT(stop()) );
	//QActionGroup * connectgroup = new QActionGroup(this);
	//connectgroup->addAction( m_connectact );
	//connectgroup->addAction( m_disconnectact );
	m_connectact->setEnabled(true);
	m_disconnectact->setEnabled(false);

	// Availability actions :
	m_availgrp = new QActionGroup( this );
	m_availgrp->setExclusive(true);

	m_avact_avail = new QAction( tr("&Available"), this );
	m_avact_avail->setCheckable(true);
//	m_avact_avail->setStatusTip( tr("") );
	connect( m_avact_avail, SIGNAL(triggered()),
	         m_engine, SLOT(setAvailable()) );
	m_availgrp->addAction( m_avact_avail );
	//	m_avact_avail->setChecked( true );
	m_avact_away = new QAction( tr("A&way"), this );
	m_avact_away->setCheckable(true);
	connect( m_avact_away, SIGNAL(triggered()),
	         m_engine, SLOT(setAway()) );
	m_availgrp->addAction( m_avact_away );
	m_avact_brb = new QAction( tr("&Be Right Back"), this );
	m_avact_brb->setCheckable(true);
	connect( m_avact_brb, SIGNAL(triggered()),
	         m_engine, SLOT(setBeRightBack()) );
	m_availgrp->addAction( m_avact_brb );
	m_avact_otl = new QAction( tr("&Out To Lunch"), this );
	m_avact_otl->setCheckable(true);
	connect( m_avact_otl, SIGNAL(triggered()),
	         m_engine, SLOT(setOutToLunch()) );
	m_availgrp->addAction( m_avact_otl );
	m_avact_dnd = new QAction( tr("&Do not disturb"), this );
	m_avact_dnd->setCheckable(true);
	connect( m_avact_dnd, SIGNAL(triggered()),
	         m_engine, SLOT(setDoNotDisturb()) );
	m_availgrp->addAction( m_avact_dnd );

	if(m_engine->getAvailState() == QString("berightback"))
		m_avact_brb->setChecked( true );
	else if(m_engine->getAvailState() == QString("donotdisturb"))
		m_avact_dnd->setChecked( true );
	else if(m_engine->getAvailState() == QString("away"))
		m_avact_away->setChecked( true );
	else if(m_engine->getAvailState() == QString("outtolunch"))
		m_avact_otl->setChecked( true );
	else
		m_avact_avail->setChecked( true );

}

void MainWidget::createMenus()
{
	QMenu * filemenu = menuBar()->addMenu(tr("&File"));
	filemenu->addAction( m_cfgact );
	filemenu->addAction( m_systrayact );
	filemenu->addSeparator();
	filemenu->addAction( m_connectact );
	filemenu->addAction( m_disconnectact );
	filemenu->addSeparator();
	filemenu->addAction( m_quitact );

	QMenu * avail = menuBar()->addMenu(tr("&Availability"));
	avail->addActions( m_availgrp->actions() );

	QMenu * helpmenu = menuBar()->addMenu(tr("&Help"));
	helpmenu->addAction( tr("&About XIVO Client"), this, SLOT(about()) );
	helpmenu->addAction( tr("About &Qt"), qApp, SLOT(aboutQt()) );
}

/*!
 * tablimit property is defining the maximum
 * number of profile that can be displayed in the Tabbed
 * widget.
 *
 * \sa setTablimit
 * \sa m_tablimit
 */
int MainWidget::tablimit() const
{
	return m_tablimit;
}

/*!
 * \sa tablimit
 * \sa m_tablimit
 */
void MainWidget::setTablimit(int tablimit)
{
	QSettings settings;
	m_tablimit = tablimit;
	settings.setValue("display/tablimit", m_tablimit);
}

/*! \brief create and show the system tray icon
 *
 * Create the system tray icon, show it and connect its
 * activated() signal to some slot 
 */
void MainWidget::createSystrayIcon()
{
	m_systrayIcon = new QSystemTrayIcon(m_icongrey, this);
	QMenu * menu = new QMenu(QString("SystrayMenu"), this);
	menu->addActions( m_availgrp->actions() );
	menu->addSeparator();
	menu->addAction(m_cfgact);
	menu->addAction(m_quitact);
	m_systrayIcon->setContextMenu( menu );
	m_systrayIcon->show();
	//connect( m_systrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
	//         this, SLOT(show()) );
	connect( m_systrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
	         this, SLOT(systrayActivated(QSystemTrayIcon::ActivationReason)) );
	connect( m_systrayIcon, SIGNAL(messageClicked()),
	         this, SLOT(systrayMsgClicked()) );
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
	qDebug() << " systrayActivated() reason=" << reason
	         << "visible=" << isVisible() << " hidden=" << isHidden()
	         << "active=" << isActiveWindow();
	// QSystemTrayIcon::DoubleClick
	// QSystemTrayIcon::Trigger
	if (reason == QSystemTrayIcon::Trigger)
	{
		if( isVisible() && !isActiveWindow() )
		{
			showNormal();
			activateWindow();
			raise();
		}
		else
		{
			// Toggle visibility
			setVisible(!isVisible());
			if( isVisible() )
			{
				showNormal();
				activateWindow();
				raise();
			}
		}
	}
}

/*!
 * This slot implementation show, activate (and raise) the
 * window.
 */
void MainWidget::systrayMsgClicked()
{
	qDebug() << "MainWidget::systrayMsgClicked()";
	setVisible(true);
	activateWindow();
	raise();
}

//! Enable the Start Button and set the red indicator
void MainWidget::setDisconnected()
{
	m_connectact->setEnabled(true);
	m_disconnectact->setEnabled(false);
	if(m_systrayIcon)
		m_systrayIcon->setIcon(m_icongrey);
	statusBar()->showMessage(tr("Disconnected"));
}

//! Disable the Start Button and set the green indicator
void MainWidget::setConnected()
{
	m_connectact->setEnabled(false);
	m_disconnectact->setEnabled(true);
	if(m_systrayIcon)
		m_systrayIcon->setIcon(m_icon);
	statusBar()->showMessage(tr("Connected"));
}

/*!
 * Display the new profile in the tabbed area
 * and show a message with the systray icon
 */
void MainWidget::showNewProfile(Popup * popup)
{
	QTime currentTime = QTime::currentTime();
	QString currentTimeStr = currentTime.toString("hh:mm:ss");
	if(m_systrayIcon)
	{
		m_systrayIcon->showMessage(tr("Incoming call"),
		                           currentTimeStr + "\n"
								   + popup->message() );
	}
	if(m_tabwidget)
	{
		int index = m_tabwidget->addTab(popup, currentTimeStr);
		qDebug() << "added tab" << index;
		m_tabwidget->setCurrentIndex(index);
		if(index >= m_tablimit)
		{
			// close the first widget
			m_tabwidget->widget(0)->close();
		}
		// show the window and give it the focus.
		setVisible(true);
		activateWindow();
		raise();
	}
	else
	{
		popup->show();
	}
}

void MainWidget::hideEvent(QHideEvent *event)
{
	// called when minimized
	//qDebug() << "MainWidget::hideEvent(" << event << ")";
	// if systray available
	qDebug() << "MainWidget::hideEvent : spontaneous="
	         << event->spontaneous() << " isMinimized()="
			 << isMinimized();
	//if(event->spontaneous())
	//	event->ignore();
	//else
		event->accept();
	if( QSystemTrayIcon::isSystemTrayAvailable() )
		setVisible(false);
}

/*! \brief Catch the Close event
 *
 * This method is called when the user click the upper right X of the
 * Window.
 * We ignore the event but hide the window (to minimize it to systray)
 */
void MainWidget::closeEvent(QCloseEvent *event)
{
	qDebug() << "MainWidget::closeEvent()";
	//event->accept();
	if( QSystemTrayIcon::isSystemTrayAvailable() )
		setVisible( false );
	else
		showMinimized();
	event->ignore();
}

#if 0
void MainWidget::changeEvent(QEvent *event)
{
	qDebug() << "MainWidget::changeEvent() eventtype=" << event->type();
	//if(event->type() == 105)
	//	event->accept();
		//event->ignore();
}
#endif

/*! \brief Shows the about box
 *
 * Use a QMessageBox to show the about box.
 * The about box contains the version.
 */
void MainWidget::about()
{
	QString applicationVersion("0.1");
	QMessageBox::about(this,
			   tr("About XIVO Client"),
			   tr("<h3>XIVO Client</h3>"
			      "<p>This application shows to the user the profile associated"
			      " with incoming phone calls.</p>"
			      "<p>Version : %1</p>"
			      "<p>(C) 2007 <b>Proformatique</b> "
			      "<a href=\"http://www.proformatique.com\">"
			      "http://www.proformatique.com</a></p>"
			      "<p>67 rue Voltaire 92800 Puteaux FRANCE</p>"
			      "<p>E-mail : technique@proformatique.com</p>"
			      "<p>(+33/0)1.41.38.99.60</p>"
			      "<p>Author : Thomas Bernard</p>").arg(applicationVersion) );
}

