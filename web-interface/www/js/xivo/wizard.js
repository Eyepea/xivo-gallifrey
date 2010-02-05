/*
 * XiVO Web-Interface
 * Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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

var xivo_wz_fm_dbconfig_backend_elt = {
	'error-dbconfig-sqlite-xivo':	{'style':	{display: 'none'}},
	'fd-dbconfig-sqlite-xivodb':	{'style':	{display: 'none'}},
	'it-dbconfig-sqlite-xivodb':	{'property':	{disabled: true}}, 
	'error-dbconfig-sqlite-ipbx':	{'style':	{display: 'none'}},
	'fd-dbconfig-sqlite-ipbxdb':	{'style':	{display: 'none'}},
	'it-dbconfig-sqlite-ipbxdb':	{'property':	{disabled: true}}, 
	'fd-dbconfig-mysql-host':	{'style':	{display: 'none'}},
	'it-dbconfig-mysql-host':	{'property':	{disabled: true}}, 
	'fd-dbconfig-mysql-port':	{'style':	{display: 'none'}},
	'it-dbconfig-mysql-port':	{'property':	{disabled: true}},
	'error-dbconfig-mysql-xivo':	{'style':	{display: 'none'}},
	'fd-dbconfig-mysql-xivodbname': {'style':	{display: 'none'}},
	'it-dbconfig-mysql-xivodbname': {'property':	{disabled: true}},
	'fd-dbconfig-mysql-xivouser':	{'style':	{display: 'none'}},
	'it-dbconfig-mysql-xivouser':	{'property':	{disabled: true}}, 
	'fd-dbconfig-mysql-xivopass':	{'style':	{display: 'none'}},
	'it-dbconfig-mysql-xivopass':	{'property':	{disabled: true}}, 
	'error-dbconfig-mysql-ipbx':	{'style':	{display: 'none'}},
	'fd-dbconfig-mysql-ipbxdbname': {'style':	{display: 'none'}},
	'it-dbconfig-mysql-ipbxdbname': {'property':	{disabled: true}},
	'fd-dbconfig-mysql-ipbxuser':	{'style':	{display: 'none'}},
	'it-dbconfig-mysql-ipbxuser':	{'property':	{disabled: true}}, 
	'fd-dbconfig-mysql-ipbxpass':	{'style':	{display: 'none'}},
	'it-dbconfig-mysql-ipbxpass':	{'property':	{disabled: true}}, 
	'links':
		{link:	[['error-dbconfig-sqlite-xivo',0,1],
			 ['fd-dbconfig-sqlite-xivodb',0,1],
			 ['it-dbconfig-sqlite-xivodb',0,1],
			 ['error-dbconfig-sqlite-ipbx',0,1],
			 ['fd-dbconfig-sqlite-ipbxdb',0,1],
			 ['it-dbconfig-sqlite-ipbxdb',0,1],
			 ['fd-dbconfig-mysql-host',0,1],
			 ['it-dbconfig-mysql-host',0,1],
			 ['fd-dbconfig-mysql-port',0,1],
			 ['it-dbconfig-mysql-port',0,1],
			 ['error-dbconfig-mysql-xivo',0,1],
			 ['fd-dbconfig-mysql-xivodbname',0,1],
			 ['it-dbconfig-mysql-xivodbname',0,1],
			 ['fd-dbconfig-mysql-xivouser',0,1],
			 ['it-dbconfig-mysql-xivouser',0,1],
			 ['fd-dbconfig-mysql-xivopass',0,1],
			 ['it-dbconfig-mysql-xivopass',0,1],
			 ['error-dbconfig-mysql-ipbx',0,1],
			 ['fd-dbconfig-mysql-ipbxdbname',0,1],
			 ['it-dbconfig-mysql-ipbxdbname',0,1],
			 ['fd-dbconfig-mysql-ipbxuser',0,1],
			 ['it-dbconfig-mysql-ipbxuser',0,1],
			 ['fd-dbconfig-mysql-ipbxpass',0,1],
			 ['it-dbconfig-mysql-ipbxpass',0,1]]
		}
};

var xivo_wz_fm_dbconfig_backend = {'sqlite':	dwho_clone(xivo_wz_fm_dbconfig_backend_elt)};
xivo_wz_fm_dbconfig_backend['sqlite']['error-dbconfig-sqlite-xivo']['style'] = {display: 'block'};
xivo_wz_fm_dbconfig_backend['sqlite']['fd-dbconfig-sqlite-xivodb']['style'] = {display: 'block'};
xivo_wz_fm_dbconfig_backend['sqlite']['it-dbconfig-sqlite-xivodb']['property'] = {disabled: false};
xivo_wz_fm_dbconfig_backend['sqlite']['error-dbconfig-sqlite-ipbx']['style'] = {display: 'block'};
xivo_wz_fm_dbconfig_backend['sqlite']['fd-dbconfig-sqlite-ipbxdb']['style'] = {display: 'block'};
xivo_wz_fm_dbconfig_backend['sqlite']['it-dbconfig-sqlite-ipbxdb']['property'] = {disabled: false};

xivo_attrib_register('fm_dbconfig_backend-sqlite',xivo_wz_fm_dbconfig_backend['sqlite']);


xivo_wz_fm_dbconfig_backend['mysql'] = dwho_clone(xivo_wz_fm_dbconfig_backend_elt);
xivo_wz_fm_dbconfig_backend['mysql']['fd-dbconfig-mysql-host']['style'] = {display: 'block'};
xivo_wz_fm_dbconfig_backend['mysql']['it-dbconfig-mysql-host']['property'] = {disabled: false};
xivo_wz_fm_dbconfig_backend['mysql']['fd-dbconfig-mysql-port']['style'] = {display: 'block'};
xivo_wz_fm_dbconfig_backend['mysql']['it-dbconfig-mysql-port']['property'] = {disabled: false};
xivo_wz_fm_dbconfig_backend['mysql']['error-dbconfig-mysql-xivo']['style'] = {display: 'block'};
xivo_wz_fm_dbconfig_backend['mysql']['fd-dbconfig-mysql-xivodbname']['style'] = {display: 'block'};
xivo_wz_fm_dbconfig_backend['mysql']['it-dbconfig-mysql-xivodbname']['property'] = {disabled: false};
xivo_wz_fm_dbconfig_backend['mysql']['fd-dbconfig-mysql-xivouser']['style'] = {display: 'block'};
xivo_wz_fm_dbconfig_backend['mysql']['it-dbconfig-mysql-xivouser']['property'] = {disabled: false};
xivo_wz_fm_dbconfig_backend['mysql']['fd-dbconfig-mysql-xivopass']['style'] = {display: 'block'};
xivo_wz_fm_dbconfig_backend['mysql']['it-dbconfig-mysql-xivopass']['property'] = {disabled: false};
xivo_wz_fm_dbconfig_backend['mysql']['error-dbconfig-mysql-ipbx']['style'] = {display: 'block'};
xivo_wz_fm_dbconfig_backend['mysql']['fd-dbconfig-mysql-ipbxdbname']['style'] = {display: 'block'};
xivo_wz_fm_dbconfig_backend['mysql']['it-dbconfig-mysql-ipbxdbname']['property'] = {disabled: false};
xivo_wz_fm_dbconfig_backend['mysql']['fd-dbconfig-mysql-ipbxuser']['style'] = {display: 'block'};
xivo_wz_fm_dbconfig_backend['mysql']['it-dbconfig-mysql-ipbxuser']['property'] = {disabled: false};
xivo_wz_fm_dbconfig_backend['mysql']['fd-dbconfig-mysql-ipbxpass']['style'] = {display: 'block'};
xivo_wz_fm_dbconfig_backend['mysql']['it-dbconfig-mysql-ipbxpass']['property'] = {disabled: false};

xivo_attrib_register('fm_dbconfig_backend-mysql',xivo_wz_fm_dbconfig_backend['mysql']);

function xivo_wizard_chg_dbconfig_backend()
{
	if((backend = dwho_eid('it-dbconfig-backend')) !== false)
		xivo_chg_attrib('fm_dbconfig_backend-' + backend.value,'links',0,1);
}

function xivo_wizard_ipbximportuser_error(sum)
{
	var spansum = dwho.dom.create_element('span',
					      {'id':		'ipbximportuser-tooltips-error',
					       'className':	'fm-txt-error'},
					      sum);

	dwho.dom.create_focus_caption(
			dwho_eid('tooltips'),
			dwho_eid('ipbximportuser-lines-status'),
			spansum,
			'center');
}

function xivo_wizard_dbconfig_backend_onload()
{
	xivo_wizard_chg_dbconfig_backend();

	dwho.dom.add_event('change',
			   dwho_eid('it-dbconfig-backend'),
			   xivo_wizard_chg_dbconfig_backend);

	dwho.dom.add_event('change',
			   dwho_eid('it-language'),
			   function()
			   {
				this.form['refresh'].value = 1;
				this.form.submit();
			   });

	dwho.dom.add_event('click',
			   dwho_eid('it-previous'),
			   function()
			   {
				this.type = 'submit';
			   });

	dwho.dom.add_event('click',
			   dwho_eid('it-verify'),
			   function()
			   {
				this.form['verify'].value = 1;
			   	this.type = 'submit';
			   });
}

dwho.dom.set_onload(xivo_wizard_dbconfig_backend_onload);
