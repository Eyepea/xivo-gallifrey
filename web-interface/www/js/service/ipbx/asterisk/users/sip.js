/*
 * XiVO Web-Interface
 * Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

xivo_ast_user_protocol_elt['sip'] = {
	'sip-protocol-nat': {it: true, fd: true},
	'protocol-progressinband': {it: true, fd: true},
	'sip-protocol-dtmfmode': {it: true, fd: true},
	'protocol-rfc2833compensate': {it: true, fd: true},
	'sip-protocol-qualify': {it: true, fd: true},
	'protocol-rtptimeout': {it: true, fd: true},
	'protocol-rtpholdtimeout': {it: true, fd: true},
	'protocol-rtpkeepalive': {it: true, fd: true},
	'protocol-allowtransfer': {it: true, fd: true},
	'protocol-autoframing': {it: true, fd: true},
	'protocol-videosupport': {it: true, fd: true},
	'protocol-maxcallbitrate': {it: true, fd: true},
	'protocol-g726nonstandard': {it: true, fd: true},
	'protocol-t38pt-udptl': {it: true},
	'protocol-t38pt-rtp': {it: true},
	'protocol-t38pt-tcp': {it: true},
	'protocol-t38pt-usertpsource': {it: true},
	'protocol-insecure': {it: true, fd: true},
	'protocol-trustrpid': {it: true, fd: true},
	'protocol-sendrpid': {it: true, fd: true},
	'protocol-allowsubscribe': {it: true, fd: true},
	'protocol-allowoverlap': {it: true, fd: true},
	'protocol-promiscredir': {it: true, fd: true},
	'protocol-usereqphone': {it: true, fd: true},
	'protocol-canreinvite': {it: true, fd: true},
	'protocol-fromuser': {it: true, fd: true},
	'protocol-fromdomain': {it: true, fd: true},
	'sip-protocol-amaflags': {it: true, fd: true},
	'protocol-useclientcode': {it: true, fd: true},

	'protocol-subscribemwi': {fd: true},
	'protocol-buggymwi': {fd: true},

	'autoprov-modact': {it: true},
	'autoprov-vendormodel': {it: true},
	'autoprov-macaddr': {it: true},
//	'protocol-name': {it: 'error'}
};
