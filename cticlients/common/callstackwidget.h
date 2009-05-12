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

#ifndef __CALLSTACKWIDGET_H__
#define __CALLSTACKWIDGET_H__

#include <QHash>
#include <QList>
#include <QObject>
#include <QWidget>
#include <QString>
#include <QDateTime>

#include "callwidget.h"
#include "xivoconsts.h"

class QVBoxLayout;

class UserInfo;
class BaseEngine;

/*! \brief Widget displaying the current open channels for a phone line.
 */
class CallStackWidget : public QWidget
{
    Q_OBJECT
public:
    CallStackWidget(BaseEngine * engine, QWidget * parent = 0); //!< Constructor
public slots:
    void updateUser(UserInfo *);
    void updateDisplay();
    void hupchan(const QString &);
    void transftonumberchan(const QString &);
    void parkcall(const QString &);
    void reset();
    void monitorPeer(UserInfo *);
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
private:
    //        void emptyList();        //!< remove all calls from the list
signals:
    //! originate, transfer & atxfer signals
    void actionCall(const QString &,
                    const QString &,
                    const QString & dst = "");
    void changeTitle(const QString &);                //!< change Title
    void monitorPeerRequest(const QString &);        //!< send the userid of the new monitored peer
private:
    BaseEngine * m_engine;      //!< pointer to the BaseEngine
    UserInfo * m_monitored_ui;  //!< user currently monitored
    QVBoxLayout * m_layout;        //!< Vertical Layout used
        
    QHash<QString, CallWidget *> m_affhash;        //!< List of CallWidget Widgets
};

#endif
