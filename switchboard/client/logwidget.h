#ifndef __LOGWIDGET_H__
#define __LOGWIDGET_H__

#include <QWidget>
#include "logeltwidget.h"

class QVBoxLayout;
class SwitchBoardEngine;
class QRadioButton;

class LogWidget : public QWidget
{
	Q_OBJECT
public:
	LogWidget(SwitchBoardEngine * engine, QWidget * parent = 0);
	void addElement(const QString & peer, LogEltWidget::Direction d,
	                const QDateTime & dt, int duration);
protected:
	void timerEvent(QTimerEvent *);
private:
	int mode();
public slots:
	void clear();
	void addLogEntry(const QDateTime & dt, int, const QString &, int);
	void setPeerToDisplay(const QString &);
private slots:
	void modeChanged(bool);
signals:
	void askHistory(const QString &, int);
private:
	SwitchBoardEngine * m_engine;
	QVBoxLayout * m_layout;
	QString m_peer;
	int m_timer;
	QRadioButton * m_radioNone;
	QRadioButton * m_radioOut;
	QRadioButton * m_radioIn;
	QRadioButton * m_radioMissed;
};

#endif

