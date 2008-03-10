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

#ifndef __PEERWIDGET_H__
#define __PEERWIDGET_H__

#include <QWidget>
#include <QPoint>

#include "peerchannel.h"

class QLabel;
class QPixmap;

class BaseEngine;
class PeerChannel;

/*! \brief Widget to display a Peer status
 *
 * Display Icons for the state and the peer name.
 */
class PeerWidget : public QWidget
{
	Q_OBJECT
public:
	PeerWidget(const QString & id,
                   const QString & name,
                   const QPixmap *,
                   const QPixmap *,
                   const QPixmap *,
                   const QPixmap *,
                   const QPixmap *,
                   const QPixmap *,
                   const QPixmap *,
                   const QPixmap *,
                   const QPixmap *,
                   const QPixmap *,
                   const QPixmap *,
                   const QPixmap *);
        //	           QWidget * parent = 0/*, int size = 16*/);
	~PeerWidget();
	void clearChanList();
	void addChannel(const QString &, const QString &, const QString &);
	void setName(const QString &);
	void setEngine(BaseEngine *);
protected:
	void mouseMoveEvent(QMouseEvent * event);
	void mousePressEvent(QMouseEvent * event);
        void mouseDoubleClickEvent(QMouseEvent * event);
	void dragEnterEvent(QDragEnterEvent * event);
	void dragMoveEvent(QDragMoveEvent * event);
	void dropEvent(QDropEvent * event);
	void contextMenuEvent(QContextMenuEvent *);
signals:
	//! originate signal
	void originateCall(const QString &, const QString &);
	//! transfer signal
	void transferCall(const QString &, const QString &);
	//! atxfer signal
	void atxferCall(const QString &, const QString &);
	//! intercept signal
	void interceptChan(const QString &);
	//! hang up signal
	void hangUpChan(const QString &);
	//! dial/call signal
	void emitDial(const QString &);
	//! hide the widget in the channel
	void doRemoveFromPanel(const QString &);
public slots:
	//! set phone or person icon in blue
	void setBlue(int n);
	//void setCyan(int n);
	//! set phone or person icon in gray
	void setGray(int n);
	//! set phone or person icon in green
	void setGreen(int n);
	//! set phone or person icon in orange
	void setOrange(int n);
	//! set phone or person icon in red
	void setRed(int n);
	//! set phone or person icon in yellow
	void setYellow(int n);
	//void setBlack(int n);
	//void setDarkGreen(int n);
	void updateMyCalls(const QStringList &, const QStringList &, const QStringList &);
private slots:
	void transferChan(const QString &);
	void removeFromPanel();
	void dial();
private:
	BaseEngine * m_engine;  //!< Base Engine reference
	QLabel * m_statelbl;	//!< Peer state display (ringing, online, ...)
	QLabel * m_availlbl;	//!< Peer state display from XIVO CTI Client
	QLabel * m_voicelbl;
	QLabel * m_fwdlbl;
	QLabel * m_textlbl;		//!< text label : to display peer name
	//QPixmap m_square;		//!< pixmap used to display the states
	QPoint m_dragstartpos;	//!< drag start position
	QString m_id;	//!< peer id : asterisk/protocol/extension
	QString m_name;	//!< caller id to display : usualy the NAME of the person

	QAction * m_removeAction;	//!< action to remove this peer from the window
	QAction * m_dialAction;		//!< action to dial this number
	
	QList<PeerChannel *> m_channels;	//!< channels associated with THIS peer
	QList<PeerChannel *> m_mychannels;	//!< channels assiciated with ME

	/* TODO : have the Pixmaps as static objects */
	const QPixmap *m_phone_green;	//!< green phone icon
	const QPixmap *m_phone_red;	//!< red phone icon
	const QPixmap *m_phone_orange;	//!< orange phone icon
	const QPixmap *m_phone_grey;	//!< grey phone icon
	const QPixmap *m_phone_yellow;	//!< yellow phone icon
	const QPixmap *m_phone_blue;	//!< blue phone icon

	const QPixmap *m_person_green;	//!< green person icon
	const QPixmap *m_person_red;	//!< red person icon
	const QPixmap *m_person_orange;	//!< orange person icon
	const QPixmap *m_person_grey;	//!< grey person icon
	const QPixmap *m_person_yellow;	//!< yellow person icon
	const QPixmap *m_person_blue;	//!< blue person icon
};

#endif

