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
	m_statelbl = new QLabel(this);
	m_square.fill( Qt::gray );
	m_statelbl->setPixmap( m_square );
	layout->addWidget( m_statelbl );
	m_textlbl = new QLabel(txtlbl, this);
	layout->addWidget( m_textlbl );
	// to be able to receive drop
	setAcceptDrops(true);
}

void PeerWidget::setRed()
{
	m_square.fill( Qt::red );
	m_statelbl->setPixmap( m_square );
}

void PeerWidget::setGreen()
{
	m_square.fill( Qt::green );
	m_statelbl->setPixmap( m_square );
}

void PeerWidget::setGray()
{
	m_square.fill( Qt::gray );
	m_statelbl->setPixmap( m_square );
}

void PeerWidget::setOrange()
{
	m_square.fill( QColor(255,127,0) );
	m_statelbl->setPixmap( m_square );
}

void PeerWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
		m_dragstartpos = event->pos();
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
	qDebug() << "dragMoveEvent()";
	qDebug() << event->mimeData()->formats();
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

