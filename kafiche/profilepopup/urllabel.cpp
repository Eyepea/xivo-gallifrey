#include <QLabel>
#include "urllabel.h"

UrlLabel::UrlLabel(const QString & url, QWidget * parent)
: QLabel(parent)
{
	QString displaytext, text;
	//setForegroundRole( QPalette::Link );
	//setOpenExternalLinks(true);	// new in Qt 4.2
	setTextInteractionFlags( Qt::LinksAccessibleByMouse
                            | Qt::LinksAccessibleByKeyboard
	                        | Qt::TextSelectableByMouse
	                        | Qt::TextSelectableByKeyboard );
	if(url.startsWith("mailto:"))
	{
		displaytext = url.mid(7);
	}
	else
	{
		displaytext = url;
	}
	text = "<a href=\"";
	text.append(url);
	text.append("\">");
	text.append(displaytext);
	text.append("</a>");
	setText(text);
}

