#include "VariantToJson.h"

namespace JsonQt
{
	QString VariantToJson::parse(const QVariantMap& data)
	{
		QStringList members;
		for(QVariantMap::ConstIterator it = data.begin(); it != data.end(); ++it)
		{
			members.append(QString("\"%1\": %2").arg(it.key()).arg(parseElement(it.value())));
		}
		return "{" + members.join(", ") + "}";
	}
	QString VariantToJson::parseElement(const QVariant& value)
	{
		switch(value.type())
		{
			case QVariant::Bool:
				return value.toBool() ? "true" : "false";
			case QVariant::Map:
				return parse(value.toMap());
			case QVariant::Int:
				return QString::number(value.toInt());
			case QVariant::LongLong:
				return QString::number(value.toLongLong());
			case QVariant::Double:
				return QString::number(value.toDouble());
			case QVariant::UInt:
				return QString::number(value.toUInt());
			case QVariant::ULongLong:
				return QString::number(value.toULongLong());
			case QVariant::List:
			case QVariant::StringList:
				return parseList(value.toList());
			case QVariant::String:
				return QString("\"%1\"").arg(value.toString().replace("\\", "\\\\").replace("\"", "\\\""));
			case QVariant::Invalid:
				return "null";
			default:
				return QString();
		}
	}

	QString VariantToJson::parseList(const QVariantList& list)
	{
		QStringList parts;
		Q_FOREACH(QVariant variant, list)
		{
			parts.append(parseElement(variant));
		}
		return "[" + parts.join(", ") + "]";
	}
}
