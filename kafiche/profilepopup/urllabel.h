#ifndef __URLLABEL_H__
#define __URLLABEL_H__

#include <QLabel>

class UrlLabel : public QLabel
{
	Q_OBJECT
public:
	UrlLabel(const QString & url, QWidget *parent=0);
};

#endif

