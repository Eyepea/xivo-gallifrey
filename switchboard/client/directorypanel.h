/* $Id$
 */
#ifndef __DIRECTORYPANEL_H__
#define __DIRECTORYPANEL_H__

#include <QWidget>

class QLineEdit;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;

class DirectoryPanel : public QWidget
{
	Q_OBJECT
public:
	DirectoryPanel(QWidget * parent = 0);
signals:
	void searchDirectory(const QString &);
private slots:
	void startSearch();
	void itemDoubleClicked(QTableWidgetItem *);
public slots:
	void setSearchResponse(const QString &);
private:
	QLineEdit * m_searchText;
	QPushButton * m_searchButton;
	QTableWidget * m_table;
};

#endif

