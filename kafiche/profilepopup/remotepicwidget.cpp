/*
kafiche client : popup profile for incoming calls
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
#include <QStackedLayout>
#include <QUrl>
#include <QPixmap>
#include <QDebug>
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
	}
	if(!m_tempFile)
	{
		m_tempFile = new QTemporaryFile(this);
		m_tempFile->setAutoRemove(true);
	}
	m_tempFile->open();
	qDebug() << m_tempFile->fileName();
	m_http->setHost(url.host(), url.port() != -1 ? url.port() : 80);
	if (!url.userName().isEmpty())
		m_http->setUser(url.userName(), url.password());
	//httpRequestAborted = false;
	int httpGetId = m_http->get(url.path(), m_tempFile);
	qDebug() << httpGetId;
	// faut envoyer les données sur un fichier temporaire ?
	// QTemporaryFile().open() et filename() apres.
}

void RemotePicWidget::httpDataReadProgress(int /*bytesRead*/, int /*totalBytes*/)
{
//	qDebug() << (100*bytesRead/totalBytes) << "%"
//	         << bytesRead << "out of" << totalBytes;
}

void RemotePicWidget::httpRequestFinished(int requestId, bool error)
{
	qDebug() << "httpRequestFinished" << requestId << error;
	QPixmap pixmap(m_tempFile->fileName());	// ?
	m_tempFile->close();
	m_label->setPixmap(pixmap);
}

void RemotePicWidget::httpReadResponseHeader(const QHttpResponseHeader &responseHeader)
{
	qDebug() << "RemotePicWidget::httpReadResponseHeader"
	         << responseHeader.statusCode();
}


