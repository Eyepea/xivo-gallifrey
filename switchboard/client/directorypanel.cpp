/* $Id$ */
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include "directorypanel.h"

DirectoryPanel::DirectoryPanel(QWidget * parent)
: QWidget(parent)
{
	QVBoxLayout * vlayout = new QVBoxLayout(this);
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
	vlayout->addStretch( 1 );
}

void DirectoryPanel::startSearch()
{
	searchDirectory( m_searchText->text() );
}

