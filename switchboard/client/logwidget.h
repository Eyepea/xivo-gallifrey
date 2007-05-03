#ifndef __LOGWIDGET_H__
#define __LOGWIDGET_H__

#include <QWidget>
#include "logeltwidget.h"

class QVBoxLayout;

class LogWidget : public QWidget
{
	Q_OBJECT
public:
	LogWidget(QWidget * parent = 0);
	void addElement(const QString & peer, LogEltWidget::Direction d,
	                const QDateTime & dt, int duration);
public slots:
	void clear();
	void addLogEntry(const QDateTime & dt, int, const QString &, int);
private:
	QVBoxLayout * m_layout;
};

#endif

