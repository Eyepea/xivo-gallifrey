/* XiVO Client
 * Copyright (C) 2007-2010, Proformatique
 *
 * This file is part of XiVO Client.
 *
 * XiVO Client is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, with a Section 7 Additional
 * Permission as follows:
 *   This notice constitutes a grant of such permission as is necessary
 *   to combine or link this software, or a modified version of it, with
 *   the OpenSSL project's "OpenSSL" library, or a derivative work of it,
 *   and to copy, modify, and distribute the resulting work. This is an
 *   extension of the special permission given by Trolltech to link the
 *   Qt code with the OpenSSL library (see
 *   <http://doc.trolltech.com/4.4/gpl.html>). The OpenSSL library is
 *   licensed under a dual license: the OpenSSL License and the original
 *   SSLeay license.
 *
 * XiVO Client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XiVO Client.  If not, see <http://www.gnu.org/licenses/>.
 */

/* $Revision$
 * $Date$
 */

#ifndef __QUEUESPANEL_H__
#define __QUEUESPANEL_H__

#include <QObject>
#include <QHash>
#include <QLabel>
#include <QList>
#include <QMap>
#include <QVariant>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QLineEdit>
#include <QSpinBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QCheckBox>
#include <QGridLayout>
#include <QProgressBar>
#include <QPushButton>

#include "xlet.h"
#include "queueinfo.h"


class UserInfo;
class QueuesPanel;

/*! \brief to configure if the queue should be shown and the queue 
 *  stats parameters
 */
class QueuesPanelConfigure : public QWidget
{
    Q_OBJECT

    public:
        QueuesPanelConfigure(QueuesPanel *xlet);
        QWidget* buildConfigureQueueList(QWidget *);

    protected:
        virtual void closeEvent(QCloseEvent *);

    private slots:
        void changeQueueStatParam(int);
};

class QueueRow : public QWidget {
    Q_OBJECT

    public:
        QueueRow(const QueueInfo *info, QueuesPanel *xlet);
        void update();
        void updateSliceStat(const QString &stat, const QString &value);
        void updateLongestWaitWidget(int display, uint greenlevel, uint orangelevel);
        void updateBusyWidget();
        void updateName();

        static QWidget* makeTitleRow(QWidget *parent);
        static void setLayoutColumnWidth(QGridLayout *layout, int nbStat);

    private:
        QLabel *m_name;  //!< to display the name of queue
        QPushButton *m_more;  //!< to display queue details
        QLabel *m_longestWait;  //!< to display the longuest waiting time for each queue
        QPushButton *m_move;  //!< to change the order in which the queue are displayed
        QProgressBar *m_busy;  //!< to display the queues busy level
        QHash<QString, QLabel *> m_infoList; //!< the stats info
        static uint m_maxbusy;  //!< Maximum value for busy level
        QGridLayout *m_layout;
        const QueueInfo *qinfo;
        QueuesPanel *xlet;

};

/*! \brief Displays queues and their status
 */
class QueuesPanel : public XLet
{
    Q_OBJECT

    public:
        QueuesPanel(BaseEngine *, QWidget *parent=0);
        void eatQueuesStats(const QVariantMap &p);
        static void eatQueuesStats_t(const QVariantMap &p, void *udata) {
            ((QueuesPanel*)udata)->eatQueuesStats(p);
        };

        bool showMoreQueueDetailButton() { return m_showMore; };
        bool showNumber() { return m_showNumber; };

    protected:
        virtual void contextMenuEvent(QContextMenuEvent *);

    private:
        void openConfigureWindow();
        void saveQueueOrder(const QStringList &);
        void setQueueOrder(const QStringList &);
        void loadQueueOrder();                      //!< request load of queue order

    signals:
        void changeWatchedQueue(const QString &);   //!< Watch this queue

    public slots:
        void removeQueues(const QString &, const QStringList &);
        void newQueueList(const QStringList &);
        void settingChanged(const QVariantMap &);
        void updateLongestWaitWidgets();
        void askForQueueStats();
        void queueClicked();

    private:
        bool m_showMore;
        bool m_showNumber;
        QueuesPanelConfigure *m_configureWindow;
        
        QVBoxLayout *m_layout;
        QHash<QString, QueueRow *> m_queueList;
};

#endif
