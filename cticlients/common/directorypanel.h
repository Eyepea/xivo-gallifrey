/* XIVO CTI clients
 * Copyright (C) 2007, 2008  Proformatique
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Linking the Licensed Program statically or dynamically with other
 * modules is making a combined work based on the Licensed Program. Thus,
 * the terms and conditions of the GNU General Public License version 2
 * cover the whole combination.
 *
 * In addition, as a special exception, the copyright holders of the
 * Licensed Program give you permission to combine the Licensed Program
 * with free software programs or libraries that are released under the
 * GNU Library General Public License version 2.0 or GNU Lesser General
 * Public License version 2.1 or any later version of the GNU Lesser
 * General Public License, and with code included in the standard release
 * of OpenSSL under a version of the OpenSSL license (with original SSLeay
 * license) which is identical to the one that was published in year 2003,
 * or modified versions of such code, with unchanged license. You may copy
 * and distribute such a system following the terms of the GNU GPL
 * version 2 for the Licensed Program and the licenses of the other code
 * concerned, provided that you include the source code of that other code
 * when and as the GNU GPL version 2 requires distribution of source code.
*/

/* $Revision$
 * $Date$
 */

#ifndef __DIRECTORYPANEL_H__
#define __DIRECTORYPANEL_H__

#include <QWidget>

class QDropEvent;
class QFocusEvent;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;

class ExtendedLineEdit;
class ExtendedTableWidget;
class PeerChannel;

/*! \brief Directory allowing search
 */
class DirectoryPanel : public QWidget
{
	Q_OBJECT
public:
	DirectoryPanel(QWidget * parent = 0);
        void myfocus();
protected:
        void dropEvent(QDropEvent *);
        void focusInEvent(QFocusEvent *);
signals:
	//! start a search
	void searchDirectory(const QString &);
	//! dial selected number
	void emitDial(const QString &, bool);
	void copyNumber(const QString &);
	//! transfer one of my call to this number
	void transferCall(const QString &, const QString &);
	//! originate a call
	void originateCall(const QString &, const QString &);
private slots:
	void dialNumber();
        void sendMail();
	void startSearch();
        void itemClicked(QTableWidgetItem *);
	void itemDoubleClicked(QTableWidgetItem *);
	void transferChan(const QString &);
public slots:
        void contextMenuEvent(QContextMenuEvent *);
	void setSearchResponse(const QString &);
	void stop();
	void updateMyCalls(const QStringList &, const QStringList &, const QStringList &);
private:
	ExtendedLineEdit * m_searchText;	//!< search text input
	ExtendedTableWidget * m_table;		//!< table to display results
	QPushButton * m_searchButton;	//!< button
	QString m_numberToDial;		//!< used to store number to dial or to transfer to
	QString m_mailAddr;		//!< used to store email address
	QList<PeerChannel *> m_mychannels;	//!< "my channels" list for transfer menu
};

#endif
