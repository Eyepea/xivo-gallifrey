/*
XIVO customer information client : popup profile for incoming calls
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
#include <QDebug>
#include "xmlhandler.h"
#include "popup.h"

/*!
 * Basic constructor
 * \param popup Popup widget related to the XML stream
 */
XmlHandler::XmlHandler( Popup *popup )
: m_popup(popup), m_isParsingInfo(false)
{
	qDebug() << "XmlHandler::XmlHandler()";
}

/*!
 * Start of element callback.
 *
 * Detect "info" element and store "type" and "name"
 * attributes.
 */
bool XmlHandler::startElement( const QString & /*namespaceURI*/,
                               const QString & localName,
                               const QString & /*qName*/,
                               const QXmlAttributes & atts )
{
	//qDebug() << "XmlHandler::startElement()" << localName;
	if( localName == "info" )
	{
		m_isParsingInfo = true;
		m_infoType = atts.value("type");
		m_infoName = atts.value("name");
		m_infoValue = "";
	}
	else if( localName == "message" )
	{
		m_isParsingInfo = true;
		m_infoValue = "";
	}
	else
	{
		m_isParsingInfo = false;
	}
	return true;
}

/*!
 * Called when a XML element is closed :
 * use the data stored in attributes to create
 * widgets in the "popup"
 */
bool XmlHandler::endElement( const QString & /*namespaceURI*/,
                             const QString & localName,
                             const QString & /*qName*/)
{
	//qDebug() << "XmlHandler::endElement()" << localName;
	m_isParsingInfo = false;
	if( localName == QString("info") )
	{
		qDebug() << "XmlHandler::endElement()" << m_infoType << m_infoName << m_infoValue;
		if( m_infoType == QString("text") )
		{
			if(m_popup)
				m_popup->addInfoText( m_infoName, m_infoValue );
		}
		else if( m_infoType == QString("url") )
		{
			if(m_popup)
				m_popup->addInfoLink( m_infoName, m_infoValue );
		}
		else if( m_infoType == QString("picture") )
		{
			if(m_popup)
				m_popup->addInfoPicture( m_infoName, m_infoValue );
		}
		else if( m_infoType == "phone" )
		{
			if(m_popup)
				m_popup->addInfoPhone( m_infoName, m_infoValue );
		}
	}
	else if( localName == "message" )
	{
		qDebug() << m_infoValue;
		if(m_popup)
			m_popup->setMessage( m_infoValue );
	}
	else if( localName == QString("profile") )
	{
		if(m_popup)
			m_popup->finishAndShow();
	}
	return true;
}

bool XmlHandler::characters( const QString & ch )
{
	//qDebug() << "chars=" << ch;
	if(m_isParsingInfo)
	{
		m_infoValue.append(ch);
	}
	return true;
}

bool XmlHandler::endDocument()
{
	qDebug() << "XmlHandler::endDocument()";
	//if(m_popup)
	//	m_popup->finishAndShow();
	return true;
}

// QXmlErrorHandler
bool XmlHandler::warning( const QXmlParseException & exception )
{
	qDebug() << "XmlHandler::warning() " << exception.message();
	return true;
}

bool XmlHandler::error( const QXmlParseException & exception )
{
	qDebug() << "XmlHandler::error() " << exception.message();
	return true;
}

bool XmlHandler::fatalError( const QXmlParseException & exception )
{
	qDebug() << "XmlHandler::fatalError() " << exception.message();
	return true;
}

