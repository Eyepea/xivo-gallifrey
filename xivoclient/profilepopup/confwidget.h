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

/* $Id: $ */
#ifndef __CONFWIDGET_H__
#define __CONFWIDGET_H__

#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include "engine.h"

class QSpinBox;
class QCheckBox;
class QComboBox;
class MainWidget;

/*! \brief Configuration Window
 *
 * This Widget enables the user to edit the connection
 * parameters to the identification server */
/* could be a QDialog instead of QWidget */
//class ConfWidget: public QWidget
class ConfWidget: public QDialog
{
	Q_OBJECT
public:
	/*! \brief Constructor
	 *
	 * Construct the widget and its layout.
	 * Fill widgets with values got from the Engine object.
	 * Once constructed, the Widget is ready to be shown.
	 * \param engine	related Engine object where parameters will be modified
	 * \param parent	parent QWidget
	 */
	ConfWidget(Engine *engine, MainWidget *parent);
	//ConfWidget(Engine *engine, QWidget *parent = 0);
private slots:
	//! Save the configuration to the Engine object and close
	void saveAndClose();
private:
	QLineEdit *m_lineip;		//!< IP/hostname of the server
	QLineEdit *m_lineport;		//!< port of the server
	QLineEdit *m_lineast;		//!< id name of the Asterisk server
	QComboBox *m_protocombo;	//!< Protocol SIP/IAX
	QLineEdit *m_linelogin;		//!< user login
	QLineEdit *m_linepasswd;	//!< user password
	QCheckBox *m_autoconnect;	//!< Auto connect checkbox
	QSpinBox *m_kainterval_sbox;	//!< Keep alive interval
	QCheckBox *m_trytoreconnect;	//!< "Try to reconnect" Checkbox
	QSpinBox *m_tryinterval_sbox;	//!< "Try to reconnect" interval
	QSpinBox *m_tablimit_sbox;	//!< Maximum number of tabs
	Engine *m_engine;			//!< Engine object parameters are commited to
	MainWidget *m_mainwidget;	//!< MainWidget where some parameters are commited to
};

#endif

