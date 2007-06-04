/* $Id$ */
#ifndef __DISPLAYMESSAGES_H__
#define __DISPLAYMESSAGES_H__
#include <QWidget>
#include <QList>
#include <QLabel>
#include "peeritem.h"

class QVBoxLayout;
class QLineEdit;
class QTableWidget;

class DisplayMessagesPanel : public QWidget
{
	Q_OBJECT
public:
	DisplayMessagesPanel(QWidget * parent = 0);
public slots:
/* 	void affTextChanged(); */
	void addMessage(const QString &);
/* signals: */
/* 	void emitDial(const QString &); */
private:
 	//QLabel * m_text;
	QTableWidget * m_table;
};

#endif

