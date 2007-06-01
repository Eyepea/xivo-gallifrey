/* $Id$ */
#ifndef __PEERWIDGET_H__
#define __PEERWIDGET_H__
#include <QWidget>
#include <QPixmap>
#include <QPoint>
#include "peerchannel.h"

class QLabel;

/*! \brief Widget to display a Peer status
 *
 * Display Icons for the state and the peer name.
 */
class PeerWidget : public QWidget
{
	Q_OBJECT
public:
	PeerWidget(const QString & id, const QString & name,
	           QWidget * parent = 0/*, int size = 16*/);
	~PeerWidget();
	void clearChanList();
	void addChannel(const QString &, const QString &, const QString &);
protected:
	void mouseMoveEvent(QMouseEvent * event);
	void mousePressEvent(QMouseEvent * event);
//	void mouseDoubleClickEvent(QMouseEvent * event);
	void dragEnterEvent(QDragEnterEvent * event);
	void dragMoveEvent(QDragMoveEvent * event);
	void dropEvent(QDropEvent * event);
	void contextMenuEvent(QContextMenuEvent *);
signals:
	void originateCall(const QString &, const QString &);
	void transferCall(const QString &, const QString &);
	void interceptChan(const QString &);
	void emitDial(const QString &);
	void doRemoveFromPanel(const QString &);
public slots:
	void setBlue(int n);
	//void setCyan(int n);
	void setGray(int n);
	void setGreen(int n);
	void setOrange(int n);
	void setRed(int n);
	void setYellow(int n);
	//void setBlack(int n);
	//void setDarkGreen(int n);
private slots:
	void removeFromPanel();
	void dial();
private:
	QLabel * m_statelbl;	//!< Peer state display (ringing, online, ...)
	QLabel * m_availlbl;	//!< Peer state display from XIVO CTI Client
	QLabel * m_textlbl;		//!< text label : to display peer name
	//QPixmap m_square;		//!< pixmap used to display the states
	QPoint m_dragstartpos;	//!< drag start position
	QString m_id;	//!< peer id : asterisk/protocol/extension
	QString m_name;	//!< caller id to display : usualy the NAME of the person

	QAction * m_removeAction;
	QAction * m_dialAction;
	
	QList<PeerChannel *> m_channels;

	/* TODO : have the Pixmaps as static objects */
	QPixmap m_phone_green;
	QPixmap m_phone_red;
	QPixmap m_phone_orange;
	QPixmap m_phone_gray;
	QPixmap m_phone_yellow;
	QPixmap m_phone_blue;

	QPixmap m_person_green;
	QPixmap m_person_red;
	QPixmap m_person_orange;
	QPixmap m_person_gray;
	QPixmap m_person_yellow;
	QPixmap m_person_blue;
};

#endif

