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

#include <QContextMenuEvent>
#include <QDebug>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QTableWidget>
#include <QTimerEvent>
#include <QVBoxLayout>

#include "baseengine.h"
#include "extendedtablewidget.h"
#include "parkingpanel.h"
//#include "peerchannel.h"
#include "userinfo.h"

/*! \brief Constructor
 *
 * Set up the layout and the table widget.
 */
ParkingPanel::ParkingPanel(BaseEngine * engine,
                           const QVariant & options,
                           QWidget * parent)
    : QWidget(parent), m_engine(engine)
{
    QVBoxLayout * vlayout = new QVBoxLayout(this);
    vlayout->setMargin(0);
    m_table = new ExtendedTableWidget( m_engine, this );
    qDebug() << m_table;
    m_table->setAlternatingRowColors(true);
    m_table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    m_table->setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );
    m_table->setColumnCount( 4 );
    QStringList labels = (QStringList() << /*tr("XIVO Id") << */tr("Number") << tr("Time") << tr("Parked") << tr("Parker"));
    m_table->setHorizontalHeaderLabels(labels);
        
    connect( m_table, SIGNAL(itemClicked(QTableWidgetItem *)),
             this, SLOT(itemClicked(QTableWidgetItem *)) );
    connect( m_table, SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
             this, SLOT(itemDoubleClicked(QTableWidgetItem *)) );
    // forward SIGNAL
    connect( m_table, SIGNAL(actionCall(const QString &, const QString &, const QString &)),
             this, SIGNAL(actionCall(const QString &, const QString &, const QString &)) );
    vlayout->addWidget( m_table, 0 );
    m_table->resizeColumnsToContents();
    m_timerid = 0;
    m_deltasec = 2;
    setGuiOptions(options);
}

ParkingPanel::~ParkingPanel()
{
    // qDebug() << "ParkingPanel::~ParkingPanel()";
    //delete m_table;
}

/*! \brief add a message to the list
 *
 * add a new message.
 */
void ParkingPanel::parkingEvent(const QVariant & subcommand)
{
    QVariantMap map = subcommand.toMap();
    qDebug() << "ParkingPanel::parkingEvent()" << map;
    QString eventkind = map["status"].toString();
    QString astid = map["astid"].toString();
    QString channel = map["channel"].toString();
    QString fromchannel = map["fromchannel"].toString();
    QString parkplacenum = map["exten"].toString();
    QString seconds = map["timeout"].toString();
    QStringList newpark = map["args"].toStringList();
    QString calleridnum = map["calleridnum"].toString();
    QString calleridname = map["calleridname"].toString();
    QString fromcalleridnum = map["fromcalleridnum"].toString();
    QString fromcalleridname = map["fromcalleridname"].toString();

    // ignore buggy events 
    if(fromchannel == channel)
        return;

    //QString parkedpeer = channel.split("-")[0];
    QString parkedpeer = calleridname + " (" + calleridnum + ")";
    QString parkedby = fromchannel.split("-")[0];
    if(!fromcalleridnum.isEmpty())
        parkedby = fromcalleridname + " (" + fromcalleridnum + ")";
    
    if(eventkind == "parkedcall") {
        for(int m = 0; m < m_table->rowCount(); m++) {
            if (m_table->item(m, 0)->data(Qt::UserRole+0).toString() == channel
               && m_table->item(m, 0)->data(Qt::UserRole+1).toString() == astid) {
                // do not add the same entry twice !
                return;
            }
        }
        int i = 0;
        m_table->insertRow( 0 );
        //QTableWidgetItem * item0 = new QTableWidgetItem( astid );
        //item0->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
        //m_table->setItem( 0, 0, item0 );
        QTableWidgetItem * item1 = new QTableWidgetItem( parkplacenum );
        item1->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
        item1->setData(Qt::UserRole+0, channel);
        item1->setData(Qt::UserRole+1, astid);
        m_table->setItem( 0, i++, item1 );
        QTableWidgetItem * item2 = new QTableWidgetItem( seconds + " s" );
        item2->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
        m_table->setItem( 0, i++, item2 );
        QTableWidgetItem * item3 = new QTableWidgetItem( parkedpeer );
        item3->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
        m_table->setItem( 0, i++, item3 );
        QTableWidgetItem * item4 = new QTableWidgetItem( parkedby );
        item4->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
        m_table->setItem( 0, i++, item4 );

        emit newParkEvent();
        // do not start another timer if there is already one running
        if(m_timerid == 0)
            m_timerid = startTimer(m_deltasec * 1000);
    } else if ((eventkind == "unparkedcall") ||
               (eventkind == "parkedcalltimeout") ||
               (eventkind == "parkedcallgiveup")) {
        for(int m = 0; m < m_table->rowCount(); m++) {
            if (m_table->item(m, 0)->data(Qt::UserRole+0).toString() == channel
               &&m_table->item(m, 0)->data(Qt::UserRole+1).toString() == astid) {
                m_table->removeRow(m);
                // kills the timer only if there are no parked calls left
                if(m_table->rowCount() == 0) {
                    killTimer(m_timerid);
                    m_timerid = 0;
                }
            }
        }
    }
    // make columns fit the content
    m_table->resizeColumnsToContents();
}

void ParkingPanel::itemClicked(QTableWidgetItem * item)
{
    int rown   = m_table->row(item);
    QString astid    = m_table->item(rown, 0)->data(Qt::UserRole+1).toString();
    QString placenum = m_table->item(rown, 0)->text();
    //qDebug() << "ParkingPanel::itemClicked" << rown << astid << placenum;
    if(m_engine && m_engine->getXivoClientUser() && astid == m_engine->getXivoClientUser()->astid())
        emit copyNumber(placenum);
}

void ParkingPanel::itemDoubleClicked(QTableWidgetItem * item)
{
    int rown   = m_table->row(item);
    QString astid    = m_table->item(rown, 0)->data(Qt::UserRole+1).toString();
    QString placenum = m_table->item(rown, 0)->text();
    //qDebug() << "ParkingPanel::itemDoubleClicked" << rown << astid << placenum;
    if(m_engine && m_engine->getXivoClientUser() && astid == m_engine->getXivoClientUser()->astid())
        emit actionCall("originate", "user:special:me", "ext:" + placenum); // Call
}

void ParkingPanel::timerEvent(QTimerEvent * event)
{
    int timerId = event->timerId();
    if (timerId == m_timerid)
        for(int i = 0; i < m_table->rowCount(); i++) {
            QTableWidgetItem * item = m_table->takeItem(i, 1);
            int leftsec = item->text().split(" ")[0].toInt() - m_deltasec;
            item->setText( QString::number(leftsec) + " s" );
            m_table->setItem( i, 1, item );
        }
}

