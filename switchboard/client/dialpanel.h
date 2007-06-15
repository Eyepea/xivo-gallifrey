/* $Id$ */
#ifndef __DIALPANEL_H__
#define __DIALPANEL_H__
#include <QWidget>
#include <QList>
#include "peeritem.h"

class QVBoxLayout;
class QLineEdit;
class QComboBox;

/*! \brief Simple widget to enter a	number and dial it
 */
class DialPanel : public QWidget
{
	Q_OBJECT
public:
	DialPanel(QWidget * parent = 0);
public slots:
	//void textEdited(const QString &);
	void inputValidated();
signals:
	void emitDial(const QString &);		//!< dial a number
private:
	//QLineEdit * m_input;
	QComboBox * m_input;	//!< input widget
};

#endif

