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

#ifndef __SWITCHBOARDWINDOW_H__
#define __SWITCHBOARDWINDOW_H__

#include <QHash>
#include <QList>
#include <QVariant>
#include <QWidget>

class QGridLayout;
class QMouseEvent;

class BaseEngine;
class PeerItem;
class PeersLayout;
class BasePeerWidget;
class PeerWidgetFactory;
class UserInfo;
class Group;

/*! \brief Widget displaying PeerItems
 *
 * This widget use a PeersLayout to display PeerItems in a grid.
 */
class SwitchBoardWindow : public QWidget
{
    Q_OBJECT
public:
    SwitchBoardWindow(BaseEngine *, QWidget * parent = 0);
    ~SwitchBoardWindow();        //!< Destructor
protected:
    // event handlers
    void mousePressEvent(QMouseEvent *); //!< Catch mouse press events
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *event);
    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent *);
    void dragMoveEvent(QDragMoveEvent *);
    void contextMenuEvent(QContextMenuEvent *);
public slots:
    void setGuiOptions(const QVariant &);
    void setUserInfo(const UserInfo *);
    void updateUser(UserInfo *);
    void updatePeerAgent(double,
                         const QString &,
                         const QString &,
                         const QVariant &);
    void removePeer(const QString &);
    void removePeers();
private slots:
    void removePeerFromLayout();
    void removeGroup();
    void changeGroupColor();
    void changeGroupName();
    void addPhoneNumberEntry();
private:
    void saveGroups() const;
    void savePositions() const;
    void reloadGroups();
    void reloadExternalPhones();
    BasePeerWidget * addPeerWidget(PeerItem * peeritem, const QPoint & pos);
    Group * getGroup( const QPoint & ) const;
    BaseEngine * m_engine;        //!< engine to connect to peer widgets
    QHash<QString, PeerItem *> m_peerhash;        //!< PeerItem hash
    PeersLayout * m_layout;                        //!< Grid Layout for displaying peers
    PeerWidgetFactory * m_peerwidgetfactory;    //!< to build *PeerWidget objects
    // for the groups of people :
    bool m_trace_box;   //!< is box drawing enable
    QPoint m_first_corner;  //!< first corner of the box being drawn
    QPoint m_second_corner; //!< second corner of the box being drawn
    QList<Group *> m_group_list;    //!< list of the groups
    Group * m_group_to_resize;  //!< group being moved/resized
    enum {ETop=1, EBottom, ERight, ELeft, EMove} m_group_resize_mode;
};

#endif
