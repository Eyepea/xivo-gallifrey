#include "astchannel.h"

AstChannel::AstChannel(const QString & id, QObject * parent)
: QObject(parent), m_id(id)
{
}

const QString & AstChannel::id() const
{
	return m_id;
}

const QString & AstChannel::extension() const
{
	return m_extension;
}

void AstChannel::setExtension(const QString & ext)
{
	m_extension = ext;
}

