/* $Id$ */
#include <QContextMenuEvent>
#include <QMenu>
#include <QDebug>
#include "extendedtablewidget.h"

ExtendedTableWidget::ExtendedTableWidget(QWidget * parent)
: QTableWidget(parent)
{
}

ExtendedTableWidget::ExtendedTableWidget(int rows, int columns, QWidget * parent)
: QTableWidget(rows, columns, parent)
{
}

void ExtendedTableWidget::contextMenuEvent(QContextMenuEvent * event)
{
	qDebug() << "ExtendedTableWidget::contextMenuEvent()";
	qDebug() << event->pos();
	QTableWidgetItem * item = itemAt( event->pos() );
	QRegExp re("\\+?[0-9\\s\\.]+");
	if(item && re.exactMatch( item->text() ))
	{
		m_numberToDial = item->text();
		qDebug() << "preparing to dial :" << m_numberToDial;
		QMenu contextMenu(this);
		contextMenu.addAction( tr("&Dial"), this, SLOT(dialNumber()) );
		contextMenu.exec( event->globalPos() );
	}
}

void ExtendedTableWidget::dialNumber()
{
	if(m_numberToDial.length() > 0)
	{
		emitDial( m_numberToDial );
	}
}

