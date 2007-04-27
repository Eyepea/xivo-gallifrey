#ifndef __LOGWIDGET_H__
#define __LOGWIDGET_H__

#include <QWidget>

class QVBoxLayout;

class LogWidget : public QWidget
{
public:
	LogWidget(QWidget * parent = 0);
	void clear();
private:
	QVBoxLayout * m_layout;
};

#endif

