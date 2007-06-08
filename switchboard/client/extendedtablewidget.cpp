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
		if(!m_mychannels.empty())
		{
			QMenu * transferMenu = new QMenu(tr("&Transfer"), &contextMenu);
			QListIterator<PeerChannel *> i(m_mychannels);
			while(i.hasNext())
			{
				const PeerChannel * channel = i.next();
				transferMenu->addAction(channel->otherPeer(),
				                        channel, SLOT(transfer()));
			}
			contextMenu.addMenu(transferMenu);
		}
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

void ExtendedTableWidget::updateMyCalls(const QStringList & chanIds,
                               const QStringList & chanStates,
							   const QStringList & chanOthers)
{
	while(!m_mychannels.isEmpty())
		delete m_mychannels.takeFirst();
	for(int i = 0; i<chanIds.count(); i++)
	{
		PeerChannel * ch = new PeerChannel(chanIds[i], chanStates[i], chanOthers[i]);
		connect(ch, SIGNAL(transferChan(const QString &)),
		        this, SLOT(transferChan(const QString &)) );
		m_mychannels << ch;
	}
}

