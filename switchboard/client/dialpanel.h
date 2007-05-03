#ifndef __DIALPANEL_H__
#define __DIALPANEL_H__
#include <QWidget>
#include <QList>
#include "peeritem.h"

class QVBoxLayout;
class QLineEdit;
class SwitchBoardEngine;

class DialPanel : public QWidget
{
	Q_OBJECT
public:
	DialPanel(QWidget * parent = 0);
	void setEngine(SwitchBoardEngine *);
public slots:
	void affTextChanged();
private:
	SwitchBoardEngine * m_engine;
	QLineEdit * m_input;
};

#endif

