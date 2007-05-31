/* $Id$ */
#ifndef __EXTENDEDTABLEWIDGET_H__
#define __EXTENDEDTABLEWIDGET_H__

#include <QTableWidget>

class QContextMenuEvent;

class ExtendedTableWidget : public QTableWidget
{
	Q_OBJECT
public:
	ExtendedTableWidget(QWidget * parent = 0);
	ExtendedTableWidget(int rows, int columns, QWidget * parent = 0);
protected:
	void contextMenuEvent(QContextMenuEvent * event);
signals:
	void emitDial(const QString & number);
private slots:
	void dialNumber();
private:
	QString m_numberToDial;
};

#endif

