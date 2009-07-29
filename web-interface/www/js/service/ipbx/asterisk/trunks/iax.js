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

var xivo_ast_trunks_elt_default = {
	'protocol-username': {it: true, fd: true},
	'protocol-language': {it: true, fd: true},
	'protocol-qualify': {it: true, fd: true},
	'protocol-qualifysmoothing': {it: true, fd: true},
	'protocol-qualifyfreqok': {it: true, fd: true},
	'protocol-qualifyfreqnotok': {it: true, fd: true},
	'protocol-timezone': {it: true, fd: true},
	'protocol-codecpriority': {it: true, fd: true},
	'protocol-sendani': {it: true, fd: true},
	'protocol-port': {it: true, fd: true},
	'protocol-mask': {it: true, fd: true},
	'protocol-outkey': {it: true, fd: true},
	'protocol-maxauthreq': {it: true, fd: true},
	'protocol-amaflags': {it: true, fd: true},
	'protocol-accountcode': {it: true, fd: true},
	'protocol-host-static': {it: true, fd: true},
	'protocol-host-type': {it: true, fd: true}};

var xivo_ast_trunk_type_elt = {};
xivo_ast_trunk_type_elt['peer'] = {
	'protocol-language': {it: false, fd: false},
	'protocol-codecpriority': {it: false, fd: false},
	'protocol-maxauthreq': {it: false, fd: false},
	'protocol-amaflags': {it: false, fd: false},
	'protocol-accountcode': {it: false, fd: false}};
xivo_ast_trunk_type_elt['friend'] = {};
xivo_ast_trunk_type_elt['user'] = {
	'protocol-username': {it: false, fd: false},
	'protocol-qualify': {it: false, fd: false},
	'protocol-qualifysmoothing': {it: false, fd: false},
	'protocol-qualifyfreqok': {it: false, fd: false},
	'protocol-qualifyfreqnotok': {it: false, fd: false},
	'protocol-timezone': {it: false, fd: false},
	'protocol-sendani': {it: false, fd: false},
	'protocol-port': {it: false, fd: false},
	'protocol-mask': {it: false, fd: false},
	'protocol-outkey': {it: false, fd: false},
	'protocol-host-static': {it: false, fd: false},
	'protocol-host-type': {it: {property: {disabled: true,
						  className: 'it-readonly',
						  value: 'dynamic'}},
				  fd: true}};
