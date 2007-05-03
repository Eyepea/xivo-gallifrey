#ifndef __LOGELTWIDGET_H__
#define __LOGELTWIDGET_H__

#include <QWidget>
#include <QDateTime>

class LogEltWidget : public QWidget
{
	Q_OBJECT
public:
	typedef enum { OutCall = 1, InCall = 2 } Direction;
	LogEltWidget( const QString & peer,
	              Direction d,
				  const QDateTime & dt,
				  int duration,
				  QWidget * parent = 0 );
	const QDateTime & dateTime() const { return m_dateTime; };
	const QString & peer() const { return m_peer; };
	Direction direction() const { return m_direction; };
private:
	QDateTime m_dateTime;
	QString m_peer;
	Direction m_direction;
};

#endif

