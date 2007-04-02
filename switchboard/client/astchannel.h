#ifndef __ASTCHANNEL_H__
#define __ASTCHANNEL_H__
#include <QObject>

class AstChannel : public QObject
{
public:
	AstChannel(const QString & id, QObject * parent = 0);
	AstChannel(const AstChannel &);
	const QString & id() const;
	const QString & extension() const;
	void setExtension(const QString &);
	const QString & state() const;
	void setState(const QString &);
private:
	QString m_id;
	QString m_extension;
	QString m_state;
	enum { TO, FROM } m_direction;
	QString m_linkedchannel;
	QString m_linkedextension;
};

#endif

