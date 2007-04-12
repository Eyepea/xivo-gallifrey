#ifndef __PEERSLAYOUT_H__
#define __PEERSLAYOUT_H__
#include <QLayout>
/* Layout to organize the peers on the screen
 * They should be movable.
 * The layout should be loadable/savable in the configuration (with QSettings)
 */

class PeersLayout : public QLayout
{
	Q_OBJECT
public:
	PeersLayout(QWidget * parent);
	PeersLayout();
	void setGeometry( const QRect & );
	QSize sizeHint() const;
	QSize minimumSize() const;
	QSize maximumSize() const;
	void addItem(QLayoutItem *);
	int count() const;
	QLayoutItem* itemAt(int) const;
	QLayoutItem* takeAt(int);
private:
	QSize size() const;
	QSize maxItemSize() const;
	QList<QLayoutItem *> m_list;
};

#endif

