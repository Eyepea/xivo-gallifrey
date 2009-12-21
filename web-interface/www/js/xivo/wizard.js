/*
 * XiVO Web-Interface
 * Copyright (C) 2009  Proformatique <technique@proformatique.com>
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

var xivo_wz_fm_db_backend_elt = {
	'fd-db-file-xivo':	{'style':		{display: 'none'}},
	'it-db-file-xivo':	{'property':	{disabled: true}}, 
	'fd-db-file-ipbx':	{'style':		{display: 'none'}},
	'it-db-file-ipbx':	{'property':	{disabled: true}}, 
	'fd-db-host':	{'style':		{display: 'none'}},
	'it-db-host':	{'property':	{disabled: true}}, 
	'fd-db-host-port':	{'style':		{display: 'none'}},
	'it-db-host-port':	{'property':	{disabled: true}},
	'fd-db-dbname-xivo': {'style': {display: 'none'}},
	'it-db-dbname-xivo': {'property': {disabled: true}},
	'fd-db-user-xivo':	{'style':		{display: 'none'}},
	'it-db-user-xivo':	{'property':	{disabled: true}}, 
	'fd-db-pwd-xivo':	{'style':		{display: 'none'}},
	'it-db-pwd-xivo':	{'property':	{disabled: true}}, 
	'fd-db-dbname-ipbx': {'style': {display: 'none'}},
	'it-db-dbname-ipbx': {'property': {disabled: true}},
	'fd-db-user-ipbx':	{'style':		{display: 'none'}},
	'it-db-user-ipbx':	{'property':	{disabled: true}}, 
	'fd-db-pwd-ipbx':	{'style':		{display: 'none'}},
	'it-db-pwd-ipbx':	{'property':	{disabled: true}}, 
	'links':
		{link:	[['fd-db-file-xivo',0,1],
				 ['it-db-file-xivo',0,1],
				 ['fd-db-file-ipbx',0,1],
				 ['it-db-file-ipbx',0,1],
				 ['fd-db-host',0,1],
				 ['it-db-host',0,1],
				 ['fd-db-host-port',0,1],
				 ['it-db-host-port',0,1],
				 ['fd-db-dbname-xivo',0,1],
				 ['it-db-dbname-xivo',0,1],
				 ['fd-db-user-xivo',0,1],
				 ['it-db-user-xivo',0,1],
				 ['fd-db-pwd-xivo',0,1],
				 ['it-db-pwd-xivo',0,1],
				 ['fd-db-dbname-ipbx',0,1],
				 ['it-db-dbname-ipbx',0,1],
				 ['fd-db-user-ipbx',0,1],
				 ['it-db-user-ipbx',0,1],
				 ['fd-db-pwd-ipbx',0,1],
				 ['it-db-pwd-ipbx',0,1]
	 ]
		}
};

var xivo_wz_fm_db_backend = {'sqlite':	dwho_clone(xivo_wz_fm_db_backend_elt)};
xivo_wz_fm_db_backend['sqlite']['fd-db-file-xivo']['style'] = {display: 'block'};
xivo_wz_fm_db_backend['sqlite']['it-db-file-xivo']['property'] = {disabled: false};
xivo_wz_fm_db_backend['sqlite']['fd-db-file-ipbx']['style'] = {display: 'block'};
xivo_wz_fm_db_backend['sqlite']['it-db-file-ipbx']['property'] = {disabled: false};

xivo_attrib_register('fm_db_backend-sqlite',xivo_wz_fm_db_backend['sqlite']);


xivo_wz_fm_db_backend['mysql'] = dwho_clone(xivo_wz_fm_db_backend_elt);
xivo_wz_fm_db_backend['mysql']['fd-db-host']['style'] = {display: 'block'};
xivo_wz_fm_db_backend['mysql']['it-db-host']['property'] = {disabled: false};
xivo_wz_fm_db_backend['mysql']['fd-db-host-port']['style'] = {display: 'block'};
xivo_wz_fm_db_backend['mysql']['it-db-host-port']['property'] = {disabled: false};
xivo_wz_fm_db_backend['mysql']['fd-db-dbname-xivo']['style'] = {display: 'block'};
xivo_wz_fm_db_backend['mysql']['it-db-dbname-xivo']['property'] = {disabled: false};
xivo_wz_fm_db_backend['mysql']['fd-db-user-xivo']['style'] = {display: 'block'};
xivo_wz_fm_db_backend['mysql']['it-db-user-xivo']['property'] = {disabled: false};
xivo_wz_fm_db_backend['mysql']['fd-db-pwd-xivo']['style'] = {display: 'block'};
xivo_wz_fm_db_backend['mysql']['it-db-pwd-xivo']['property'] = {disabled: false};
xivo_wz_fm_db_backend['mysql']['fd-db-dbname-ipbx']['style'] = {display: 'block'};
xivo_wz_fm_db_backend['mysql']['it-db-dbname-ipbx']['property'] = {disabled: false};
xivo_wz_fm_db_backend['mysql']['fd-db-user-ipbx']['style'] = {display: 'block'};
xivo_wz_fm_db_backend['mysql']['it-db-user-ipbx']['property'] = {disabled: false};
xivo_wz_fm_db_backend['mysql']['fd-db-pwd-ipbx']['style'] = {display: 'block'};
xivo_wz_fm_db_backend['mysql']['it-db-pwd-ipbx']['property'] = {disabled: false};

xivo_attrib_register('fm_db_backend-mysql',xivo_wz_fm_db_backend['mysql']);

function xivo_wizard_chg_db_backend()
{
	if((backend = dwho_eid('it-db-backend')) !== false)
		xivo_chg_attrib('fm_db_backend-' + backend.value,'links',0,1);
}

function xivo_wizard_db_backend_onload()
{
	xivo_wizard_chg_db_backend();

	dwho.dom.add_event('change',
			   dwho_eid('it-db-backend'),
			   xivo_wizard_chg_db_backend);
}

dwho.dom.set_onload(xivo_wizard_db_backend_onload);
