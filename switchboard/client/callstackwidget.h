#ifndef __CALLSTACKWIDGET_H__
#define __CALLSTACKWIDGET_H__
#include <QWidget>
#include <QString>

class QVBoxLayout;

class CallStackWidget : public QWidget
{
	Q_OBJECT
public:
	CallStackWidget(QWidget * parent = 0);
public slots:
	void addCall(const QString &callerId,
	             const QString &callerIdName,
				 const QString &channel,
				 const QString &ext);
private:
	QVBoxLayout * m_layout;
};

#endif

