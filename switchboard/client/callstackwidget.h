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
	Call( const QString & channelme);
	Call( const QString & channelme, const QString & action, const int & time,
	      const QString & direction, const QString & channelpeer,
	      const QString & exten, const QString & phonen);
	Call( const Call & call);
	const QString & getPhone() {return m_phonen;};
	const QString & getChannelMe() {return m_channelme;};
	const QString & getAction() {return m_action;};
	const int & getTime() {return m_time;};
	const QString & getDirection() {return m_direction;};
	const QString & getChannelPeer() {return m_channelpeer;};
	const QString & getExten() {return m_exten;};
	int updateTime();
	void updateCall(const QString & action,
			const int & time,
			const QString & direction,
			const QString & channelpeer,
			const QString & exten);
private:
	QString m_channelme;
	QString m_action;
	int m_time;
	QString m_direction;
	QString m_channelpeer;
	QString m_exten;
	QString m_phonen;
};


class CallStackWidget : public QWidget
{
	Q_OBJECT
public:
	CallStackWidget(QWidget * parent = 0);
public slots:
	void addCall(const QString & channelme,
		     const QString & action,
		     const int & time,
		     const QString & direction,
		     const QString & channelpeer,
		     const QString & exten,
		     const QString & phonen);
	void showCalls(const QString & tomonitor, const QString & callerid);
	int updateTime();
	void hupchan(const QString & channel);
protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
private:
	void emptyList();
signals:
	void hangUp(const QString & tomonitor);
	void selectForMonitoring(const QString & peer);
	void changeTitle(const QString &);
private:
	QVBoxLayout * m_layout;
	QList<Call> m_calllist;
	QList<CallWidget *> m_afflist;
};

#endif

