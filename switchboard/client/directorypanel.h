/* $Id$
 */
#ifndef __DIRECTORYPANEL_H__
#define __DIRECTORYPANEL_H__

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

class DirectoryPanel : public QWidget
{
	Q_OBJECT
public:
	DirectoryPanel(QWidget * parent = 0);
signals:
	void searchDirectory(const QString &);
private slots:
	void startSearch();
public slots:

private:
	QLineEdit * m_searchText;
	QPushButton * m_searchButton;
};

#endif

