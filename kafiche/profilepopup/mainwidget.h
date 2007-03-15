#ifndef __MAINWIDGET_H__
#define __MAINWIDGET_H__
#include <QWidget>
#include <QPushButton>
#include <QSystemTrayIcon>
#include <QMainWindow>
#include "engine.h"

class QSystemTrayIcon;
class QTabWidget;

/*! \brief Main window class
 *
 * This widget contains buttons enabling the user
 * to popup the config window, to quit and to start
 * the login process.
 */
class MainWidget: public QMainWindow
{
	Q_OBJECT
public:
	//! Constructor
	MainWidget(Engine *engine, QWidget *parent=0);
	int tablimit() const;
	void setTablimit(int);
private slots:
	void popupConf();
	void setDisconnected();
	void setConnected();
	void systrayActivated(QSystemTrayIcon::ActivationReason);
	void showNewProfile(Popup *);	//!< display a Profile widget
protected:
	void hideEvent(QHideEvent *event);	//!< Catch hide events
	//void closeEvent(QCloseEvent *event);
	//void changeEvent(QEvent *event);
private:
	void createActions();	//!< Create Actions (for menus)
	void createMenus();		//!< Create Menus
	void createSystrayIcon();	//!< Create the systray Icon and its menu
private:
	Engine * m_engine;			//!< pointer to the Engine used
	//QPushButton * m_btnstart;	//!< Start Button
	QSystemTrayIcon * m_systrayIcon;	//!< System Tray Icon
	QIcon m_iconred;	//!< Icon object with red indicator
	QIcon m_icongreen;	//!< Icon object with green indicator
	QTabWidget * m_tabwidget;	//!< Area to display profiles
	int m_tablimit;		//!< Maximum number of tabs in m_tabwidget
	// actions :
	QAction * m_cfgact;		//!< Configuration Action
	QAction * m_quitact;	//!< Quit Action
	QAction * m_systrayact;	//!< "Go to systray" action
	QAction * m_connectact;	//!< "Connect" Action
	QAction * m_disconnectact;	//!< "Disconnect" Action
};

#endif

