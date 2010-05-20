/* XiVO Client
 * Copyright (C) 2007-2010, Proformatique
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

#ifndef __XLETFACTORY_H__
#define __XLETFACTORY_H__

#include <QObject>
#include <QHash>
#include <QDir>
#include "xlet.h"
class QWidget;
class BaseEngine;

/*! xlet creator function prototype */
typedef XLet* (*newXLetProto)(BaseEngine *, QWidget *);

/*! \brief XLet Factory
 *
 * Use this class to instanciate a XLet.
 * XLet would be first searched in built-in XLet list
 * and then in plugins directory if not built-in. */
class XLetFactory : public QObject
{
    public:
        XLetFactory(BaseEngine *engine, QObject *parent);
        XLet* newXLet(const QString &id, QWidget *topwindow) const;

    private:
        BaseEngine * m_engine;  //!< BaseEngine reference
        QHash<QString, newXLetProto> m_xlets;  //!< built in XLets constuctors
        QDir m_pluginsDir;  //!< directory where to find plugins
        bool m_pluginsDirFound; //!< Is plugins directory found.
};

#endif

