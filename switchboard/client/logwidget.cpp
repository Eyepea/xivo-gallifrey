#include <QVBoxLayout>
#include <QLayoutItem>
#include <QLabel>
#include <QDebug>
#include "logwidget.h"
#include "logeltwidget.h"

LogWidget::LogWidget(QWidget * parent)
: QWidget(parent)
{
	m_layout = new QVBoxLayout(this);
	m_layout->setSpacing(2);
#if 0
	LogEltWidget * e = new LogEltWidget("peer test",
	                                    LogEltWidget::InCall,
										QDateTime::currentDateTime(),
										42,
										this);
	m_layout->addWidget(e);
	addElement("peer test2", LogEltWidget::InCall, QDateTime::currentDateTime(), 0);
	addElement("peer test2", LogEltWidget::InCall, QDateTime::currentDateTime(), 0);
	addElement("peer test3", LogEltWidget::OutCall, QDateTime::currentDateTime(),
	           43);
#endif
	m_layout->addStretch(1);
}

void LogWidget::addElement(const QString & peer, LogEltWidget::Direction d,
                           const QDateTime & dt, int duration)
{
	//qDebug() << "LogWidget::addElement()" << peer << d << dt << duration;
	int i, index = 0;
	for(i = 0; i<m_layout->count(); i++)
	{
		QWidget * widget = m_layout->itemAt( i )->widget();
		if(widget)
		{
			LogEltWidget * logelt = qobject_cast<LogEltWidget *>(widget);
			if(logelt)
			{
				if(dt == logelt->dateTime()
				   && peer == logelt->peer()
				   && d == logelt->direction())
					return;
				else if(dt >= logelt->dateTime())
					break;
				index = i + 1;
			}
		}
	}
	LogEltWidget * e = new LogEltWidget(peer, d, dt, duration, this);
	m_layout->insertWidget(index, e);
}

void LogWidget::clear()
{
	qDebug() << "LogWidget::clear()";
	QLayoutItem * child;
	while ((child = m_layout->takeAt(0)) != 0)
	{
		delete child;
	}
}

void LogWidget::addLogEntry(const QDateTime & dt, int duration,
                            const QString & peer, int d)
{
	// TODO: manage the list !
	addElement(peer, (LogEltWidget::Direction)d, dt, duration);
}

