#ifndef __SWITCHBOARDENGINE_H__
#define __SWITCHBOARDENGINE_H__
#include <QHash>
#include <QObject>
#include <QTcpSocket>

class QTimer;
class SwitchBoardWindow;

class SwitchBoardEngine: public QObject
{
	Q_OBJECT
public:
	SwitchBoardEngine(QObject * parent = 0);
	void setWindow(SwitchBoardWindow *);
	void setAddress(const QString & host, quint16 port);
	quint16 port() const;
	const QString & host() const;
	void saveSettings();	//!< save settings
private:
	void connectSocket();
	void finishedReceivingHints();
	void loadSettings();	//!< load settings
protected:
	void timerEvent(QTimerEvent *event);
public slots:
	void start();
	void stop();
	void selectAsMonitored(const QString & peer);
	void originateCall(const QString & src, const QString & dst);
	void transferCall(const QString & src, const QString & dst);
private slots:
	void updatePeers(const QStringList & liststatus);
	void updateCallerids(const QStringList & liststatus);
	void socketConnected();
	void socketDisconnected();
	void socketHostFound();
	void socketError(QAbstractSocket::SocketError);
	void socketStateChanged(QAbstractSocket::SocketState);
	void socketReadyRead();
	void hangUp(const QString & peer);
signals:
	void started();
	void stopped();
	void emitTextMessage(const QString &);
	int updateTime();
	void updateCall(const QString & channelme,
			const QString & action,
			const int & time,
			const QString & direction,
			const QString & channelpeer,
			const QString & exten,
			const QString & phone);
	void endCall(const QString &);
	void showCalls(const QString & tomonitor, const QString & callerid);
	void updatePeer(const QString &, const QString &,
	                const QString &, const QString &);
	void removePeer(const QString &);
private:
	QTcpSocket * m_socket;
	int m_timer;
	quint16 m_port;
	QString m_host;
	QString m_astid;
	SwitchBoardWindow * m_window;
	QString m_pendingcommand;
	QString m_tomonitor;
	QHash<QString, QString> m_callerids;
};

#endif

