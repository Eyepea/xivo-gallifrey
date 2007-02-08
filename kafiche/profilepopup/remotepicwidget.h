#ifndef __REMOTEPICWIDGET_H__
#define __REMOTEPICWIDGET_H__
#include <QWidget>
#include <QHttp>

class QTemporaryFile;
class QLabel;

//! Downloads and displays a picture
class RemotePicWidget : public QWidget
{
	Q_OBJECT
public:
	//! Constructor
	RemotePicWidget(const QString & name, const QString & url, QWidget *parent=0);
	//! Starts the download process
	void startHttpRequest(const QString &);
private slots:
	//! connected to the requestFinished() signal
	void httpRequestFinished(int, bool);
	//! connected to the dataReadProgress() signal
	void httpDataReadProgress(int, int);
	//! connected to the responseHeaderReceived() signal
	void httpReadResponseHeader(const QHttpResponseHeader &);
private:
	QHttp * m_http;				//!< QHttp object
	QTemporaryFile * m_tempFile;	//!< Temporary file for storing the picture
	QLabel * m_label;	//!< QLabel widget used to display the picture
};

#endif

