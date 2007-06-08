/* $Id$ */
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
protected:
	//void contextMenuEvent(QContextMenuEvent *);
signals:
	void searchDirectory(const QString &);
	void emitDial(const QString &);
	void updateMyCalls(const QStringList &, const QStringList &, const QStringList &);
private slots:
	void startSearch();
	void itemDoubleClicked(QTableWidgetItem *);
	//void dialNumber();
public slots:
	void setSearchResponse(const QString &);
	void stop();
private:
	QLineEdit * m_searchText;
	QPushButton * m_searchButton;
	QTableWidget * m_table;
	QString m_numberToDial;
};

#endif

