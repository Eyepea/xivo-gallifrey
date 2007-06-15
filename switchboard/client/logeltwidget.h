/* $Id$ */
#ifndef __LOGELTWIDGET_H__
#define __LOGELTWIDGET_H__

#include <QWidget>
#include <QDateTime>
#include <QAction>
#include <QContextMenuEvent>

/*! \brief Log element widget
 */
class LogEltWidget : public QWidget
{
	Q_OBJECT
public:
	//! Call direction (out or in)
	typedef enum { OutCall = 1, InCall = 2 } Direction;
	LogEltWidget( const QString & peer,
	              Direction d,
				  const QDateTime & dt,
				  int duration,
				  QWidget * parent = 0 );
	//! get m_dateTime
	const QDateTime & dateTime() const { return m_dateTime; };
	//! get m_peer
	const QString & peer() const { return m_peer; };
	//! get m_direction
	Direction direction() const { return m_direction; };
protected:
	void contextMenuEvent(QContextMenuEvent *);
private slots:
	void callBackPeer();
signals:
	void emitDial(const QString &);		//!< signal to dial back.
private:
	QDateTime m_dateTime;	//!< date time of the call 
	QString m_peer;			//!< phone number who called/was called
	Direction m_direction;	//!< call direction (In/out)
	QAction * m_dialAction;	//!< dial action
};

#endif

