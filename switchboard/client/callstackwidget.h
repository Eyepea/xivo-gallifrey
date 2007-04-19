#ifndef __CALLSTACKWIDGET_H__
#define __CALLSTACKWIDGET_H__
#include <QList>
#include <QWidget>
#include <QString>
#include "callwidget.h"

class QVBoxLayout;

/*! \brief Object storing call parametters
 */
class Call// : public QObject
{
public:
	//Peer( const QString & ext, QObject * parent = 0 );
	Call( const QString & channelme);
	Call( const QString & channelme, const QString & action, const int & time,
	      const QString & direction, const QString & channelpeer,
	      const QString & exten, const QString & phonen);
	Call( const Call & call);
	const QString & getPhone() const {return m_phonen;};
	const QString & getChannelMe() const {return m_channelme;};
	const QString & getAction() const {return m_action;};
	const int & getTime() const {return m_time;};
	const QString & getDirection() const {return m_direction;};
	const QString & getChannelPeer() const {return m_channelpeer;};
	const QString & getExten() const {return m_exten;};
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

/*! \brief Widget displaying the current open channels for a phone line.
 */
class CallStackWidget : public QWidget
{
	Q_OBJECT
public:
	CallStackWidget(QWidget * parent = 0);	//!< Constructor
public slots:
	//! Add a call to the list to be displayed
	void addCall(const QString & channelme,
		     const QString & action,
		     const int & time,
		     const QString & direction,
		     const QString & channelpeer,
		     const QString & exten,
		     const QString & phonen);
//	void showCalls(const QString & tomonitor, const QString & callerid);
	void updateDisplay();
	int updateTime();
	void hupchan(const QString & channel);
	void reset();
protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
private:
	void emptyList();	//!< remove all calls from the list
signals:
	void hangUp(const QString &);	//!< hang up a channel
	void changeTitle(const QString &);	//!< change Title
private:
	QVBoxLayout * m_layout;	//!< Vertical Layout used
	QList<Call> m_calllist;	//!< list of Call Objects
	QList<CallWidget *> m_afflist;	//!< List of CallWidget Widgets
	QString m_monitoredPeer;	//!< Peer monitored
};

#endif

