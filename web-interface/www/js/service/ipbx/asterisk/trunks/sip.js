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

var xivo_ast_trunks_elt_default = {
	'protocol-username': {it: true, fd: true},
	'protocol-qualify': {it: true, fd: true},
	'protocol-rtptimeout': {it: true, fd: true},
	'protocol-rtpholdtimeout': {it: true, fd: true},
	'protocol-rtpkeepalive': {it: true, fd: true},
	'protocol-g726nonstandard': {it: true, fd: true},
	'protocol-port': {it: true, fd: true},
	'protocol-usereqphone': {it: true, fd: true},
	'protocol-fromuser': {it: true, fd: true},
	'protocol-fromdomain': {it: true, fd: true},
	'protocol-host-static': {it: true, fd: true},
	'protocol-host-type': {it: true, fd: true}};

var xivo_ast_trunk_type_elt = {};
xivo_ast_trunk_type_elt['peer'] = {};
xivo_ast_trunk_type_elt['friend'] = {};
xivo_ast_trunk_type_elt['user'] = {
	'protocol-username': {it: false, fd: false},
	'protocol-qualify': {it: false, fd: false},
	'protocol-rtptimeout': {it: false, fd: false},
	'protocol-rtpholdtimeout': {it: false, fd: false},
	'protocol-rtpkeepalive': {it: false, fd: false},
	'protocol-g726nonstandard': {it: false, fd: false},
	'protocol-port': {it: false, fd: false},
	'protocol-usereqphone': {it: false, fd: false},
	'protocol-fromuser': {it: false, fd: false},
	'protocol-fromdomain': {it: false, fd: false},
	'protocol-host-static': {it: false, fd: false},
	'protocol-host-type': {it: {property: {disabled: true,
						  className: 'it-readonly',
						  value: 'dynamic'}},
				  fd: true}};
