/* XIVO CTI clients
 * Copyright (C) 2007-2009  Proformatique
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Linking the Licensed Program statically or dynamically with other
 * modules is making a combined work based on the Licensed Program. Thus,
 * the terms and conditions of the GNU General Public License version 2
 * cover the whole combination.
 *
 * In addition, as a special exception, the copyright holders of the
 * Licensed Program give you permission to combine the Licensed Program
 * with free software programs or libraries that are released under the
 * GNU Library General Public License version 2.0 or GNU Lesser General
 * Public License version 2.1 or any later version of the GNU Lesser
 * General Public License, and with code included in the standard release
 * of OpenSSL under a version of the OpenSSL license (with original SSLeay
 * license) which is identical to the one that was published in year 2003,
 * or modified versions of such code, with unchanged license. You may copy
 * and distribute such a system following the terms of the GNU GPL
 * version 2 for the Licensed Program and the licenses of the other code
 * concerned, provided that you include the source code of that other code
 * when and as the GNU GPL version 2 requires distribution of source code.
 */

/* $Revision$
 * $Date$
 */

#include <QHBoxLayout>
#include <QGridLayout>
#include <QApplication>
#include <QLabel>
#include <QMouseEvent>
#include <QDebug>
#include <QFont>
#include <QTextFormat>
#include <QMenu>

#include "callstackwidget.h"
#include "callwidget.h"
#include "userinfo.h"
#include "xivoconsts.h"

// Static members initialization
QPixmap * CallWidget::m_call_yellow = NULL;
QPixmap * CallWidget::m_call_blue   = NULL;
QPixmap * CallWidget::m_call_red    = NULL;
QPixmap * CallWidget::m_call_gray   = NULL;

/*! \brief Constructor
 *
 * set up the widget, start timer.
 */
CallWidget::CallWidget(UserInfo * ui, const QString & channelme,
                       const QString & status, uint ts,
                       const QString & channelpeer, const QString & callerid,
                       const QString & calleridname,
                       QWidget * parent)
    : QWidget(parent), m_square(16,16)
{
    // qDebug() << "CallWidget::CallWidget()" << channelme;
    if(m_call_yellow == NULL)
    {
        m_call_yellow = new QPixmap(":/images/phone-yellow.png");
        m_call_blue   = new QPixmap(":/images/phone-blue.png");
        m_call_red    = new QPixmap(":/images/phone-red.png");
        m_call_gray   = new QPixmap(":/images/phone-grey.png");
    }
    m_ui = ui;
    QGridLayout * gridlayout = new QGridLayout(this);
    
    m_channelme = channelme;
    m_channelpeer = channelpeer;
    
    //        qDebug() << "spacing" << gridlayout->spacing()
    //                 << ", margin" << gridlayout->margin();
    //        gridlayout->setSpacing(0);
    //gridlayout->setMargin(0);
    
    gridlayout->setColumnStretch(3, 1);
    m_lbl_status = new QLabel(this);
    gridlayout->addWidget(m_lbl_status, 0, 0);
        
    m_lbl_time = new QLabel(this);
    m_lbl_time->setFont(QFont("", 8, QFont::Bold));
    m_startTime = QDateTime::fromTime_t(ts);
    startTimer(1000);
    gridlayout->addWidget(m_lbl_time, 1, 0, 1, 3);
        
    m_lbl_direction = new QLabel(this);
    gridlayout->addWidget(m_lbl_direction, 0, 1);
        
    m_lbl_exten = new QLabel(this);
    m_lbl_exten->setFont(QFont("courier", 10, QFont::Light));
    gridlayout->addWidget(m_lbl_exten, 0, 2);
        
    updateWidget(status, ts, "cpeer", callerid, calleridname);
        
    m_hangUpAction = new QAction( tr("&Hangup"), this);
    m_hangUpAction->setStatusTip( tr("Hang up/Close the channel") );
    connect( m_hangUpAction, SIGNAL(triggered()),
             this, SLOT(hangUp()) );
        
    m_transferToNumberAction = new QAction( tr("&Transfer to number"), this);
    m_transferToNumberAction->setStatusTip( tr("Transfer the channel to the dialed number") );
    connect( m_transferToNumberAction, SIGNAL(triggered()),
             this, SLOT(transferToNumber()) );
        
    m_parkCall = new QAction( tr("&Park the call"), this);
    m_parkCall->setStatusTip( tr("Park this call") );
    connect( m_parkCall, SIGNAL(triggered()),
             this, SLOT(parkCall()) );
}

/*! \brief Destructor
 */
CallWidget::~CallWidget()
{
    // qDebug() << "CallWidget::~CallWidget()";
}

/*! \brief update time displayed in m_lbl_time
 */
void CallWidget::updateCallTimeLabel()
{
    int time = m_startTime.secsTo(QDateTime::currentDateTime());
    m_lbl_time->setText( "[" + QString::number(time/60) + " min "
                         + QString::number(time%60) + " s]" );
}

/*! \brief timer event
 *
 * update the time displayed.
 */
void CallWidget::timerEvent(QTimerEvent * /*event*/)
{
    // event->timerId();
    updateCallTimeLabel();
}

/*! \brief update displayed stuff
 */
void CallWidget::updateWidget(const QString & status,
                              uint ts,
                              const QString & channelpeer,
                              const QString & callerid,
                              const QString & calleridname)
{
    //qDebug() << "CallWidget::updateWidget()" << status << time << exten;
    setActionPixmap(status);
    m_channelpeer = channelpeer;
    //qDebug() << time << m_startTime << m_startTime.secsTo(QDateTime::currentDateTime());
    //m_startTime = QDateTime::currentDateTime().addSecs(-time);
    m_startTime = QDateTime::fromTime_t(ts);
    updateCallTimeLabel();
    if ((status == CHAN_STATUS_CALLING) || (status == CHAN_STATUS_LINKED_CALLER))
        m_lbl_direction->setPixmap(QPixmap(":/images/rightarrow.png"));
    else if ((status == CHAN_STATUS_RINGING) || (status == CHAN_STATUS_LINKED_CALLED))
        m_lbl_direction->setPixmap(QPixmap(":/images/leftarrow.png"));
    else
        qDebug() << "CallWidget::updateWidget() : status unknown" << status;
        
    QString text = tr("Unknown");
    if(calleridname == "<meetme>")
        text = tr("Conference room number %1").arg(callerid);
    else if(calleridname != "<unknown>" && !calleridname.isEmpty())
        text = tr("%1 : %2").arg(callerid).arg(calleridname);
    else if(!callerid.isEmpty())
        text = callerid;
    m_lbl_exten->setText(text);
}

/*! \brief set icon depending on status
 */
void CallWidget::setActionPixmap(const QString & status)
{
    if (status == CHAN_STATUS_CALLING)
        m_lbl_status->setPixmap( *m_call_yellow );
    else if (status == CHAN_STATUS_RINGING)
        m_lbl_status->setPixmap( *m_call_blue );
    else if ((status == "On the phone") || (status == "Up"))
        m_lbl_status->setPixmap( *m_call_red );
    else if (status == CHAN_STATUS_LINKED_CALLER)
        m_lbl_status->setPixmap( *m_call_red );
    else if (status == CHAN_STATUS_LINKED_CALLED)
        m_lbl_status->setPixmap( *m_call_red );
    else {
        m_lbl_status->setPixmap( *m_call_gray );
        qDebug() << "CallWidget::setActionPixmap() : status unknown" << status;
    }
}

/*! \brief mouse press event
 *
 * store mouse position for drag&drop.
 */
void CallWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragstartpos = event->pos();
    }
}

/*! \brief mouse move event
 *
 * start drag if left button pressed and if the
 * mouse has been moved enough.
 */
void CallWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - m_dragstartpos).manhattanLength()
        < QApplication::startDragDistance())
        return;

    qDebug() << "CallWidget::mouseMoveEvent() starting DRAG" << m_channelme ;

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData();
    //mimeData->setText(m_channelme); // XXX
    mimeData->setText(m_channelpeer); // XXX
    mimeData->setData(USERID_MIMETYPE, m_ui->userid().toAscii());
    //mimeData->setData(CHANNEL_MIMETYPE, m_channelme.toAscii());
    mimeData->setData(CHANNEL_MIMETYPE, m_channelpeer.toAscii());
    drag->setMimeData(mimeData);

    Qt::DropAction dropAction = drag->start(Qt::CopyAction | Qt::MoveAction);
    qDebug() << "dropAction =" << dropAction;
}

/*! \brief hang up the channel
 */
void CallWidget::hangUp()
{
    qDebug() << "CallWidget::hangUp()" << m_channelme;
    doHangUp( m_channelme );
}

/*! \brief transfers the channel to a number
 */
void CallWidget::transferToNumber()
{
    qDebug() << "CallWidget::transferToNumber()" << m_channelpeer;
    doTransferToNumber( m_channelpeer);
}

/*! \brief transfers the channel to a number
 */
void CallWidget::parkCall()
{
    qDebug() << "CallWidget::parkCall()" << m_channelme;
    doParkCall( m_channelme );
}

/*! \brief open the context menu
 */
void CallWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu contextMenu;
    contextMenu.addAction(m_hangUpAction);
    // m_transferToNumberAction only if there is something written
    contextMenu.addAction(m_transferToNumberAction);
    contextMenu.addAction(m_parkCall);
    contextMenu.exec(event->globalPos());
}

/*! \brief return m_channelme
 */
const QString & CallWidget::channel() const
{
    return m_channelme;
}

