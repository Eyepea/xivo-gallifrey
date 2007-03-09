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
	void dragEnterEvent(QDragEnterEvent * event);
	void dragMoveEvent(QDragMoveEvent * event);
	void dropEvent(QDropEvent * event);
public slots:
	void setRed();
	void setGreen();
	void setGray();
	void setOrange();
private:
	QLabel * m_statelbl;
	QLabel * m_textlbl;
	QPixmap m_square;
	QPoint m_dragstartpos;
	SwitchBoardEngine * m_engine;
};

#endif

