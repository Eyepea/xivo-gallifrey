#ifndef __CALLWIDGET_H__
#define __CALLWIDGET_H__
#include <QWidget>

class QLabel;

class CallWidget : public QWidget
{
public:
	CallWidget(QWidget * parent = 0);
	CallWidget(const QString & tomonitor,
		   QWidget * parent = 0);
	CallWidget(const QString & channelme,
		   const QString & action,
		   const QString & time,
		   const QString & direction,
		   const QString & channelpeer,
		   const QString & exten,
		   QWidget * parent = 0);
	void updateWidget(const QString & action,
			  const QString & time,
			  const QString & direction,
			  const QString & channelpeer,
			  const QString & exten);
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
	QLabel * m_lbl_channelme;
	QLabel * m_lbl_action;
	QLabel * m_lbl_time;
	QLabel * m_lbl_direction;
	QLabel * m_lbl_channelpeer;
	QLabel * m_lbl_exten;
};



#endif

