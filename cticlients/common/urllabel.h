/* XIVO CTI clients
Copyright (C) 2007, 2008  Proformatique

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/

/* $Revision$
 * $Date$
 */

#ifndef __URLLABEL_H__
#define __URLLABEL_H__

#include <QLabel>

/*! \brief UrlLabel Widget
 *
 * This label is a QLabel designed to display an URL. */
class UrlLabel : public QLabel
{
	Q_OBJECT
public:
	UrlLabel(const QString & url, QWidget *parent=0);
};

#endif
