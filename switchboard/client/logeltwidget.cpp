#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QDebug>
#include "logeltwidget.h"

LogEltWidget::LogEltWidget( const QString & peer, Direction d,
                            const QDateTime & dt, int duration,
							QWidget * parent )
: QWidget(parent), m_dateTime(dt), m_peer(peer), m_direction(d)
{
//	qDebug() << "  LogEltWidget::LogEltWidget()" << peer << d << dt << duration << parent;
	QHBoxLayout * layout = new QHBoxLayout( this );
	layout->setMargin(0);

	QLabel * lbldt = new QLabel( dt.toString(Qt::SystemLocaleDate), this );
	layout->addWidget(lbldt);

	QLabel * lblduration = new QLabel( this );
	lblduration->setText(QString::number(duration) + "s");
	layout->addWidget(lblduration);

	QLabel * lblpeer = new QLabel( peer, this );
	layout->addWidget(lblpeer);

	/*	QLabel * lbldir = new QLabel( this );
	  lbldir->setText((d == OutCall)?"<=":"=>");
	  layout->addWidget(lbldir);
	*/

	layout->addStretch(1);

	m_dialAction = new QAction( tr("&Dial"), this );
	m_dialAction->setStatusTip( tr("Dial back") );
	connect( m_dialAction, SIGNAL(triggered()),
	         this, SLOT(callBackPeer()) );
}

void LogEltWidget::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu contextMenu(this);
	contextMenu.addAction( m_dialAction );
	contextMenu.exec(event->globalPos());
}

void LogEltWidget::callBackPeer()
{
	emitDial(m_peer);
}

