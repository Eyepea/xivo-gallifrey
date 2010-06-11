/* XiVO Client
 * Copyright (C) 2010, Proformatique
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

#ifndef __PARKINGINFO_H__
#define __PARKINGINFO_H__

#include "baselib_export.h"
#include <QStringList>
#include <QHash>
#include <QVariant>

/*! \brief for storing parking (conference room) infos
 *
 * All attributes use implicitly shared data
 * so it would be not too costy to copy/return this
 * class.
 */
class BASELIB_EXPORT ParkingInfo
{
    public:
        ParkingInfo() {};
        //! Copy constructor. Just copy all attributes
        ParkingInfo(const ParkingInfo &);
        //! cast to QString operator for debugging
        operator QString() const {
                QString str("ParkingInfo(");
                str.append(m_roomnumber);
                str.append(", ");
                str.append(m_roomname);
                str.append(", [");
                foreach(QString uid, m_uniqueids.keys()) {
                        str.append(uid);
                        str.append(" ");
                }
                str.append("])");
                return str;
        }
        bool setProperties(const QVariantMap &);
        bool update(const QVariantMap &);
        const QString &roomname() const;  //! conference room name
        const QString &roomnumber() const;  //! conference room number
        const QString &adminid() const;  //! conference room admin id
        const QString &adminnum() const;  //! conference room admin num
        bool paused() const;  //! conference room paused status
        const QStringList &adminlist() const;  //! conference room admin list
        const QVariantMap &uniqueids() const;  //! conference room uniqueids
        
    private:
        QString m_context;  //!< room context
        QString m_roomname;     //!< room access name
        QString m_roomnumber;   //!< room access number (if any)
        QString m_pin;      //!< room pin number (if any)
        QString m_adminpin; //!< room admin pin number (if any) 
        QString m_adminid;  //!< admin id ??? (global)
        QString m_adminnum;  //!< admin num ( local in the parking room )
        bool m_paused;
        QStringList m_adminlist;    //!< admin list (user ids)
        QVariantMap m_uniqueids;    //!< people in this conference room
};

#endif /* __PARKINGINFO_H__ */
