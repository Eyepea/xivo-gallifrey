/* $Id$ */
#ifndef __DIRECTORYPANEL_H__
#define __DIRECTORYPANEL_H__

#include <QWidget>

class QLineEdit;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;

/*! \brief Directory allowing search
 */
class DirectoryPanel : public QWidget
{
	Q_OBJECT
public:
	DirectoryPanel(QWidget * parent = 0);
protected:
	//void contextMenuEvent(QContextMenuEvent *);
signals:
	//! start a search
	void searchDirectory(const QString &);
	//! dial selected number
	void emitDial(const QString &);
	//! transfer one of my call to this number
	void transferCall(const QString &, const QString &);
	//! originate a call
	void originateCall(const QString &, const QString &);
	//! signal to be redirected to view
	void updateMyCalls(const QStringList &, const QStringList &, const QStringList &);
private slots:
	void startSearch();
	void itemDoubleClicked(QTableWidgetItem *);
	//void dialNumber();
public slots:
	void setSearchResponse(const QString &);
	void stop();
private:
	QLineEdit * m_searchText;	//!< search text input
	QPushButton * m_searchButton;	//!< button
	QTableWidget * m_table;		//!< table to display results
	QString m_numberToDial;		//!< used for dial action
};

#endif

