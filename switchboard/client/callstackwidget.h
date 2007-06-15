/* $Id$ */
#ifndef __CALLSTACKWIDGET_H__
#define __CALLSTACKWIDGET_H__
#include <QObject>
#include <QList>
#include <QWidget>
#include <QString>
#include <QDateTime>
#include "callwidget.h"

class QVBoxLayout;

/*! \brief Object storing call parametters
 */
class Call// : public QObject
{
public:
	//Peer( const QString & ext, QObject * parent = 0 );
	Call( const QString & channelme);
	Call( const QString & channelme, const QString & action, int time,
	      const QString & direction, const QString & channelpeer,
	      const QString & exten, const QString & phonen);
	Call( const Call & call);
	//! get m_phonen
	const QString & getPhone() const {return m_phonen;};
	//! get m_channelme
	const QString & getChannelMe() const {return m_channelme;};
	//! get m_action
	const QString & getAction() const {return m_action;};
	//! get duration of the channel
	int getTime() const {
		return m_startTime.secsTo(QDateTime::currentDateTime());
	};
	//! get m_direction
	const QString & getDirection() const {return m_direction;};
	//! get m_channelpeer
	const QString & getChannelPeer() const {return m_channelpeer;};
	//! get m_exten
	const QString & getExten() const {return m_exten;};
	void updateCall(const QString & action,
			int time,
			const QString & direction,
			const QString & channelpeer,
			const QString & exten);
private:
	QString m_channelme;	//!< "my" channel 
	QString m_action;		//!< action
	QDateTime m_startTime;	//!< channel start time
	QString m_direction;	//!< chan direction
	QString m_channelpeer;	//!< linked channel
	QString m_exten;		//!< extension
	QString m_phonen;		//!< phone number
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
		     int time,
		     const QString & direction,
		     const QString & channelpeer,
		     const QString & exten,
		     const QString & phonen);
//	void showCalls(const QString & tomonitor, const QString & callerid);
	void updateDisplay();
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
	void monitoredPeerChanged(const QString &);
private:
	QVBoxLayout * m_layout;	//!< Vertical Layout used
	QList<Call> m_calllist;	//!< list of Call Objects
	QList<CallWidget *> m_afflist;	//!< List of CallWidget Widgets
	QString m_monitoredPeer;	//!< Peer monitored
};

#endif

