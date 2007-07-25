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
#ifndef __DIALPANEL_H__
#define __DIALPANEL_H__
#include <QObject>
#include <QWidget>
#include <QList>

class QVBoxLayout;
class QLineEdit;
class QComboBox;
class QMouseEvent;

/*! \brief Simple widget to enter a number and dial it
 */
class DialPanel : public QWidget
{
	Q_OBJECT
public:
	DialPanel(QWidget * parent = 0);
protected:
        /*void mouseMoveEvent(QMouseEvent *);	//!< Catch mouse press events */
        /*void mousePressEvent(QMouseEvent *); */
        void dragEnterEvent(QDragEnterEvent *);
	void dropEvent(QDropEvent *);
public slots:
	//void textEdited(const QString &);
	void inputValidated();
signals:
	void emitDial(const QString &);		//!< dial a number
	void originateCall(const QString &, const QString &);   	//!< originates a number
private:
	//QLineEdit * m_input;
	QComboBox * m_input;	//!< input widget
};

#endif

