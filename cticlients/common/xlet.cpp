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

#include "xlet.h"
#include "baseengine.h"

XLet::XLet(BaseEngine * engine, QWidget * parent)
    : QWidget(parent),
      m_engine(engine)
{
    connect( this, SIGNAL(logAction(const QString &)),
             m_engine, SLOT(logAction(const QString &)) );
    connect( this, SIGNAL(shouldNotOccur(const QString &, const QString &)),
             m_engine, SLOT(shouldNotOccur(const QString &, const QString &)) );
    connect( this, SIGNAL(ipbxCommand(const QVariantMap &)),
             m_engine, SLOT(ipbxCommand(const QVariantMap &)) );
}

/*! \brief connect actionCall signal to the engine
 */
void XLet::connectDials()
{
    connect( this, SIGNAL(actionCall(const QString &,
                                     const QString &,
                                     const QString &)),
             m_engine, SLOT(actionCall(const QString &,
                                       const QString &,
                                       const QString &)) );
}

