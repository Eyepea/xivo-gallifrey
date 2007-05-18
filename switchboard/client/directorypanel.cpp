/* $Id$ */
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTableWidget>
#include <QLabel>
#include <QContextMenuEvent>
#include <QMenu>
#include <QDebug>
#include "directorypanel.h"

DirectoryPanel::DirectoryPanel(QWidget * parent)
: QWidget(parent)
{
	QVBoxLayout * vlayout = new QVBoxLayout(this);
	vlayout->setMargin(0);
	QLabel * titleLbl = new QLabel( tr("Directory"), this );
	vlayout->addWidget( titleLbl, 0, Qt::AlignCenter );
	QHBoxLayout * hlayout = new QHBoxLayout();
	m_searchText = new QLineEdit(this);
	connect( m_searchText, SIGNAL(returnPressed()),
	         this, SLOT(startSearch()) );
	hlayout->addWidget( m_searchText );
	m_searchButton = new QPushButton( tr("Search"), this );
	connect( m_searchButton, SIGNAL(clicked()),
	         this, SLOT(startSearch()) );
	hlayout->addWidget( m_searchButton );
	vlayout->addLayout( hlayout );
	m_table = new QTableWidget( this );
	connect( m_table, SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
	         this, SLOT(itemDoubleClicked(QTableWidgetItem *)) );
	vlayout->addWidget(m_table);
	/*
	QStringList labelList;
	labelList << QString("Numero") << QString("Nom");
	m_table->setColumnCount(2);
	m_table->setRowCount(3);
	m_table->setHorizontalHeaderLabels( labelList );
	for(int y = 0; y < 3; y++)
		for(int x = 0; x < 2; x++)
		{
			m_table->setItem( y, x, new QTableWidgetItem("-" + QString::number(x) + "," + QString::number(y)) );
		}
	qDebug() << m_table->columnCount();*/
	//vlayout->addStretch( 1 );
}

void DirectoryPanel::itemDoubleClicked(QTableWidgetItem * item)
{
	qDebug() << item << item->text();
	// check if the string is a number
	QRegExp re("[0-9]+");
	qDebug() << re.exactMatch(item->text());
	// TODO : DIAL
}

void DirectoryPanel::setSearchResponse(const QString & resp)
{
	int i, x, y;
	//qDebug() << "setSearchResponse()" << resp;
	QStringList items = resp.split(";");
	int ncolumns = items[0].toInt();
	int nrows = ((items.size() - 1) / ncolumns) - 1;
	m_table->setColumnCount(ncolumns);
	m_table->setRowCount(nrows);
	//qDebug() << items.size() << nrows << ncolumns ;
	QStringList labelList;
	for(i = 1; i <= ncolumns; i++)
		labelList << items[i];
	m_table->setHorizontalHeaderLabels( labelList );
	for(y = 0; y < nrows; y++)
		for(x = 0; x < ncolumns; x++)
		{
			QTableWidgetItem * item = new QTableWidgetItem(items[1+(1+y)*ncolumns+x]);
			item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
			//item->setToolTip();
			//item->setStatusTip();
			//qDebug() << x << y << item->flags();
			m_table->setItem( y, x, item );
			//qDebug() << m_table->cellWidget( y, x );
		}
}

void DirectoryPanel::startSearch()
{
	searchDirectory( m_searchText->text() );
}

void DirectoryPanel::contextMenuEvent(QContextMenuEvent * event)
{
	qDebug() << "DirectoryPanel::contextMenuEvent" << event->pos();
	//qDebug() << "  " << event->globalPos();
	qDebug() << "  " << event->pos() - m_table->pos() << "m_table->pos()=" << m_table->pos();
	QTableWidgetItem * item = m_table->itemAt( event->pos() - m_table->pos() );
	qDebug() << "  " << item;
	if(item)
	{
		qDebug() << "   " << item->text();
		// TODO : check that this is a number !
		QMenu contextMenu(this);
		contextMenu.addAction( tr("&Dial") );
		contextMenu.exec( event->globalPos() );
	}
}

