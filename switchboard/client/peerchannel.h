#ifndef __PEERCHANNEL_H__
#define __PEERCHANNEL_H__

#include <QObject>
#include <QString>

class PeerChannel : public QObject
{
	Q_OBJECT
public:
	PeerChannel(const QString &id, const QString & state,
	            const QString &otherPeer, QObject * parent=0);
	//PeerChannel(const PeerChannel & other);
	//PeerChannel& operator=(const PeerChannel& other);
	const QString & otherPeer() const { return m_otherPeer; };
public slots:
	void intercept();
	void hangUp();
signals:
	void interceptChan(const QString &);
private:
	QString m_id;
	QString m_state;
	QString m_otherPeer;
};

#endif

