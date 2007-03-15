#ifndef __URLLABEL_H__
#define __URLLABEL_H__

#include <QLabel>

/*! \brief UrlLabel Widget
 *
 * This label is a QLabel designed to display an URL. */
class UrlLabel : public QLabel
{
	Q_OBJECT
public:
	UrlLabel(const QString & url, QWidget *parent=0);
};

#endif

