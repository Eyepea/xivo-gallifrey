#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
#include "callwidget.h"


CallWidget::CallWidget(QWidget * parent)
: QWidget(parent)
{
	QHBoxLayout * layout = new QHBoxLayout(this);

	qDebug() << "spacing" << layout->spacing()
	         << ", margin" << layout->margin();
	//layout->setSpacing(0);
	layout->setMargin(0);

	QLabel * lblid = new QLabel("1", this);
	//lblid->setBackgroundRole( QPalette::Highlight );
	lblid->setAutoFillBackground( true );
	layout->addWidget(lblid);

	QLabel * lblcaller = new QLabel("caller", this);
	layout->addWidget(lblcaller);

	QLabel * lblstate = new QLabel("On Hold", this);
	layout->addWidget(lblstate);
}

