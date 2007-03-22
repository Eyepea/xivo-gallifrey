/*
kafiche client : popup profile for incoming calls
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
#include <QLabel>
#include "urllabel.h"

/*! \brief Constructor
 *
 * Call QLabel constructor, set properties to
 * allow link to be clickable and put some html magic
 * into the label text. */
UrlLabel::UrlLabel(const QString & url, QWidget * parent)
: QLabel(parent)
{
	QString displaytext, text;
	//setForegroundRole( QPalette::Link );
	setOpenExternalLinks(true);	// new in Qt 4.2
	setTextInteractionFlags( Qt::LinksAccessibleByMouse
                            | Qt::LinksAccessibleByKeyboard );
	//                        | Qt::TextSelectableByMouse
	//                        | Qt::TextSelectableByKeyboard );
	if(url.startsWith("mailto:"))
	{
		displaytext = url.mid(7);
	}
	else
	{
		displaytext = url;
	}
	text = "<a href=\"";
	text.append(url);
	text.append("\">");
	text.append(displaytext);
	text.append("</a>");
	setText(text);
}

