/* $Id: $
 * $Log: $
 */
#ifndef __DIRECTORYPANEL_H__
#define __DIRECTORYPANEL_H__

#include <QWidget>
#include <QLineEdit>
#include <QAction>

class DirectoryPanel : public QWidget
{
	Q_OBJECT
public:
	DirectoryPanel(QWidget * parent);
signals:
	void searchDirectory(const QString &);
public slots:

private:
	QLineEdit * m_searchText;
	QAction * m_searchAction;
};

#endif

