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

var xivo_smenu = {
	'click': {'id': '','class': ''},
	'bak': {},
	'before': {'id': '','class': ''},
	'class': 'moc',
	'display': {},
	'part': 'sb-part-first',
	'tab': 'smenu-tab-1',
	'last': false};

function xivo_smenu_click(obj,cname,part,last)
{
	if(dwho_is_empty(obj.id) === true)
		return(false);

	var disobj = click = before = '';
	var num = 0;
	var id = obj.id;

	if(xivo_smenu['display'] !== '' && (disobj = dwho_eid(xivo_smenu['display'])) !== false)
		disobj.style.display = 'none';

	if((disobj = dwho_eid(part)) !== false)
	{
		xivo_smenu['display'] = part;
		disobj.style.display = 'block';
	}

	if(xivo_smenu['click']['id'] !== '' && (click = dwho_eid(xivo_smenu['click']['id'])) !== false)
		click.className = xivo_smenu['click']['class'];

	if(dwho_is_undef(xivo_smenu['bak'][id]) === false)
	{
		xivo_smenu['click']['id'] = id;
		xivo_smenu['click']['class'] = xivo_smenu['bak'][id];
	}

	if(xivo_smenu['before']['id'] !== '' && (before = dwho_eid(xivo_smenu['before']['id'])) !== false)
		before.className = xivo_smenu['before']['class'];

	var rs = id.match(/^([a-z0-9-_]*)(\d+)$/i);

	if(rs !== null)
	{
		var vid = rs[1];
		var num = Number(rs[2]);
		var nid = vid+(num-1);
		var get = '';

		if(num > 1 && (get = dwho_eid(nid)) !== false)
		{
			if(dwho_is_undef(xivo_smenu['bak'][nid]) === true)
				xivo_smenu['bak'][nid] = get.className;

			xivo_smenu['before']['id'] = nid;
			xivo_smenu['before']['class'] = get.className;
			get.className = cname+'-before';
		}
		else
			xivo_smenu['before']['id'] = xivo_smenu['before']['class'] = '';
	}

	obj.className = Boolean(last) === false ? cname : cname+'-last';

	dwho.dom.free_focus();
}

function xivo_smenu_fmsubmit(obj)
{
	if(dwho_is_object(obj) === false
	|| obj.nodeName.toLowerCase() !== 'form'
	|| dwho_is_undef(obj['fm_smenu-tab']) === true
	|| dwho_is_undef(obj['fm_smenu-part']) === true
	|| xivo_smenu['click']['id'] === ''
	|| xivo_smenu['display'] === '')
		return(false);

	obj['fm_smenu-tab'].value = xivo_smenu['click']['id'];
	obj['fm_smenu-part'].value = xivo_smenu['display'];

	return(true);
}

function xivo_smenu_out(obj,cname,last)
{
	if((ul = dwho.dom.etag('ul',obj,0)) !== false)
		ul.style.display = 'none';

	if(dwho_is_empty(obj.id) === true || xivo_smenu['click']['id'] === obj.id)
		return(false);

	var num = 0;
	var id = obj.id;

	if(Boolean(last) === true)
	{
		obj.className = cname+'-last';
		return(true);
	}

	var rs = id.match(/^([a-z0-9-_]*)(\d+)$/i);

	if(rs !== null)
	{
		var vid = rs[1];
		var num = Number(rs[2]);
		var nid = vid+(num+1);

		if(xivo_smenu['click']['id'] === nid)
		{
			obj.className = cname+'-before';
			return(true);
		}
	}

	obj.className = cname;

	return(true);
}

function xivo_smenu_over(obj,cname,last)
{
	if((ul = dwho.dom.etag('ul',obj,0)) !== false)
		ul.style.display = 'block';

	if(dwho_is_empty(obj.id) === true || xivo_smenu['click']['id'] === obj.id)
		return(false);

	var num = 0;
	var id = obj.id;

	if(dwho_is_undef(xivo_smenu['bak'][id]) === true)
		xivo_smenu['bak'][id] = obj.className;

	if(Boolean(last) === true)
	{
		obj.className = cname+'-last';
		return(true);
	}

	var rs = id.match(/^([a-z0-9-_]*)(\d+)$/i);

	if(rs !== null)
	{
		var vid = rs[1];
		var num = Number(rs[2]);
		var nid = vid+(num+1);

		if(xivo_smenu['click']['id'] === nid)
		{
			obj.className = cname+'-before';
			return(true);
		}
	}

	obj.className = cname;

	return(true);
}

function xivo_submenu_onload()
{
	if(dwho_eid(xivo_smenu['tab']) === false)
		return(false);

	xivo_smenu['bak'][xivo_smenu['tab']] = dwho_eid(xivo_smenu['tab']).className;
	xivo_smenu_click(dwho_eid(xivo_smenu['tab']),xivo_smenu['class'],xivo_smenu['part'],xivo_smenu['last']);
}

dwho.dom.set_onload(xivo_submenu_onload);
