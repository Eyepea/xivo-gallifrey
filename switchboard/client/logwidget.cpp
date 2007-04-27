#include <QVBoxLayout>
#include "logwidget.h"

LogWidget::LogWidget(QWidget * parent)
: QWidget(parent)
{
	m_layout = new QVBoxLayout(this);
}

void LogWidget::clear()
{
	QLayoutItem * child;
	while ((child = m_layout->takeAt(0)) != 0)
	{
		delete child;
	}
}

