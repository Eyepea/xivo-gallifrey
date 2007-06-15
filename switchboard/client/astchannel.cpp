/* $Id$ */
#include "astchannel.h"

/*! \brief constructor
 *
 * Just initialize the m_id property.
 */
AstChannel::AstChannel(const QString & id, QObject * parent)
: QObject(parent), m_id(id)
{
}

/*! \brief get m_id
 */
const QString & AstChannel::id() const
{
	return m_id;
}

/*! \brief get m_extension
 */
const QString & AstChannel::extension() const
{
	return m_extension;
}

/*! \brief set m_extension
 */
void AstChannel::setExtension(const QString & ext)
{
	m_extension = ext;
}

