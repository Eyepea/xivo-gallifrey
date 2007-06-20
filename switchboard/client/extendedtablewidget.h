/* $Id$ */
#ifndef __EXTENDEDTABLEWIDGET_H__
#define __EXTENDEDTABLEWIDGET_H__

#include <QTableWidget>
#include <QList>
#include "peerchannel.h"

class QContextMenuEvent;

/*! \brief Table class inheriting QTableWidget with contextMenu added.
 */
class ExtendedTableWidget : public QTableWidget
{
	Q_OBJECT
public:
	ExtendedTableWidget(QWidget * parent = 0);
	ExtendedTableWidget(int rows, int columns, QWidget * parent = 0);
protected:
	void contextMenuEvent(QContextMenuEvent * event);
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dropEvent(QDropEvent *event);
signals:
	void emitDial(const QString & number);	//!< dial
	void transferCall(const QString &, const QString &);	//!< transfer a call
	void originateCall(const QString &, const QString &);	//!< originate a call
public slots:
	void updateMyCalls(const QStringList &, const QStringList &, const QStringList &);
private slots:
	void dialNumber();
	void transferChan(const QString &);
private:
	QString m_numberToDial;		//!< used to store number to dial or to transfer to
	QList<PeerChannel *> m_mychannels;	//!< "my channels" list for transfer menu
};

#endif

