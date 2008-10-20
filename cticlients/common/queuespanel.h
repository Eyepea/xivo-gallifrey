/* XIVO CTI clients
 * Copyright (C) 2007, 2008  Proformatique
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

#ifndef __QUEUESPANEL_H__
#define __QUEUESPANEL_H__

#include <QHash>
#include <QLabel>
#include <QList>
#include <QObject>
#include <QWidget>

class QGridLayout;
class QLabel;
class QProgressBar;
class QPushButton;

class BaseEngine;

/*! \brief Identity Display
 */
class QueuesPanel : public QWidget
{
	Q_OBJECT
public:
	QueuesPanel(BaseEngine *, QWidget * parent = 0);
	~QueuesPanel();
        void setEngine(BaseEngine *);
protected:
        void update();
private:
        void addQueue(const QString &, const QString &, bool);
signals:
        void changeWatchedQueue(const QString &);
public slots:
        void removeQueues(const QString &, const QStringList &);
	void setQueueList(bool, const QMap<QString, QVariant> &);
        void setQueueStatus(const QStringList &);
        void updatePeerAgent(const QString &,
                             const QString &,
                             const QStringList &);
private slots:
        void queueClicked();
private:
        QGridLayout * m_gridlayout;
        QStringList m_statitems;
        QHash<QString, QString> m_statlegends;

        QHash<QString, QLabel *> m_queuelabels;
        QHash<QString, QPushButton *> m_queuemore;
        QHash<QString, QProgressBar *> m_queuebusies;
        QHash<QString, QHash<QString, QLabel *> > m_queueinfos;

        BaseEngine * m_engine;

        quint32 m_maxbusy;
        
        QLabel * m_title1;
        QLabel * m_title2;
        QHash<QString, QLabel *> m_title_infos;
};

#endif /* __QUEUESPANEL_H__ */
