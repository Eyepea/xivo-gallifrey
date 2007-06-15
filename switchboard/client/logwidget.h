/* $Id$ */
#ifndef __LOGWIDGET_H__
#define __LOGWIDGET_H__

#include <QWidget>
#include "logeltwidget.h"

class QVBoxLayout;
class SwitchBoardEngine;
class QRadioButton;

/*! \brief Call Log display widget
 */
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
	void askHistory(const QString &, int);	//!< need history to be updated !
private:
	SwitchBoardEngine * m_engine;	//!< SwitchBoardEngine object
	QVBoxLayout * m_layout;			//!< Widget layout
	QString m_peer;					//!< "monitored" peer
	int m_timer;					//!< timer id for refresh
	QRadioButton * m_radioNone;		//!< "None" radio button
	QRadioButton * m_radioOut;		//!< "Out" radio button
	QRadioButton * m_radioIn;		//!< "In" radio button
	QRadioButton * m_radioMissed;	//!< "Missed" radio button
};

#endif

