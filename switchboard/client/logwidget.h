#ifndef __LOGWIDGET_H__
#define __LOGWIDGET_H__

#include <QWidget>
#include "logeltwidget.h"

class QVBoxLayout;
class SwitchBoardEngine;

class LogWidget : public QWidget
{
	Q_OBJECT
public:
	LogWidget(SwitchBoardEngine * engine, QWidget * parent = 0);
	void addElement(const QString & peer, LogEltWidget::Direction d,
	                const QDateTime & dt, int duration);
protected:
	void timerEvent(QTimerEvent *);
public slots:
	void clear();
	void addLogEntry(const QDateTime & dt, int, const QString &, int);
	void setPeerToDisplay(const QString &);
signals:
	void askHistory(const QString &);
private:
	SwitchBoardEngine * m_engine;
	QVBoxLayout * m_layout;
	QString m_peer;
	int m_timer;
};

#endif

