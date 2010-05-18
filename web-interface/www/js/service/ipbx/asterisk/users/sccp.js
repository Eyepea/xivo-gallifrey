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

xivo_ast_user_protocol_elt['sccp'] = {
//	'protocol-name': {it: false},
//	'protocol-secret': {it: false},
	'protocol-host-type': {it: false},
	'protocol-host-static': {it: false},
	'protocol-disallow': {it: false},
//	'protocol-dtmfmode': {it: false, fd: false},
//	'protocol-nat': {it: false, fd: false},
	'protocol-t38pt-udptl': {it: false},
	'protocol-t38pt-rtp': {it: false},
	'protocol-t38pt-tcp': {it: false},
	'protocol-t38pt-usertpsource': {it: false},
	'protocol-allowtransfer': {it: true, fd: true},
	'protocol-park': {it: true, fd: true},
	'protocol-cfwdall': {it: true, fd: true},
	'protocol-cfwdbusy': {it: true, fd: true},
	'protocol-cfwdnoanswer': {it: true, fd: true},
	'protocol-pickupexten': {it: true, fd: true},
	'protocol-pickupcontext': {it: true}, //, fd: true},
	'protocol-pickupmodeanswer': {it: true, fd: true},
	'protocol-dnd': {it: true, fd: true},
	'protocol-directrtp': {it: true, fd: true},
	'protocol-earlyrtp': {it: true, fd: true},
	'protocol-private': {it: true, fd: true},
	'protocol-privacy': {it: true, fd: true},
	'protocol-mwilamp': {it: true, fd: true},
	'protocol-mwioncall': {it: true, fd: true},
	'protocol-echocancel': {it: true, fd: true},
	'protocol-silencesuppression': {it: true, fd: true},
	'protocol-incominglimit': {it: true, fd: true},
	'protocol-keepalive': {it: true, fd: true},
	'protocol-tzoffset': {it: true, fd: true},
	'protocol-imageversion': {it: true, fd: true},
	'protocol-trustphoneip': {it: true, fd: true},
	'protocol-secondary_dialtone_digits': {it: true, fd: true},
	'protocol-secondary_dialtone_tone': {it: true, fd: true},
	'protocol-audio_tos': {it: true, fd: true},
	'protocol-audio_cos': {it: true, fd: true},
	'protocol-video_tos': {it: true, fd: true},
	'protocol-video_cos': {it: true, fd: true},
	'protocol-adhocnumber': {it: true, fd: true},
	'sccp-protocol-nat': {it: true, fd: true},
	'sccp-protocol-amaflags': {it: true, fd: true},
	'sccp-protocol-dtmfmode': {it: true, fd: true},

	'codec-active': {it: false, fd: false},
	'codeclist': {it: {style: {display: 'inline'}, property: {disabled: true, className: 'it-disabled'}}},
	'codec': {it: {style: {display: 'inline'}, property: {disabled: true, className: 'it-disabled'}}},

	'autoprov-modact': {it: true},
	'autoprov-vendormodel': {it: true},
	'autoprov-macaddr': {it: true}
};
