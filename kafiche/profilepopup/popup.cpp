#include <QPixmap>
#include <QLabel>
#include <QUrl>
#include <QDebug>
#include "popup.h"
#include "xmlhandler.h"
#include "remotepicwidget.h"
#include "urllabel.h"

// Test - to be removed.
Popup::Popup(QWidget *parent)
: QWidget(parent)
{
	QPixmap *face = new QPixmap( "portrait.jpg" );
	QLabel *lbl = new QLabel(this);
	lbl->setPixmap( *face );
	m_inputstream = 0;
}

/*!
 * This constructor init all XML objects and connect signals
 * to slots.
 * \param inputstream	inputstream to read the XML
 * \param sessionid		sessionid to check incoming connection to
 * \param parent		parent widget
 */
Popup::Popup(QIODevice *inputstream, const QString & sessionid, QWidget *parent)
: QWidget(parent), m_inputstream(inputstream),
  m_xmlInputSource(inputstream), m_handler(this),
  m_sessionid(sessionid)
{
	qDebug() << "Popup(" << inputstream << ")";
	setAttribute(Qt::WA_DeleteOnClose);
	m_reader.setContentHandler(&m_handler);
	m_reader.setErrorHandler(&m_handler);
	connect( inputstream, SIGNAL(readyRead()),
	         this, SLOT(streamNewData()) );
	connect( inputstream, SIGNAL(aboutToClose()),
	         this, SLOT(streamAboutToClose()) );
	connect( inputstream, SIGNAL(disconnected()),
	         this, SLOT(socketDisconnected()) );
	connect( inputstream, SIGNAL(error(QAbstractSocket::SocketError)),
	         this, SLOT(socketError(QAbstractSocket::SocketError)));
	m_parsingStarted = false;
	m_vlayout = new QVBoxLayout(this);
	QLabel * title = new QLabel(tr("Incoming call"), this);
	title->setAlignment(Qt::AlignHCenter);
	m_vlayout->addWidget(title);
}

void Popup::addInfoText(const QString & name, const QString & value)
{
	QLabel * lblname = new QLabel(name, this);
	QLabel * lblvalue = new QLabel(value, this);
	lblvalue->setTextInteractionFlags( Qt::TextSelectableByMouse
	                                  | Qt::TextSelectableByKeyboard );
	QHBoxLayout * hlayout = new QHBoxLayout();
	hlayout->addWidget(lblname);
	hlayout->addWidget(lblvalue);
	m_vlayout->addLayout(hlayout);
}

void Popup::addInfoPhone(const QString & name, const QString & value)
{
	// at the moment we are not doing anything special...
	addInfoText(name, value);
}

void Popup::addInfoLink(const QString & name, const QString & value)
{
	QLabel * lblname = new QLabel(name, this);
	UrlLabel * lblvalue = new UrlLabel(value, this);
	QHBoxLayout * hlayout = new QHBoxLayout();
	hlayout->addWidget(lblname);
	hlayout->addWidget(lblvalue);
	m_vlayout->addLayout(hlayout);
}

void Popup::addInfoPicture(const QString & name, const QString & value)
{
	qDebug() << "Popup::addInfoPicture()" << value;
	QUrl url(value);
	//QUrl url = QUrl::fromEncoded(value);
	// TODO: faire un widget special qui bouffe des Images HTTP ?
	if(url.scheme() != QString("http"))
	{
		QLabel *lbl = new QLabel( name, this );
		QPixmap *face = new QPixmap( value );
		// TODO: connect a signal to close() ?
		lbl->setPixmap( *face );
		m_vlayout->addWidget( lbl );
	}
	else
	{
		RemotePicWidget * pic = new RemotePicWidget( name, value, this );
		m_vlayout->addWidget( pic );
	}
}

// === SLOTS ===
void Popup::streamNewData()
{
	bool b = false;
	qDebug() << "Popup::streamNewData()";
	qDebug() << m_inputstream->bytesAvailable() << "bytes available";
	if(m_parsingStarted)
	{
		b = m_reader.parseContinue();
	}
	else
	{
		b = m_reader.parse(&m_xmlInputSource, true);
		m_parsingStarted = b;
	}
	qDebug() << "parse returned" << b;
}

void Popup::streamAboutToClose()
{
	qDebug() << "Popup::streamAboutToClose()";
	finishAndShow();
}

void Popup::socketDisconnected()
{
	qDebug() << "Popup::socketDisconnected()";
	/* finish the parsing */
	m_reader.parseContinue();
}

void Popup::socketError(QAbstractSocket::SocketError err)
{
	qDebug() << "Popup::socketError()" << err;
}

//
void Popup::finishAndShow()
{
	qDebug() << "Popup::finishAndShow()";
	//dumpObjectInfo();
	dumpObjectTree();
	// ...
	show();
}

void Popup::closeEvent(QCloseEvent * event)
{
	qDebug() << "Popup::closeEvent(" << event << ")";
}

