#include <QHBoxLayout>
#include <QApplication>
#include <QLabel>
#include <QMouseEvent>
#include <QDebug>
#include "callstackwidget.h"
#include "callwidget.h"

CallWidget::CallWidget(QWidget * parent)
: QWidget(parent)
{
	QHBoxLayout * layout = new QHBoxLayout(this);
	m_lbl_time = new QLabel(this);
	layout->addWidget(m_lbl_time);
}

CallWidget::CallWidget(const QString & tomonitor,
                       QWidget * parent)
: QWidget(parent)
{
	QHBoxLayout * layout = new QHBoxLayout(this);
	if (tomonitor == QString(""))
		m_lbl_time = new QLabel("No phone monitored", this);
	else
		m_lbl_time = new QLabel("Phone monitored = " + tomonitor, this);
	layout->addWidget(m_lbl_time);
}

CallWidget::CallWidget(const QString & channelme,
		       const QString & action,
		       const int & time,
		       const QString & direction,
		       const QString & channelpeer,
		       const QString & exten,
                       QWidget * parent)
: QWidget(parent), m_square(16,16)
{
	QHBoxLayout * layout = new QHBoxLayout(this);

// 	m_callerid = callerid;
// 	m_calleridname = calleridname;
 	m_channelme = channelme;

//	qDebug() << "spacing" << layout->spacing()
//	         << ", margin" << layout->margin();
//	layout->setSpacing(0);
	//layout->setMargin(0);

	m_lbl_action = new QLabel(this);
	layout->addWidget(m_lbl_action, 0, Qt::AlignLeft );
	setActionPixmap(action);
	m_lbl_time = new QLabel("[" + QString::number(time/60) + " min " + QString::number(time%60) + " s]",
				this);
	layout->addWidget(m_lbl_time, 0, Qt::AlignLeft );
	m_lbl_direction = new QLabel(direction, this);
	layout->addWidget(m_lbl_direction, 0, Qt::AlignLeft );
	m_lbl_channelpeer = new QLabel(channelpeer, this);
	layout->addWidget(m_lbl_channelpeer, 0, Qt::AlignLeft );
	m_lbl_exten = new QLabel(exten, this);
	layout->addWidget(m_lbl_exten, 0, Qt::AlignLeft );
	QLabel * dummy = new QLabel(this);
	layout->addWidget(dummy, 1, Qt::AlignLeft );
	//setAcceptDrops(true);
}

void CallWidget::updateWidget(const QString & action,
			      const int & time,
			      const QString & direction,
			      const QString & channelpeer,
			      const QString & exten)
{
	qDebug() << this << "updateWidget";
	//m_lbl_action->setText(action);
	setActionPixmap(action);
	m_lbl_time->setText("[" + QString::number(time/60) + " min " + QString::number(time%60) + " s]");
	m_lbl_direction->setText(direction);
	m_lbl_channelpeer->setText(channelpeer);
	m_lbl_exten->setText(exten);
}

void CallWidget::setActionPixmap(const QString & action)
{
	if(action == QString("Calling"))
		m_square.fill( Qt::yellow );
	else if(action == QString("Ringing"))
		m_square.fill( Qt::cyan );
	else if(action == QString("On the phone"))
		m_square.fill( Qt::red );
	else
		m_square.fill( Qt::gray );
	m_lbl_action->setPixmap( m_square );
}

void CallWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		m_dragstartpos = event->pos();
/*
		if(m_lbl_channelpeer)
		{
			qDebug() << "I'm selecting this one for future use :" << m_channelme
			         << m_lbl_channelpeer->text();
		}
*/
	}
}

void CallWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (!(event->buttons() & Qt::LeftButton))
		return;
	if ((event->pos() - m_dragstartpos).manhattanLength()
	    < QApplication::startDragDistance())
		return;

	qDebug() << "CallWidget::mouseMoveEvent() starting DRAG" << m_channelme ;

	QDrag *drag = new QDrag(this);
	QMimeData *mimeData = new QMimeData();
	mimeData->setText(/*"test"*/ m_channelme);
	drag->setMimeData(mimeData);

	Qt::DropAction dropAction = drag->start(Qt::CopyAction | Qt::MoveAction);
	qDebug() << "dropAction =" << dropAction;
}

/*
void CallWidget::dragEnterEvent(QDragEnterEvent *event)
{
	qDebug() << "dragEnterEvent()";
// 	qDebug() << event->mimeData()->formats();
// 	if(event->mimeData()->hasText())
// 	{
// 		if(event->proposedAction() & (Qt::CopyAction|Qt::MoveAction))
// 			event->acceptProposedAction();
// 	}
}
*/

void CallWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	qDebug() << "mouseDoubleClickEvent" << event;
	if (event->button() == Qt::RightButton) {
// 		qDebug() << this << this->parentWidget();
// 		qDebug() << "I want to Hangup" << m_channelme << m_lbl_channelpeer->text();
		CallStackWidget * csw = (CallStackWidget *) this->parentWidget();
		csw->hupchan(m_channelme);
	}
}


/*
void CallWidget::setCallerId(const QString & callerid)
{
	m_callerid = callerid;
}

void CallWidget::setCallerIdName(const QString & calleridname)
{
	m_calleridname = calleridname;
}

void CallWidget::setChannel(const QString & channel)
{
	m_channel = channel;
}
*/

const QString & CallWidget::channel() const
{
	return m_channelme;
}

