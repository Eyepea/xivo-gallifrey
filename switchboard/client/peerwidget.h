#ifndef __PEERWIDGET_H__
#define __PEERWIDGET_H__
#include <QWidget>
#include <QPixmap>
#include <QPoint>

class QLabel;
class SwitchBoardEngine;

class PeerWidget : public QWidget
{
	Q_OBJECT
public:
	PeerWidget(const QString & txtlbl, SwitchBoardEngine * engine,
	           QWidget * parent = 0, int size = 16);
protected:
	void mouseMoveEvent(QMouseEvent * event);
	void mousePressEvent(QMouseEvent * event);
	void mouseDoubleClickEvent(QMouseEvent * event);
	void dragEnterEvent(QDragEnterEvent * event);
	void dragMoveEvent(QDragMoveEvent * event);
	void dropEvent(QDropEvent * event);
public slots:
	void setBlue(int n);
	void setCyan(int n);
	void setGray(int n);
	void setGreen(int n);
	void setOrange(int n);
	void setRed(int n);
	void setYellow(int n);
	void setBlack(int n);
	void setDarkGreen(int n);
private:
	QLabel * m_statelbl;
	QLabel * m_availlbl;
	QLabel * m_textlbl;
	QPixmap m_square;
	QPoint m_dragstartpos;
	SwitchBoardEngine * m_engine;
};

#endif

