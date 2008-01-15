/* XIVO CTI clients
Copyright (C) 2007, 2008  Proformatique

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/

/* $Revision$
 * $Date$
 */

#ifndef __REMOTEPICWIDGET_H__
#define __REMOTEPICWIDGET_H__

#include <QWidget>
#include <QHttp>

class QFile;
class QLabel;
class QSslError;
class QTemporaryFile;

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

        void httpSslErrors(const QList<QSslError> &);
private:
	QHttp * m_http;				//!< QHttp object
        // QTemporaryFile * m_tempFile;	//!< Temporary file for storing the picture
        QFile * m_tempFile;		//!< Temporary file for storing the picture
	QLabel * m_label;		//!< QLabel widget used to display the picture
        int m_httpGetId;
};

#endif
