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

var xivo_conf = {'attrib': {},'eid': {}};

function xivo_bwcheck()
{
	this.ver	= navigator.appVersion;
	this.agent	= navigator.userAgent;
	this.opera	= (navigator.userAgent.indexOf('Opera') > -1);
	this.ie5	= (this.ver.indexOf('MSIE 5') > -1 && this.opera === false);
	this.ie6	= (this.ver.indexOf('MSIE 6') > -1 && this.opera === false);
	this.ie7	= (this.ver.indexOf('MSIE 7') > -1 && this.opera === false);
	this.ie8	= (this.ver.indexOf('MSIE 8') > -1 && this.opera === false);
	this.ie		= (this.ie5 || this.ie6 || this.ie7 || this.ie8);
	this.mac	= (this.agent.indexOf('Mac') > -1);
	this.cookie	= typeof(navigator.cookieEnabled) !== 'undefined' ? navigator.cookieEnabled : false;

	if(typeof(window.XMLHttpRequest) !== 'undefined')
		this.httpreq = function () { return(window.XMLHttpRequest); };
	else
		this.httpreq = function () { return(new ActiveXObject('Microsoft.XMLHTTP')); };
}

var xivo_bw = new xivo_bwcheck();

function xivo_open_center(url,name,width,height,param)
{
	if(xivo_bw.ie)
	{
		var x = 0;
		var y = 0;
		var w = screen.width;
		var h = screen.height;
	}
	else
	{
		var x = window.screenX;
		var y = window.screenY;
		var w = window.outerWidth;
		var h = window.outerHeight;
	}

	var cx = x;
	if(w > width)
		cx += Math.round((w - width) / 2);

	var cy = y;
	if(h > height)
		cy += Math.round((h - height) / 2);

	return open(url,name,'left=' + cx + 'px,top=' + cy + 'px,width=' + width + 'px,height=' + height + 'px' + param);
}

function xivo_attrib_register(id,arr)
{
	if(dwho_is_undef(xivo_conf['attrib'][id]) === true)
		xivo_conf['attrib'][id] = arr;
}

function xivo_chg_style_attrib(elem,obj,type)
{
	type = dwho_is_undef(type) === true ? 0 : type;

	if(dwho_is_array(obj) === true && dwho_is_object(obj[type]) === true)
		obj = obj[type];
	else if(dwho_is_object(obj) === false)
		return(false);
	else if(dwho_is_undef(elem['type']) === false && elem['type'] === 'hidden')
		return(null);

	var style = '';
	var styles = '';

	try
	{
		if(dwho_is_undef(elem['style']) === false)
			var styletype = 1;
		else if(dwho_is_undef(elem.style.setAttribute) === false)
			var styletype = 2;
		else
			var styletype = 3;

		for(var property in obj)
		{
			style = obj[property];

			if(dwho_is_scalar(style) === false)
				continue;

			style = dwho_string(style).replace(/\s/g,'');

			if(style.length === 0)
				continue;
			else if(styletype !== 3 && (pos = property.search(/-/)) != -1)
			{
				tmp = property;
				property  = tmp.substring(0,pos);
				property += tmp.substr(pos+1,1).toUpperCase();
				property += tmp.substr(pos+2,(tmp.length - pos-2));
			}

			if(styletype === 1)
				elem['style'][property] = style;
			else if(styletype === 2)
				elem.style.setAttribute(property,style);
			else
				styles += property+':'+obj[style]+';';
		}

		if(styletype === 3 && styles.length > 2)
		{
			elemstyle = elem.style.cssText.replace(/\s/g,'');

			if(elemstyle.charAt(elemstyle.length-1) !== ';')
				elemstyle += ';';

			elem.style.cssText = elemstyle+styles;
		}
	}
	catch(e)
	{
		return(false);
	}

	return(true);
}

function xivo_split(str,delimit)
{
	str = dwho_string(str);

	if(str.match(/\\/) === null || delimit.length != 1)
		return(str.split(delimit));

	var len = str.length;

	var norm = 0;
	var lit = 1;
	var strip = 2;

	var st = norm;
	var c = out = '';
	var r = new Array();

	for(var i = 0;i < len;i++)
	{
		c = str.charAt(i);

		switch(st)
		{
			case norm:
				if(c === '\\')
					st = strip;
				else if(c === delimit)
				{
					r.push(out);
					out = '';
				}
				else
					out += c;
				break;
			case strip:
				if(c === delimit)
					out += c;
				else
					out += '\\'+c;
				st = norm;
				break;
		}
	}

	if(out.length != 0)
		r.push(out);

	return(r);
}

function xivo_chg_property_attrib(elem,obj,type)
{
	if(dwho_is_array(obj) === true && dwho_is_object(obj[type]) === true)
		obj = obj[type];
	else if(dwho_is_object(obj) === false)
		return(false);

	for(var property in obj)
	{
		if(property === 'style')
			xivo_chg_style_attrib(elem,obj,type);
		else if(property === 'AddclassName')
			dwho.dom.add_cssclass(elem,obj[property]);
		else if(property === 'RemoveclassName')
			dwho.dom.remove_cssclass(elem,obj[property]);
		else
			elem[property] = obj[property];
	}

	return(true);
}

function xivo_chg_attrib(name,id,type,link)
{
	link = Boolean(link);
	type = dwho_is_undef(type) === true ? 0 : type;

	if(dwho_is_undef(xivo_conf['attrib'][name]) === true
	|| dwho_is_undef(xivo_conf['attrib'][name][id]) === true)
		return(false);

	var ref_elem = xivo_conf['attrib'][name][id];

	if((get = dwho_eid(id)) !== false)
	{
		if(dwho_is_undef(ref_elem['property']) === false)
			xivo_chg_property_attrib(get,ref_elem['property'],type);

		if(dwho_is_undef(ref_elem['style']) === false)
			xivo_chg_style_attrib(get,ref_elem['style'],type);
	}

	if(dwho_is_undef(ref_elem['link']) === true)
		return(null);
	else if(dwho_type_object(ref_elem['link']) === false || link === false)
	{
		xivo_chg_attrib(name,ref_elem['link'],type,2);
		return(null);
	}

	for(var property in ref_elem['link'])
	{
		if(dwho_is_undef(ref_elem['link'][property][2]) === false)
			nlink = ref_elem['link'][property][2];
		else
			nlink = 2;

		xivo_chg_attrib(name,
				ref_elem['link'][property][0],
				ref_elem['link'][property][1],
				nlink);
	}
}

function xivo_menu_active()
{
	var xivo_menu_active = 'mn-' + dwho_location_app_path.replace(/\//g,'--');
	xivo_menu_active = xivo_menu_active.replace(/_/g,'-');

	if((xivo_menu_active = dwho_eid(xivo_menu_active)) !== false)
		dwho.dom.add_cssclass(xivo_menu_active,'mn-active');
}

function xivo_form_select_add_host_ipv4_subnet(id,value)
{
	if(dwho_chk_ipv4_strict(value) === false
	&& dwho_chk_host(value) === false
	&& dwho_chk_ipv4_subnet(value) === false)
		return(false);

	return(dwho.form.select_add_entry(id,value,value));
}

function xivo_form_mk_acl(tree)
{
	var ref = tree.form[tree.name];
	var value = tree.value;
	var len = value.length;
	var nb = ref.length
	var sub = 0;
	var rs = false;

	for(var i = 0;i < nb;i++)
	{
		sub = ref[i].value.substring(0,len);

		if(value === sub)
			ref[i].checked = Boolean(tree.checked);
	}
}

function xivo_form_result(success,str)
{
	str = dwho_string(str);

	if(Boolean(success) === true)
		txtclassname = 'fm-txt-success';
	else
		txtclassname = 'fm-txt-error';

	if(str.length === 0 || dwho_eid('tooltips') === false)
		var property = {innerHTML: '&nbsp;'};
	else
	{
		str = str.replace(/;/g,'\\;');
		str = str.replace(/\|/g,'\\|');
		str = str.replace(/:/g,'\\:');

		var property = {'className': txtclassname, 'innerHTML': str};
	}

	xivo_chg_property_attrib(dwho_eid('tooltips'),property);
}

function xivo_set_confirm_uri_for_main_listing()
{
	if((listing = dwho_eid('table-main-listing')) === false
	|| dwho_is_function(listing.getElementsByTagName) === false)
		return(false);

	var list = listing.getElementsByTagName('a');
	var nb = list.length;

	for(var i = 0;i < nb;i++)
		dwho.dom.set_confirm_newlocation(list[i]);

	return(true);
}

dwho.dom.set_onload(xivo_set_confirm_uri_for_main_listing);
dwho.dom.set_onload(xivo_menu_active);
