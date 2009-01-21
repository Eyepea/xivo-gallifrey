/*
 * XiVO Web-Interface
 * Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
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

xivo_ast_user_protocol_elt['custom'] = {
	'protocol-name': {it: false},
	'protocol-secret': {it: false},
	'protocol-interface': {it: true, fd: true},
	'protocol-qualify': {it: false},
	'protocol-disallow': {it: false},
	'protocol-callerid': {it: false},
	'protocol-host-dynamic': {it: false},
	'protocol-host-static': {it: false},
	'protocol-permit': {it: false},
	'protocol-deny': {it: false},
	'protocol-accountcode': {it: false},

	'ufeatures-enableclient': {it: false},
	'ufeatures-loginclient': {it: false},
	'ufeatures-passwdclient': {it: false},
	'ufeatures-profileclient': {it: false},
	'ufeatures-enablehint': {it: false},
	'ufeatures-enablevoicemail': {it: false},
	'ufeatures-enablexfer': {it: false},
	'ufeatures-enableautomon': {it: false},
	'ufeatures-callrecord': {it: false},
	'ufeatures-callfilter': {it: false},
	'ufeatures-enablednd': {it: false},
	'ufeatures-enablerna': {it: false},
	'ufeatures-destrna': {it: false},
	'ufeatures-enablebusy': {it: false},
	'ufeatures-destbusy': {it: false},
	'ufeatures-enableunc': {it: false},
	'ufeatures-destunc': {it: false},
	'ufeatures-agentid': {it: false},

	'ufeatures-voicemailid': {it: false},

	'codec-active': {it: false},
	'codeclist': {it: {style: {display: 'inline'}, property: {disabled: true, className: 'it-disabled'}}},
	'codec': {it: {style: {display: 'inline'}, property: {disabled: true, className: 'it-disabled'}}},

	'rightcalllist': {it: false},
	'rightcall': {it: false}};
