#ifndef __CALLWIDGET_H__
#define __CALLWIDGET_H__
#include <QWidget>

class QLabel;

/*! \brief Widget displaying a call (channel)
 *
 * The Call is displayed with a colored square representing
 * its state, the id of the channel, the direction and
 * the source of the call
 */
class CallWidget : public QWidget
{
public:
	//! Default constructor
	CallWidget(QWidget * parent = 0);
	CallWidget(const QString & tomonitor,
		   QWidget * parent = 0);
	CallWidget(const QString & channelme,
		   const QString & action,
		   const int & time,
		   const QString & direction,
		   const QString & channelpeer,
		   const QString & exten,
		   QWidget * parent = 0);
	void updateWidget(const QString & action,
			  const int & time,
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
	void mouseDoubleClickEvent(QMouseEvent * event);
	//void dragEnterEvent(QDragEnterEvent * event);
	//	void dragMoveEvent(QDragMoveEvent * event);
	//	void dropEvent(QDropEvent * event);
private:
	void setActionPixmap(const QString &);
private:
	QPoint m_dragstartpos;	//!< used for drag
	QString m_channelme;	//!< channel identifier
	QString m_callerid;		//!< caller id
	QString m_calleridname;	//!< caller id name
	QLabel * m_lbl_action;	//!< sub widget
	QLabel * m_lbl_time;	//!< sub widget
	QLabel * m_lbl_direction;	//!< sub widget
	QLabel * m_lbl_channelpeer;	//!< sub widget
	QLabel * m_lbl_exten;	//!< sub widget
	QPixmap m_square;		//!< QPixmap used to display the action square
};



#endif

