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

#include <QDateTime>
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QTimerEvent>

#include "datetimepanel.h"

/*! \brief Constructor
 */
DatetimePanel::DatetimePanel(QWidget * parent)
    : QWidget(parent)
{
    m_datetime = new QLabel(QDateTime::currentDateTime().toString(Qt::LocaleDate));
    m_gridlayout = new QGridLayout(this);
        
    m_gridlayout->addWidget( m_datetime, 1, 1, Qt::AlignCenter);
    m_gridlayout->setColumnStretch( 0, 1 );
    m_gridlayout->setColumnStretch( 2, 1 );
    m_gridlayout->setRowStretch( 0, 1 );
    m_gridlayout->setRowStretch( 2, 1 );
        
    startTimer(1000);
}

void DatetimePanel::timerEvent(QTimerEvent *)
{
    m_datetime->setText(QDateTime::currentDateTime().toString(Qt::LocaleDate));
}
