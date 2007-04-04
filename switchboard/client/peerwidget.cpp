#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QMouseEvent>
#include <QApplication>
#include <QDebug>
#include "peerwidget.h"
#include "switchboardengine.h"

PeerWidget::PeerWidget(const QString & txtlbl, SwitchBoardEngine * engine,
                       QWidget * parent, int size)
: QWidget(parent), m_square(size,size), m_engine(engine)
{
	QHBoxLayout * layout = new QHBoxLayout(this);
	layout->setSpacing(2);
	layout->setMargin(2);
	m_statelbl = new QLabel(this);
	m_availlbl = new QLabel(this);
	m_square.fill( Qt::gray );
	m_statelbl->setPixmap( m_square );
	m_availlbl->setPixmap( m_square );
	layout->addWidget( m_statelbl, 0, Qt::AlignLeft );
	layout->addWidget( m_availlbl, 0, Qt::AlignLeft );
	m_textlbl = new QLabel(txtlbl, this);
	layout->addWidget( m_textlbl, 0, Qt::AlignLeft );
	QLabel * dummy = new QLabel(this);
	layout->addWidget( dummy, 1, Qt::AlignLeft );
	// to be able to receive drop
	setAcceptDrops(true);
}

void PeerWidget::setRed(int n)
{
	m_square.fill( Qt::red );
	if(n == 0)
	  m_statelbl->setPixmap( m_square );
	else
	  m_availlbl->setPixmap( m_square );
}

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

void PeerWidget::setGreen(int n)
{
	m_square.fill( Qt::green );
	if(n == 0)
	  m_statelbl->setPixmap( m_square );
	else
	  m_availlbl->setPixmap( m_square );
}

void PeerWidget::setGray(int n)
{
	m_square.fill( Qt::gray );
	if(n == 0)
	  m_statelbl->setPixmap( m_square );
	else
	  m_availlbl->setPixmap( m_square );
}

void PeerWidget::setBlue(int n)
{
	m_square.fill( Qt::blue );
	if(n == 0)
	  m_statelbl->setPixmap( m_square );
	else
	  m_availlbl->setPixmap( m_square );
}

void PeerWidget::setCyan(int n)
{
	m_square.fill( Qt::cyan );
	if(n == 0)
	  m_statelbl->setPixmap( m_square );
	else
	  m_availlbl->setPixmap( m_square );
}

void PeerWidget::setYellow(int n)
{
	m_square.fill( Qt::yellow );
	if(n == 0)
	  m_statelbl->setPixmap( m_square );
	else
	  m_availlbl->setPixmap( m_square );
}

void PeerWidget::setOrange(int n)
{
	m_square.fill( QColor(255,127,0) );
	if(n == 0)
	  m_statelbl->setPixmap( m_square );
	else
	  m_availlbl->setPixmap( m_square );
}

void PeerWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
		m_dragstartpos = event->pos();
	else if (event->button() == Qt::RightButton)
		qDebug() << "depending on what has been left-cliked on the left ...";
}

void PeerWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (!(event->buttons() & Qt::LeftButton))
		return;
	if ((event->pos() - m_dragstartpos).manhattanLength()
	    < QApplication::startDragDistance())
		return;

	QDrag *drag = new QDrag(this);
	QMimeData *mimeData = new QMimeData;
	//mimeData->setData("text/plain", m_textlbl->text().toAscii());
	mimeData->setText(m_textlbl->text());
	drag->setMimeData(mimeData);

	Qt::DropAction dropAction = drag->start(Qt::CopyAction | Qt::MoveAction);
	qDebug() << "dropAction=" << dropAction;
}

void PeerWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	qDebug() << "mouseDoubleClickEvent" << event;
	//	m_engine->hangUp(m_textlbl->text());

	// selects this to be monitored otherwise
        if(event->button() == Qt::LeftButton)
		m_engine->selectAsMonitored(m_textlbl->text());
}

void PeerWidget::dragEnterEvent(QDragEnterEvent *event)
{
	qDebug() << "dragEnterEvent()";
	qDebug() << event->mimeData()->formats();
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
	QString to = m_textlbl->text();
	qDebug() << "dropEvent() :" << from << "on" << to;
	qDebug() << " possibleActions=" << event->possibleActions();
	qDebug() << " proposedAction=" << event->proposedAction();
	switch(event->proposedAction())
	{
	case Qt::CopyAction:
		// transfer the call to the peer "to"
		event->acceptProposedAction();
		m_engine->originateCall(from, to);
		break;
	case Qt::MoveAction:
		event->acceptProposedAction();
		m_engine->transferCall(from, to);
		break;
	default:
		break;
	}
}

