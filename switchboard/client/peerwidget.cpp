#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QDebug>
#include "peerwidget.h"

PeerWidget::PeerWidget(const QString & txtlbl, QWidget * parent, int size)
: QWidget(parent), m_square(size,size)
{
	QHBoxLayout * layout = new QHBoxLayout(this);
	m_statelbl = new QLabel(this);
	m_square.fill( Qt::gray );
	m_statelbl->setPixmap( m_square );
	layout->addWidget( m_statelbl );
	layout->addWidget( new QLabel(txtlbl, this) );
}

void PeerWidget::setRed()
{
	m_square.fill( Qt::red );
	m_statelbl->setPixmap( m_square );
}

void PeerWidget::setGreen()
{
	m_square.fill( Qt::green );
	m_statelbl->setPixmap( m_square );
}

void PeerWidget::setGray()
{
	m_square.fill( Qt::gray );
	m_statelbl->setPixmap( m_square );
}

void PeerWidget::setOrange()
{
	m_square.fill( QColor(255,127,0) );
	m_statelbl->setPixmap( m_square );
}

