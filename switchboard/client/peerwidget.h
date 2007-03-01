#ifndef __PEERWIDGET_H__
#define __PEERWIDGET_H__
#include <QWidget>
#include <QPixmap>

class QLabel;

class PeerWidget : public QWidget
{
	Q_OBJECT
public:
	PeerWidget(const QString & txtlbl, QWidget * parent = 0, int size = 16);
public slots:
	void setRed();
	void setGreen();
	void setGray();
	void setOrange();
private:
	QLabel * m_statelbl;
	QPixmap m_square;
};

#endif

