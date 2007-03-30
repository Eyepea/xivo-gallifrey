#include <QHBoxLayout>
#include <QApplication>
#include <QLabel>
#include <QMouseEvent>
#include <QDebug>
#include "callwidget.h"


CallWidget::CallWidget(const QString & callerid,
                       const QString & calleridname,
					   const QString & channel,
                       QWidget * parent)
: QWidget(parent)
{
	QHBoxLayout * layout = new QHBoxLayout(this);

	m_callerid = callerid;
	m_calleridname = calleridname;
	m_channel = channel;

//	qDebug() << "spacing" << layout->spacing()
//	         << ", margin" << layout->margin();
	//layout->setSpacing(0);
	//layout->setMargin(0);

	QLabel * lblid = new QLabel(m_callerid, this);
	//lblid->setBackgroundRole( QPalette::Highlight );
	lblid->setAutoFillBackground( true );
	layout->addWidget(lblid);

	QLabel * lblcaller = new QLabel(m_calleridname, this);
	layout->addWidget(lblcaller);

	QLabel * lblstate = new QLabel("On Hold", this);
	layout->addWidget(lblstate);
}

void CallWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
		m_dragstartpos = event->pos();
}

void CallWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (!(event->buttons() & Qt::LeftButton))
		return;
	if ((event->pos() - m_dragstartpos).manhattanLength()
	    < QApplication::startDragDistance())
		return;
	
	QDrag *drag = new QDrag(this);
	QMimeData *mimeData = new QMimeData();
	mimeData->setText(/*"test"*/ m_channel);
	drag->setMimeData(mimeData);

	Qt::DropAction dropAction = drag->start(Qt::CopyAction | Qt::MoveAction);
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

