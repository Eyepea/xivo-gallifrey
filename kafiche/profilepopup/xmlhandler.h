#ifndef __XMLHANDLER_H__
#define __XMLHANDLER_H__

#include <QXmlDefaultHandler>

class Popup;

/*! \brief for XML Profile parsing
 *
 * This class implements QXmlContentHandler and QXmlErrorHandler.
 * The Popup related object is filled with corresponding data
 * to display.
 */
class XmlHandler: public QXmlDefaultHandler
{
public:
	//! Constructor
	XmlHandler( Popup * popup=0 );
private:
	// The following methods are inherited from QXmlContentHandler
	//! for element declaration and attributes handling
	bool startElement( const QString & namespaceURI,
	                   const QString & localName,
	                   const QString & qName,
	                   const QXmlAttributes & atts );
	//! allways called at the close of an element.
	bool endElement( const QString & namespaceURI,
	                 const QString & localName,
	                 const QString & qName );
	//! used to handle characters data within elements.
	bool characters( const QString & ch );
	//! called when XML parsing is finished
	bool endDocument();

	// The following methods are inherited from QXmlErrorHandler
	//! called by the XML parser for warnings
	bool warning( const QXmlParseException & exception );
	//! called by the XML parser for errors
	bool error( const QXmlParseException & exception );
	//! called by the XML parser for fatal errors
	bool fatalError( const QXmlParseException & exception );
private:
	Popup * m_popup;		//!< pointer to the Popup object to fill with data
	bool m_isParsingInfo;	//!< parsing state : true if we are currently parsing a \<info\> element
	QString m_infoType;		//!< type attribute of the current \<info\> element
	QString m_infoName;		//!< name attribute of the current \<info\> element
	QString m_infoValue;	//!< character value of the current \<info\> element
};

#endif

