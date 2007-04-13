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
	void addWidget(QWidget *, QPoint);
	void addItem(QLayoutItem *, QPoint);
	void addItem(QLayoutItem *);
	int count() const;
	QLayoutItem* itemAt(int) const;
	QLayoutItem* takeAt(int);
	int nbRows() const { return m_nb_rows; };
	void setNbRows(int rows) { m_nb_rows = rows; };
	int nbColumns() const { return m_nb_columns; };
	void setNbColumns(int cols) { m_nb_columns = cols; };
	QPoint getPosInGrid(QPoint) const;
	void setItemPosition(int i, QPoint pos);
	QPoint getItemPosition(int i) const;
private:
	QPoint freePosition() const;
	QSize size() const;
	QSize maxItemSize() const;
	QList<QLayoutItem *> m_list;
	QList<QPoint> m_listPos;
	int m_nb_rows;
	int m_nb_columns;
};

#endif

