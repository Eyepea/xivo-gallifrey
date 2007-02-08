/* Author : Thomas Bernard */
#include <QDebug>
#include "xmlhandler.h"
#include "popup.h"

XmlHandler::XmlHandler( Popup *popup )
: m_popup(popup), m_isParsingInfo(false)
{
	qDebug() << "XmlHandler::XmlHandler()";
}

/*
 * bool QXmlContentHandler::startElement ( const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts )
 */
bool XmlHandler::startElement( const QString & /*namespaceURI*/,
                               const QString & localName,
                               const QString & /*qName*/,
                               const QXmlAttributes & atts )
{
	//int i;
	qDebug() << "XmlHandler::startElement(" << localName << ")";
	//for(i = 0; i < atts.count(); i++)
	//{
	//	qDebug() << atts.localName(i) << "=" << atts.value(i);
	//}
	if( localName == QString("info") )
	{
		m_isParsingInfo = true;
		m_infoType = atts.value("type");
		m_infoName = atts.value("name");
		m_infoValue = "";
	}
	else
	{
		m_isParsingInfo = false;
	}
	return true;
}

bool XmlHandler::endElement( const QString & /*namespaceURI*/,
                             const QString & localName,
                             const QString & /*qName*/)
{
	qDebug() << "endElement(" << localName << ")";
	m_isParsingInfo = false;
	if( localName == QString("info") )
	{
		qDebug() << m_infoType << m_infoName << m_infoValue;
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
/*	else if( localName == QString("profile") )
	{
		if(m_popup)
			m_popup->finishAndShow();
	}*/
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
	if(m_popup)
		m_popup->finishAndShow();
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

