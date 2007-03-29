#ifndef __CALLSTACKWIDGET_H__
#define __CALLSTACKWIDGET_H__
#include <QWidget>
#include <QString>

class QVBoxLayout;

class CallStackWidget : public QWidget
{
public:
	CallStackWidget(QWidget * parent = 0);
	void addCall(const QString &);	// il y aura d'autre params :)
private:
	QVBoxLayout * m_layout;
};

#endif

