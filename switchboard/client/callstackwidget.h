#ifndef __CALLSTACKWIDGET_H__
#define __CALLSTACKWIDGET_H__
#include <QList>
#include <QWidget>
#include <QString>
#include "callwidget.h"

class QVBoxLayout;


class Call// : public QObject
{
public:
	//Peer( const QString & ext, QObject * parent = 0 );
	Call( const QString & ext);
	Call( const Call & call);
	const QString & ext() { return m_ext; };
	void setWidget(CallWidget * widget) { m_callwidget = widget; };
	CallWidget * getWidget() { return m_callwidget; };
private:
	QString m_ext;
	CallWidget * m_callwidget;
};


class CallStackWidget : public QWidget
{
	Q_OBJECT
public:
	CallStackWidget(QWidget * parent = 0);
public slots:
	void addCall(const QString & channelme,
		     const QString & action,
		     const QString & time,
		     const QString & direction,
		     const QString & channelpeer,
		     const QString & exten);
private:
	QVBoxLayout * m_layout;
	QList<Call> m_calllist;
};

#endif

