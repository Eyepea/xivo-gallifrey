#include <QDebug>
#include "peerslayout.h"

PeersLayout::PeersLayout(QWidget * parent)
: QLayout(parent)
{
	qDebug() << "PeersLayout::PeersLayout(" << parent << ")";
}

PeersLayout::PeersLayout()
{
	qDebug() << "PeersLayout::PeersLayout()";
}

QSize PeersLayout::sizeHint() const
{
	qDebug() << "PeersLayout::sizeHints()";
	return size();
}

QSize PeersLayout::minimumSize() const
{
	qDebug() << "PeersLayout::minimumSize()";
	return size();
}

QSize PeersLayout::maximumSize() const
{
	qDebug() << "PeersLayout::maximumSize()";
	return size();
}

QSize PeersLayout::size() const
{
	QSize itemSize = maxItemSize();
	return QSize(itemSize.width(), m_list.size() * itemSize.height());
}

void PeersLayout::addItem(QLayoutItem * item)
{
	qDebug() << "PeersLayout::addItem" << item;
	m_list.append(item);
}

void PeersLayout::setGeometry(const QRect & r)
{
	qDebug() << "PeersLayout::setGeometry" << r;
	QSize itemSize = maxItemSize();
	int i, x, y;
	x = 0; y = 0;
	for(i = 0; i<m_list.size(); i++)
	{
		m_list[i]->setGeometry(
		       QRect( x*itemSize.width()/*left*/, y*itemSize.height()/*top*/,
		              itemSize.width()/*width*/, itemSize.height()/*height*/ )
				                );
		y++;
		/*if(y>6)
		{
			x++; y = 0;
		}*/
	}
}

QLayoutItem* PeersLayout::itemAt(int i) const
{
	qDebug() << "PeersLayout::itemAt" << i;
	return m_list.value(i);
}

QLayoutItem* PeersLayout::takeAt(int i)
{
	qDebug() << "PeersLayout::takeAt" << i;
	return m_list.takeAt(i);
}

int PeersLayout::count() const
{
	qDebug() << "PeersLayout::count()";
	return m_list.size();
}

QSize PeersLayout::maxItemSize() const
{
	int max_w = 150;
	int max_h = 20;
	int i;
	for(i = 0; i < m_list.size(); i++)
	{
		QSize size = m_list[i]->minimumSize();
		if(size.width() > max_w)
			max_w = size.width();
		if(size.height() > max_h)
			max_h = size.height();
	}
	return QSize(max_w, max_h);
}

