/* $Id$ */
#ifndef __ASTCHANNEL_H__
#define __ASTCHANNEL_H__
#include <QObject>

/*! \brief Object to store Asterisk channel parameters
 *
 *
 */
class AstChannel : public QObject
{
	Q_OBJECT
public:
	AstChannel(const QString & id, QObject * parent = 0);
	//AstChannel(const AstChannel &);
	const QString & id() const;
	const QString & extension() const;
	void setExtension(const QString &);
	//const QString & state() const;
	//void setState(const QString &);
private:
	QString m_id;	//!< id of the asterisk channel
	QString m_extension;	//!< extension
	//QString m_state;		//!< state
	enum { TO, FROM } m_direction;	//!< direction
	QString m_linkedchannel;	//!< linked channel
	QString m_linkedextension;	//!< linked extension
};

#endif

