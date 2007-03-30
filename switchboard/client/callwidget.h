#ifndef __CALLWIDGET_H__
#define __CALLWIDGET_H__
#include <QWidget>

class CallWidget : public QWidget
{
public:
	CallWidget(const QString & callerid,
	           const QString & calleridname,
			   const QString & channel,
			   QWidget * parent = 0);
	//void setChannel(const QString &);
	const QString & channel() const;
	//void setCallerId(const QString &);
	const QString & callerId() const;
	//void setCallerIdName(const QString &);
	const QString & callerIdName() const;
protected:
	void mousePressEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
private:
	QPoint m_dragstartpos;
	QString m_channel;
	QString m_callerid;
	QString m_calleridname;
};

#endif

