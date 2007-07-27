/* XIVO switchboard
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

/* $Id$ */
#ifndef __SWITCHBOARDCONF_H__
#define __SWITCHBOARDCONF_H__

#include <QDialog>

class QCheckBox;
class QComboBox;
class QLineEdit;
class QSpinBox;

class BaseEngine;
class MainWindow;
class SwitchBoardWindow;

/*! \brief Configuration dialog
 */
class SwitchBoardConfDialog : public QDialog
{
	Q_OBJECT
public:
	SwitchBoardConfDialog(BaseEngine *,
	                      MainWindow *);
private slots:
	void saveAndClose();	//!< save configuration and close dialog
private:
	BaseEngine * m_engine;	//!< switchboard engine
	SwitchBoardWindow * m_window;	//!< main application window
	QLineEdit * m_serverhost;	//!< server host
	QLineEdit * m_sbport;		//!< server port (switchboard)
	QLineEdit * m_loginport;	//!< server port (presence)
//	QSpinBox * m_widthsb;
	QCheckBox * m_autoconnect;	//!< autoconnect checkbox
	QCheckBox * m_trytoreconnect;	//!< "Try to reconnect" Checkbox
	QSpinBox  * m_tryinterval_sbox;	//!< "Try to reconnect" interval
	QSpinBox  * m_history_sbox;	//!< History size
	QSpinBox  * m_tablimit_sbox;	//!< Maximum number of tabs
	QCheckBox * m_presence;		//!< connect to presence server checkbox
	QLineEdit * m_asterisk;	//!< asterisk server id
	QComboBox * m_protocombo;	//!< protocol(SIP/IAX/...) combo
	QLineEdit * m_userid;	//!< user id
	QLineEdit * m_passwd;	//!< password (for presence server)
	MainWindow *m_mainwindow;	//!< MainWidget where some parameters are commited to
};

#endif

