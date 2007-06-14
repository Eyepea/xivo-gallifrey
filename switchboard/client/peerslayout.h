#ifndef __PEERSLAYOUT_H__
#define __PEERSLAYOUT_H__
#include <QLayout>

/*! \brief Grid layout to organize the peers
 *
 * Layout to organize the peers on the screen
 * The peers are movable.
 */
class PeersLayout : public QLayout
{
	Q_OBJECT
public:
	//! constructor
	PeersLayout(QWidget * parent);
	//! ??
	PeersLayout();
	//! set geometry
	void setGeometry( const QRect & );
	//! return size Hint (prefered size)
	QSize sizeHint() const;
	QSize minimumSize() const;
	QSize maximumSize() const;
	void addWidget(QWidget *, QPoint);
	//! add the Item at a specific emplacement
	void addItem(QLayoutItem *, QPoint);
	//! default addItem implementation
	void addItem(QLayoutItem *);
	//! return the number of items
	int count() const;
	QLayoutItem* itemAt(int) const;
	QLayoutItem* takeAt(int);
	//! get m_nb_rows
	int nbRows() const { return m_nb_rows; };
	//! set m_nb_rows
	void setNbRows(int rows) { m_nb_rows = rows; };
	//! get m_nb_columns
	int nbColumns() const { return m_nb_columns; };
	//! set m_nb_columns
	void setNbColumns(int cols) { m_nb_columns = cols; };
	QPoint getPosInGrid(QPoint) const;
	void setItemPosition(int i, QPoint pos);
	QPoint getItemPosition(int i) const;
private:
	QPoint freePosition() const;
	QSize size() const;
	QSize maxItemSize() const;
	QList<QLayoutItem *> m_list;	//!< layout items list
	QList<QPoint> m_listPos;		//!< positions list
	int m_nb_rows;					//!< height
	int m_nb_columns;				//!< width
};

#endif

