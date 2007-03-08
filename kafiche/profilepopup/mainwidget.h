#ifndef __MAINWIDGET_H__
#define __MAINWIDGET_H__
#include <QWidget>
#include <QPushButton>
#include <QSystemTrayIcon>
#include "engine.h"

class QSystemTrayIcon;

/*! \brief Main window class
 *
 * This widget contains buttons enabling the user
 * to popup the config window, to quit and to start
 * the login process.
 */
class MainWidget: public QWidget
{
	Q_OBJECT
public:
	//! Constructor
	MainWidget(Engine *engine, QWidget *parent=0);
private slots:
	void popupConf();
	void setDisconnected();
	void setConnected();
	void systrayActivated(QSystemTrayIcon::ActivationReason);
protected:
	void hideEvent(QHideEvent *event);
	//void closeEvent(QCloseEvent *event);
	//void changeEvent(QEvent *event);
private:
	void createSystrayIcon();
private:
	Engine * m_engine;			//!< pointer to the Engine used
	QPushButton * m_btnstart;	//!< Start Button
	QSystemTrayIcon * m_systrayIcon;	//!< System Tray Icon
	QIcon m_iconred;	//!< Icon object with red indicator
	QIcon m_icongreen;	//!< Icon object with green indicator
};

#endif

