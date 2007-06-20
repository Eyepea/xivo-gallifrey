/* $Id$ */
#ifndef __SWITCHBOARDCONF_H__
#define __SWITCHBOARDCONF_H__

#include <QDialog>

class LoginEngine;
class SwitchBoardEngine;
class SwitchBoardWindow;
class QLineEdit;
class QSpinBox;
class QCheckBox;
class QComboBox;

/*! \brief Configuration dialog
 */
class SwitchBoardConfDialog : public QDialog
{
	Q_OBJECT
public:
	SwitchBoardConfDialog(SwitchBoardEngine * engine,
	                      LoginEngine * loginengine,
	                      SwitchBoardWindow * window,
	                      QWidget *parent = 0);
private slots:
	void saveAndClose();	//!< save configuration and close dialog
private:
	SwitchBoardEngine * m_engine;	//!< switchboard engine
	LoginEngine * m_loginengine;	//!< presence server engine
	SwitchBoardWindow * m_window;	//!< main application window
	QLineEdit * m_serverhost;	//!< server host
	QLineEdit * m_sbport;		//!< server port (switchboard)
	QLineEdit * m_loginport;	//!< server port (presence)
//	QSpinBox * m_widthsb;
	QCheckBox * m_autoconnect;	//!< autoconnect checkbox
	QCheckBox * m_presence;		//!< connect to presence server checkbox
	QLineEdit * m_asterisk;	//!< asterisk server id
	QComboBox * m_protocombo;	//!< protocol(SIP/IAX/...) combo
	QLineEdit * m_userid;	//!< user id
	QLineEdit * m_passwd;	//!< password (for presence server)
};

#endif

