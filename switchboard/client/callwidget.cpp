#include <QHBoxLayout>
#include <QApplication>
#include <QLabel>
#include <QMouseEvent>
#include <QDebug>
#include "callwidget.h"

CallWidget::CallWidget(const QString & channelme,
		       const QString & action,
		       const QString & time,
		       const QString & direction,
		       const QString & channelpeer,
		       const QString & exten,
                       QWidget * parent)
: QWidget(parent)
{
	QHBoxLayout * layout = new QHBoxLayout(this);

// 	m_callerid = callerid;
// 	m_calleridname = calleridname;
// 	m_channel = channel;

//	qDebug() << "spacing" << layout->spacing()
//	         << ", margin" << layout->margin();
//	layout->setSpacing(0);
	//layout->setMargin(0);

	m_lbl_channelme = new QLabel(channelme, this);
	layout->addWidget(m_lbl_channelme);
	m_lbl_action = new QLabel(action, this);
	layout->addWidget(m_lbl_action);
	m_lbl_time = new QLabel(time, this);
	layout->addWidget(m_lbl_time);
	m_lbl_direction = new QLabel(direction, this);
	layout->addWidget(m_lbl_direction);
	m_lbl_channelpeer = new QLabel(channelpeer, this);
	layout->addWidget(m_lbl_channelpeer);
	m_lbl_exten = new QLabel(exten, this);
	layout->addWidget(m_lbl_exten);
}

void CallWidget::updateWidget(const QString & action,
			      const QString & time,
			      const QString & direction,
			      const QString & channelpeer,
			      const QString & exten)
{
	m_lbl_action->setText(action);
	m_lbl_time->setText(time);
	m_lbl_direction->setText(direction);
	m_lbl_channelpeer->setText(channelpeer);
	m_lbl_exten->setText(exten);

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

