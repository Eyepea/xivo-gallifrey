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

#ifndef __XIVOCONSTS_H__
#define __XIVOCONSTS_H__

#define CHANNEL_MIMETYPE  "XIVO_ASTERISK_CHANNEL"
#define PEER_MIMETYPE     "XIVO_ASTERISK_PEER"
#define NUMBER_MIMETYPE   "XIVO_ASTERISK_NUMBER"
#define XIVO_COMMAND_ROOT "XIVO_COMMAND_ROOT"

#define CHAN_STATUS_READY "ready"
#define CHAN_STATUS_HANGUP "hangup"
#define CHAN_STATUS_CALLING "calling"
#define CHAN_STATUS_RINGING "ringing"
#define CHAN_STATUS_LINKED_CALLER "linked-caller"
#define CHAN_STATUS_LINKED_CALLED "linked-called"
#define CHAN_STATUS_UNLINKED_CALLER "unlinked-caller"
#define CHAN_STATUS_UNLINKED_CALLED "unlinked-called"

const int REQUIRED_SERVER_VERSION = 4560;
const QString __required_server_version__ = QString::number(REQUIRED_SERVER_VERSION);
const QString __current_client_version__  = SVNVER;
const QString __xivo_version__  = "0.4";
const QString __nopresence__ = "nopresence";
const QStringList XletList = (QStringList() << "customerinfo" << "features" << "history"
                              << "directory" << "search" << "fax" << "dial"
                              << "operator" << "parking" << "calls" << "switchboard"
                              << "messages" << "identity" << "datetime" << "tabber" << "conference" << "xletproto" << "callcampaign" << "mylocaldir"
#ifdef USE_WEBKIT
                              << "xletweb"
#endif /* USE_WEBKIT */
#ifdef USE_OUTLOOK
                              << "outlook"
#endif /* USE_OUTLOOK */
                              << "agents" << "agentdetails" << "queues" << "queuedetails" << "queueentrydetails");
const QStringList CheckFunctions = (QStringList() << "presence" << "customerinfo");

#endif /* __XIVOCONSTS_H__ */
