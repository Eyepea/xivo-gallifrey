/* XiVO Client
 * Copyright (C) 2007-2009, Proformatique
 *
 * This file is part of XiVO Client.
 *
 * XiVO Client is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, with a Section 7 Additional
 * Permission as follows:
 *   This notice constitutes a grant of such permission as is necessary
 *   to combine or link this software, or a modified version of it, with
 *   the OpenSSL project's "OpenSSL" library, or a derivative work of it,
 *   and to copy, modify, and distribute the resulting work. This is an
 *   extension of the special permission given by Trolltech to link the
 *   Qt code with the OpenSSL library (see
 *   <http://doc.trolltech.com/4.4/gpl.html>). The OpenSSL library is
 *   licensed under a dual license: the OpenSSL License and the original
 *   SSLeay license.
 *
 * XiVO Client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XiVO Client.  If not, see <http://www.gnu.org/licenses/>.
 */

/* $Revision$
 * $Date$
 */

#ifndef __DISPLAYMESSAGES_H__
#define __DISPLAYMESSAGES_H__

#include <QWidget>
#include <QLabel>

class QVBoxLayout;
class QLineEdit;
class QTableWidget;

/*! \brief Widget to display a list of messages
 *
 * dispays the messages in a table with timestamps.
 */
class DisplayMessagesPanel : public QWidget
{
    Q_OBJECT
        public:
    DisplayMessagesPanel(QWidget * parent = 0);
    public slots:
    /*         void affTextChanged(); */
    void addMessage(const QString &);
    /* signals: */
    /*         void emitDial(const QString &); */
 private:
    //QLabel * m_text;
    //! Table
    QTableWidget * m_table;
};

#endif
