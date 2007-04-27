#include <QHBoxLayout>
#include <QLabel>
#include "logeltwidget.h"

LogEltWidget::LogEltWidget( const QString & peer, Direction d,
                            const QDateTime & dt, int duration,
							QWidget * parent )
: QWidget(parent)
{
	QHBoxLayout * layout = new QHBoxLayout( this );
	QLabel * lblpeer = new QLabel( peer, this );
	layout->addWidget(lblpeer);

	QLabel * lbldir = new QLabel( this );
	lbldir->setText((d == OutCall)?"<=":"=>");
	layout->addWidget(lbldir);

	QLabel * lbldt = new QLabel( dt.toString(Qt::SystemLocaleDate), this );
	layout->addWidget(lbldt);

	QLabel * lblduration = new QLabel( this );
	lblduration->setText(QString::number(duration));
	layout->addWidget(lblduration);

	layout->addStretch(1);
}

