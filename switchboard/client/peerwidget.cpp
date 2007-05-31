/* $Id$ */
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QMouseEvent>
#include <QApplication>
#include <QMenu>
#include <QDebug>
#include "peerwidget.h"
#include "switchboardengine.h"
#include "switchboardwindow.h"

PeerWidget::PeerWidget(const QString & id, const QString & name,
                       QWidget * parent/*, int size*/)
: QWidget(parent)/*, m_square(size,size)*/, m_id(id), m_name(name),
m_phone_green(":/phone-green.png"), m_phone_red(":/phone-red.png"),
m_phone_orange(":/phone-orange.png"), m_phone_gray(":/phone-grey.png"),
m_phone_yellow(":/phone-yellow.png"), m_phone_blue(":/phone-blue.png"),
m_person_green(":/personal-green.png"), m_person_red(":/personal-red.png"),
m_person_orange(":/personal-orange.png"), m_person_gray(":/personal-grey.png"),
m_person_yellow(":/personal-yellow.png"), m_person_blue(":/personal-blue.png")
{
	QHBoxLayout * layout = new QHBoxLayout(this);
	layout->setSpacing(2);
	layout->setMargin(2);
	m_statelbl = new QLabel(this);
	m_availlbl = new QLabel(this);
	m_statelbl->setPixmap( m_phone_gray );
	m_availlbl->setPixmap( m_person_gray );
	layout->addWidget( m_statelbl, 0, Qt::AlignLeft );
	layout->addWidget( m_availlbl, 0, Qt::AlignLeft );
	m_textlbl = new QLabel(/*m_id + "/" +*/ m_name, this);
	// set TextInteraction Flags so the mouse clicks are not catched by the
	// QLabel widget
	m_textlbl->setTextInteractionFlags( Qt::NoTextInteraction );
	layout->addWidget( m_textlbl, 0, Qt::AlignLeft );
	layout->addStretch(1);
	// to be able to receive drop
	setAcceptDrops(true);
	m_removeAction = new QAction( tr("&Remove"), this);
	m_removeAction->setStatusTip( tr("Remove this peer from the panel") );
	connect( m_removeAction, SIGNAL(triggered()),
	         this, SLOT(removeFromPanel()) );
	m_dialAction = new QAction( tr("&Dial"), this);
	m_dialAction->setStatusTip( tr("Dial this number") );
	connect( m_dialAction, SIGNAL(triggered()),
	         this, SLOT(dial()) );
	PeerChannel ch("SIP/666-123", "Glop", "666");
	m_channels << ch;
}

void PeerWidget::setRed(int n)
{
	//m_square.fill( Qt::red );
	if(n == 0)
	  m_statelbl->setPixmap( m_phone_red/*m_square*/ );
	else
	  m_availlbl->setPixmap( m_person_red/*m_square*/ );
}

#if 0
void PeerWidget::setBlack(int n)
{
	m_square.fill( Qt::black );
	if(n == 0)
	  m_statelbl->setPixmap( m_square );
	else
	  m_availlbl->setPixmap( m_square );
}

void PeerWidget::setDarkGreen(int n)
{
	m_square.fill( Qt::darkGreen );
	if(n == 0)
	  m_statelbl->setPixmap( m_square );
	else
	  m_availlbl->setPixmap( m_square );
}
#endif 

void PeerWidget::setGreen(int n)
{
	//m_square.fill( Qt::green );
	if(n == 0)
	  m_statelbl->setPixmap( m_phone_green/*m_square*/ );
	else
	  m_availlbl->setPixmap( m_person_green/*m_square*/ );
}

void PeerWidget::setGray(int n)
{
	//m_square.fill( Qt::gray );
	if(n == 0)
	  m_statelbl->setPixmap( m_phone_gray/*m_square*/ );
	else
	  m_availlbl->setPixmap( m_person_gray/*m_square*/ );
}

void PeerWidget::setBlue(int n)
{
	//m_square.fill( Qt::blue );
	if(n == 0)
	  m_statelbl->setPixmap( m_phone_blue/*m_square*/ );
	else
	  m_availlbl->setPixmap( m_person_blue/*m_square*/ );
}

#if 0
void PeerWidget::setCyan(int n)
{
	m_square.fill( Qt::cyan );
	if(n == 0)
	  m_statelbl->setPixmap( m_square );
	else
	  m_availlbl->setPixmap( m_square );
}
#endif

void PeerWidget::setYellow(int n)
{
	//m_square.fill( Qt::yellow );
	if(n == 0)
	  m_statelbl->setPixmap( m_phone_yellow/*m_square*/ );
	else
	  m_availlbl->setPixmap( m_person_yellow/*m_square*/ );
}

void PeerWidget::setOrange(int n)
{
	//m_square.fill( QColor(255,127,0) );
	if(n == 0)
	  m_statelbl->setPixmap( m_phone_orange/*m_square*/ );
	else
	  m_availlbl->setPixmap( m_person_orange/*m_square*/ );
}

void PeerWidget::removeFromPanel()
{
	qDebug() << "PeerWidget::removeFromPanel()" << m_id;
	doRemoveFromPanel( m_id );
}

void PeerWidget::dial()
{
	qDebug() << "PeerWidget::dial()" << m_id;
	emitDial( m_id );
}

void PeerWidget::mousePressEvent(QMouseEvent *event)
{
	//qDebug() << "PeerWidget::mousePressEvent(QMouseEvent *event)";
	if (event->button() == Qt::LeftButton)
		m_dragstartpos = event->pos();
	//else if (event->button() == Qt::RightButton)
	//	qDebug() << "depending on what has been left-cliked on the left ...";
}

void PeerWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (!(event->buttons() & Qt::LeftButton))
		return;
	if ((event->pos() - m_dragstartpos).manhattanLength()
	    < QApplication::startDragDistance())
		return;

	//qDebug() << "PeerWidget::mouseMoveEvent() startDrag";
	QDrag *drag = new QDrag(this);
	QMimeData *mimeData = new QMimeData;
	qDebug() << "here" << m_id;
	mimeData->setText(m_id/*m_textlbl->text()*/);
	drag->setMimeData(mimeData);

	Qt::DropAction dropAction = drag->start(Qt::CopyAction | Qt::MoveAction);
	//qDebug() << "PeerWidget::mouseMoveEvent : dropAction=" << dropAction;
}

#if 0
void PeerWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	qDebug() << "mouseDoubleClickEvent" << event;
	//	m_engine->hangUp(m_textlbl->text());

	if(event->button() == Qt::LeftButton)
	{
		//qDebug() << m_textlbl->text();
	}
}
#endif

void PeerWidget::dragEnterEvent(QDragEnterEvent *event)
{
	qDebug() << "PeerWidget::dragEnterEvent()" << event->mimeData()->formats();
	if(event->mimeData()->hasText())
	{
		if(event->proposedAction() & (Qt::CopyAction|Qt::MoveAction))
			event->acceptProposedAction();
	}
}

void PeerWidget::dragMoveEvent(QDragMoveEvent *event)
{
	//	qDebug() << "dragMoveEvent()";
	//	qDebug() << event->mimeData()->formats();
	if(event->mimeData()->hasText())
	{
		if(event->proposedAction() & (Qt::CopyAction|Qt::MoveAction))
			event->acceptProposedAction();
	}
}

void PeerWidget::dropEvent(QDropEvent *event)
{
	QString from = event->mimeData()->text();
	QString to = m_id;
	qDebug() << "dropEvent() :" << from << "on" << to;
	qDebug() << " possibleActions=" << event->possibleActions();
	qDebug() << " proposedAction=" << event->proposedAction();
	switch(event->proposedAction())
	{
	case Qt::CopyAction:
		// transfer the call to the peer "to"
		event->acceptProposedAction();
		if(from.indexOf('c') == 0)         // 'c/' => channel
			transferCall(from, to);
		else if(from.indexOf('p') == 0)    // 'p/' => peer
			originateCall(from, to);
		break;
	case Qt::MoveAction:
		event->acceptProposedAction();
		transferCall(from, to);
		break;
	default:
		qDebug() << "Unrecognized action";
		break;
	}
}

void PeerWidget::contextMenuEvent(QContextMenuEvent * event)
{
	QMenu contextMenu(this);
	//contextMenu.addAction("&Test");
	contextMenu.addAction(m_dialAction);
	// add remove action only if we are in the central widget.
	qDebug() << parentWidget();
	if(parentWidget())
	{
		qDebug() << parentWidget()->objectName();
		qDebug() << parentWidget()->metaObject();
		qDebug() << &SwitchBoardWindow::staticMetaObject;
		qDebug() << parentWidget()->metaObject()->className();
		qDebug() << (&SwitchBoardWindow::staticMetaObject == parentWidget()->metaObject());
	}
	if(parentWidget() && (&SwitchBoardWindow::staticMetaObject == parentWidget()->metaObject()))
		contextMenu.addAction(m_removeAction);
	if( !m_channels.empty() )
	{
		QMenu * interceptMenu = new QMenu( tr("&Intercept"), &contextMenu );
		QListIterator<PeerChannel> i(m_channels);
		while(i.hasNext())
		{
			const PeerChannel & channel = i.next();
			//interceptMenu->addAction("0140000000", blah, SLOT(intercept()));
			//interceptMenu->addAction("0666666666");
			interceptMenu->addAction(channel.otherPeer(),
			                         &channel, SLOT(intercept()));
		}
		contextMenu.addMenu(interceptMenu);
	}
	contextMenu.exec(event->globalPos());
}

