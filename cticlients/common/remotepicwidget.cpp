/*
XIVO customer information client : popup profile for incoming calls
Copyright (C) 2007  Proformatique

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/
#include <QTemporaryFile>
#include <QLabel>
#include <QPixmap>
#include <QDebug>
#include <QSslError>
#include <QStackedLayout>
#include <QUrl>

#include "remotepicwidget.h"

RemotePicWidget::RemotePicWidget(const QString & name, const QString & url,
                                 QWidget *parent)
        : QWidget(parent), m_http(0), m_tempFile(0)
{
	QStackedLayout * layout = new QStackedLayout(this);
	m_label = new QLabel(name, this);
	layout->addWidget(m_label);
	startHttpRequest(url);
}

void RemotePicWidget::startHttpRequest(const QString & urlstr)
{
	QUrl url(urlstr);
	if(!m_http)
	{
		m_http = new QHttp(this);
                connect( m_http, SIGNAL(requestFinished(int, bool)),
                         this, SLOT(httpRequestFinished(int, bool)) );
                connect( m_http, SIGNAL(dataReadProgress(int, int)),
                         this, SLOT(httpDataReadProgress(int, int)) );
                connect( m_http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)),
                         this, SLOT(httpReadResponseHeader(const QHttpResponseHeader &)) );
                connect( m_http, SIGNAL(sslErrors(const QList<QSslError> &)),
                         this, SLOT(httpSslErrors(const QList<QSslError> &)) );
	}
	if(!m_tempFile) {
                // m_tempFile = new QTemporaryFile(this);
                // m_tempFile->setAutoRemove(false);
#ifdef Q_WS_WIN
                m_tempFile = new QFile("xivopush." + url.host() + url.path().replace("/", "."));
#else /* Q_WS_X11, Q_WS_MAC */
                m_tempFile = new QFile("/tmp/xivopush." + url.host() + url.path().replace("/", "."));
#endif
                m_tempFile->open(QIODevice::ReadWrite);
	}
	qDebug() << m_tempFile->fileName() << m_tempFile->size() << url.host() << url.port() << url.scheme();
        if(url.scheme() == QString("https"))
                m_http->setHost(url.host(),
                                QHttp::ConnectionModeHttps,
                                url.port() != -1 ? url.port() : 443);
        else
                m_http->setHost(url.host(),
                                QHttp::ConnectionModeHttp,
                                url.port() != -1 ? url.port() : 80);
	if (! url.userName().isEmpty())
		m_http->setUser(url.userName(), url.password());
	//httpRequestAborted = false;
        m_http->ignoreSslErrors();
	m_httpGetId = m_http->get(url.path(), m_tempFile);
	// qDebug() << "httpGetId =" << m_httpGetId;
	// faut envoyer les données sur un fichier temporaire ?
	// QTemporaryFile().open() et filename() apres.
}

void RemotePicWidget::httpSslErrors(const QList<QSslError> & errors)
{
        qDebug() << "RemotePicWidget::httpSslErrors()" << errors;
        m_http->ignoreSslErrors();
}


void RemotePicWidget::httpDataReadProgress(int , int)
{
}

// void RemotePicWidget::httpDataReadProgress(int bytesRead, int totalBytes)
// {
//         qDebug() << (100*bytesRead/totalBytes) << "%"
//                  << bytesRead << "out of" << totalBytes;
// }

void RemotePicWidget::httpRequestFinished(int requestId, bool error)
{
        if(requestId == m_httpGetId) {
                qDebug() << "RemotePicWidget::httpRequestFinished()" << requestId << error << m_tempFile->fileName() << m_tempFile->size();
                m_tempFile->close();
                QPixmap pixmap(m_tempFile->fileName());
                m_label->setPixmap(pixmap);
                m_tempFile->remove();
        }
}

void RemotePicWidget::httpReadResponseHeader(const QHttpResponseHeader &responseHeader)
{
	qDebug() << "RemotePicWidget::httpReadResponseHeader()"
	         << responseHeader.statusCode();
}
