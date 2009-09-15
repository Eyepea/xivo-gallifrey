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

function xivo_str_repeat(str,len)
{
	var r = '';
	str = xivo_string(str);
	len = Number(len);

	if(len < 1)
		return(r);

	for(var i = 0;i < len;i++)
		r += str;

	return(r);
}

function xivo_attrib_register(id,arr)
{
	if(xivo_is_undef(xivo_conf['attrib'][id]) === true)
		xivo_conf['attrib'][id] = arr;
}

function xivo_chg_style_attrib(elem,obj,type)
{
	type = xivo_is_undef(type) === true ? 0 : type;

	if(xivo_is_array(obj) === true && xivo_is_object(obj[type]) === true)
		obj = obj[type];
	else if(xivo_is_object(obj) === false)
		return(false);
	else if(xivo_is_undef(elem['type']) === false && elem['type'] === 'hidden')
		return(null);

	var style = '';
	var styles = '';

	try
	{
		if(xivo_is_undef(elem['style']) === false)
			var styletype = 1;
		else if(xivo_is_undef(elem.style.setAttribute) === false)
			var styletype = 2;
		else
			var styletype = 3;

		for(var property in obj)
		{
			style = obj[property];

			if(xivo_is_scalar(style) === false)
				continue;

			style = xivo_string(style).replace(/\s/g,'');

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

function xivo_trunc(str,nb,end,chr)
{
	var r = xivo_string(str);
	nb = Number(nb);
	end = xivo_string(end);
	chr = chr === null || xivo_is_undef(chr) === true ? ' ' : xivo_string(chr);

	if(nb < 1 || nb > r.length || (sub = xivo_substr(r,0,nb)) === '')
		return(r);

	r = sub;

	if(chr !== '' && (spos = sub.lastIndexOf(chr)) > -1)
		r = xivo_substr(r,0,spos);

	if(end.length > 0)
		r += end;

	return(r);
}

function xivo_htmlen(str,quote_style)
{
	var span = document.createElement('span');
	span.appendChild(document.createTextNode(str));

	return(xivo_htmlsc(span.innerHTML,quote_style));
}

function xivo_htmlsc(str,quote_style)
{
	str = xivo_string(str);
	quote_style = xivo_string(quote_style);

	str = str.replace(/</g,'&lt;');
	str = str.replace(/>/g,'&gt;');

	switch(quote_style.toUpperCase())
	{
		default:
			str = str.replace(/'/g,'&#039;');
		case '2':
		case 'ENT_COMPAT':
			str = str.replace(/"/g,'&quot;');
		case '0':
		case 'ENT_NOQUOTES':
	}

	return(str);
}

function xivo_split(str,delimit)
{
	str = xivo_string(str);

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
	if(xivo_is_array(obj) === true && xivo_is_object(obj[type]) === true)
		obj = obj[type];
	else if(xivo_is_object(obj) === false)
		return(false);

	for(var property in obj)
	{
		if(property === 'style')
			xivo_chg_style_attrib(elem,obj,type);
		else if(property === 'AddclassName')
			xivo.dom.add_cssclass(elem,obj[property]);
		else if(property === 'RemoveclassName')
			xivo.dom.remove_cssclass(elem,obj[property]);
		else
			elem[property] = obj[property];
	}

	return(true);
}

function xivo_chg_attrib(name,id,type,link)
{
	link = Boolean(link);
	type = xivo_is_undef(type) === true ? 0 : type;

	if(xivo_is_undef(xivo_conf['attrib'][name]) === true
	|| xivo_is_undef(xivo_conf['attrib'][name][id]) === true)
		return(false);

	var ref_elem = xivo_conf['attrib'][name][id];

	if((get = xivo_eid(id)) !== false)
	{
		if(xivo_is_undef(ref_elem['property']) === false)
			xivo_chg_property_attrib(get,ref_elem['property'],type);

		if(xivo_is_undef(ref_elem['style']) === false)
			xivo_chg_style_attrib(get,ref_elem['style'],type);
	}

	if(xivo_is_undef(ref_elem['link']) === true)
		return(null);
	else if(xivo_type_object(ref_elem['link']) === false || link === false)
	{
		xivo_chg_attrib(name,ref_elem['link'],type,2);
		return(null);
	}

	for(var property in ref_elem['link'])
	{
		if(xivo_is_undef(ref_elem['link'][property][2]) === false)
			nlink = ref_elem['link'][property][2];
		else
			nlink = 2;

		xivo_chg_attrib(name,
				ref_elem['link'][property][0],
				ref_elem['link'][property][1],
				nlink);
	}
}

function xivo_bool(bool)
{
	if(bool === null)
		return(false);

	switch(typeof(bool))
	{
		case 'object':
			return(true);
		case 'undefined':
			return(false);
	}

	switch(bool.toString().toLowerCase())
	{
		case 'y':
		case '1':
		case 'on':
		case 'yes':
		case 'true':
			return(true);
		case 'n':
		case '0':
		case 'off':
		case 'no':
		case 'false':
			return(false);
	}

	return(Boolean(bool));
}

function xivo_string(str)
{
	var r = '';

	if(xivo_is_undef(str) === true)
		return(r);

	switch(str)
	{
		case null:
		case false:
			return(r);
		case true:
			r = 1;
			break;
		default:
			r = str;
	}

	return(String(str));
}

function xivo_is_boolean(b)
{
	return((typeof(b) === 'boolean'));
}

function xivo_is_string(s)
{
	return((typeof(s) === 'string'));
}

function xivo_is_array(a)
{
	return((a instanceof Array));
}

function xivo_is_function(f)
{
	return((typeof(f) === 'function'));
}

function xivo_type_object(o)
{
	return((o !== null && typeof(o) === 'object'));
}

function xivo_is_object(o)
{
	return((xivo_type_object(o) === true && xivo_is_array(o) === false));
}

function xivo_is_undef(v)
{
	return((typeof(v) === 'undefined'));
}

function xivo_is_number(n)
{
	return((xivo_is_undef(Number(n))));
}

function xivo_is_int(i)
{
	var y = parseInt(i);

	if(isNaN(y) === true)
		return(false);

	return((i == y && i.toString() === y.toString()));
}

function xivo_is_float(i)
{
	var y = parseFloat(i);

	if(isNaN(y) === true || i != y)
		return(false);

	return((i.toString().replace(/\.00*$/,'') === y.toString()));
}

function xivo_is_uint(i)
{
	return(((xivo_is_int(i) === true && i >= 0)));
}

function xivo_is_ufloat(i)
{
	return(((xivo_is_float(i) === true && i >= 0)));
}

function xivo_is_scalar(val)
{
	switch(typeof(val))
	{
		case 'object':
		case 'undefined':
			return(false);
	}

	return(true);
}

function xivo_is_empty(val)
{
	if(xivo_is_undef(val) === true)
		return(true);
	else if(xivo_is_array(val) === true)
		return((val.length === 0));
	else if(xivo_is_object(val) === true)
	{
		for(var property in val)
			return(false);
		return(true);
	}
	
	switch(val)
	{
		case 0:
		case '0':
		case null:
		case false:
		case '':
			return(true);
	}

	return(false);
}

function xivo_has_len(val,key)
{
	if(xivo_is_undef(key) === false
	&& xivo_type_object(val) === true
	&& xivo_is_undef(val[key]) === false)
		val = val[key];

	if(xivo_is_scalar(val) === false)
		return(false);
	
	val = xivo_string(val);

	return((val.length > 0));
}

function xivo_chk_ipv4_strict(value)
{
	if(xivo_is_string(value) === false
	|| (value === xivo_long2ip(xivo_ip2long(value))) === false)
		return(false);

	return(value);
}

function xivo_long2ip(value)
{
	if(xivo_is_int(value) === false || value > 0xFFFFFFFF)
		value = 0xFFFFFFFF;

	var r	= ((value >> 24) & 0xFF) + '.'
		+ ((value >> 16) & 0xFF) + '.'
		+ ((value >> 8) & 0xFF) + '.'
		+ ((value & 0xFF));

	return(r);
}

function xivo_ip2long(value)
{
	if(xivo_is_scalar(value) === false)
		return(false);

	var split = xivo_string(value).split('.');

	if((len = split.length) > 4)
		return(false);

	var rs = 0;

	for(var i = 0;i < len;i++)
	{
		if(split[i].match(/^(?:[1-9][0-9]*|0[0-7]*|0x[0-9A-F]*)$/i) === null)
			return(false);

		var lsn = parseInt(split[i]);

		if(len === 1)
		{
			if (lsn <= 0xFFFFFFFF)
				return(lsn | 0);
			else
				return(false);
		}
		else if(i < len-1)
			mul = 1 << 8;
		else
			mul = 1 << (8 * (4-i));

		if (lsn >= mul)
			return(false);

		rs = rs * mul + lsn;
	}

	return(rs | 0);
}

function xivo_chk_host(value)
{
	if(xivo_is_string(value) === false
	|| value.length < 4
	|| value.length > 255)
		return(false);

	if(value.match(/^[a-z0-9-]+(?:\.[a-z0-9-]+)*\.[a-z]{2,4}$/i) === null)
		return(false);

	return(value);
}

function xivo_chk_ipv4_subnet(value)
{
	if(xivo_is_string(value) === false
	|| (pos = value.indexOf('/')) < 7)
		return(false);

	var mask = xivo_substr(value,pos+1);
	var ip = xivo_substr(value,0,pos);

	if(xivo_chk_ipv4_netmask(mask) === false
	|| xivo_chk_ipv4_strict(ip) === false)
		return(false);

	return(true);
}

function xivo_chk_ipv4_netmask(nm)
{
	if(xivo_chk_ipv4_netmask_bit(nm) === false
	&& xivo_chk_ipv4_netmask_dotdec(nm) === false)
		return(false);

	return(true);
}

function xivo_chk_ipv4_netmask_bit(bit)
{
	if(xivo_is_uint(bit) === false
	|| bit > 32)
		return(false);

	return(true);
}

function xivo_chk_ipv4_netmask_dotdec(nm)
{
	if((nm = xivo_ip2long(nm)) === 0)
		return(0);
	else if(nm === -1)
		return(32);
	else if(nm === false
	|| (inv = (nm ^ 0xFFFFFFFF)) === 0
	|| (inv & (inv + 1)) !== 0)
		return(false);

	return((32 - parseInt(Math.log(inv + 1,2))));
}

function xivo_strcmp(str1,str2,len)
{
	if(xivo_is_scalar(str1) === false
	|| xivo_is_scalar(str2) === false)
		return(false);

	str1 = xivo_string(str1);
	str2 = xivo_string(str2);

	if(xivo_is_uint(len) === true)
	{
		str1 = str1.substring(0,len);
		str2 = str2.substring(0,len);
	}

 	if(str1 > str2)
		return(1);
	else if(str1 === str2)
		return(0);

	return(-1);
}

function xivo_strcasecmp(str1,str2,len)
{
	if(xivo_is_scalar(str1) === false
	|| xivo_is_scalar(str2) === false)
		return(false);

	str1 = xivo_string(str1).toLowerCase();
	str2 = xivo_string(str2).toLowerCase();

	if(xivo_is_uint(len) === true)
	{
		str1 = str1.substring(0,len);
		str2 = str2.substring(0,len);
	}

 	if(str1 > str2)
		return(1);
	else if(str1 === str2)
		return(0);

	return(-1);
}

function xivo_substr(str,beg,end)
{
	var r = '';

	if(xivo_is_scalar(str) === false)
		return(r);

	var len = str.length;

	if(len === 0 || isNaN(beg) === true)
		return(r);

	if(isNaN(end) === true)
		end = len;

	beg = Number(beg);
	end = Number(end);

	if(beg < 0 || end < 0)
	{
		if(beg < 0)
			beg = len + beg;

		if(end < 0)
		{
			end = len + end;

			if(beg !== 0 && str.substr(beg,len).length > end)
				return(r);
		}

		return(str.substring(beg,end));
	}

	return(str.substr(beg,end));
}

function xivo_object_flip(obj)
{
	var r = {};

	for(var property in obj)
	{
		if(xivo_is_scalar(obj[property]) === true)
			r[obj[property]] = property;
	}

	return(r);
}

function xivo_clone(obj)
{
	if(xivo_is_array(obj) === true)
		var r = [];
	else if(xivo_is_object(obj) === true)
		var r = {};
	else
		return(obj);

	for(var property in obj)
		r[property] = xivo_clone(obj[property]);

	return(r);
}

function xivo_debug(obj)
{
	if(typeof(obj) !== 'object')
		return(false);

	var r = '';

  	for(var property in obj)
	{
		if(property === 'selectionStart' || property === 'selectionEnd')
			continue;

    		if(typeof(obj[property]) !== 'object')
		{
			if(typeof(obj[property]) !== 'function')
				r += property+':'+obj[property]+'\n';
			else
				r += 'function::'+property+'\n';
		}
		else
			r += xivo_debug(obj[property]);
	}

	return(r);
}

function xivo_leadzero(n)
{
	if (n < 10)
		n = '0' + n;

	return(n);
}

function xivo_menu_active()
{
	var xivo_menu_active = 'mn-'+xivo_api_path_info.replace(/\//g,'--');
	xivo_menu_active = xivo_menu_active.replace(/_/g,'-');

	if((xivo_menu_active = xivo_eid(xivo_menu_active)) !== false)
		xivo.dom.add_cssclass(xivo_menu_active,'mn-active');
}

function xivo_form_result(success,str)
{
	str = xivo_string(str);

	if(Boolean(success) === true)
		txtclassname = 'fm-txt-success';
	else
		txtclassname = 'fm-txt-error';

	if(str.length === 0 || xivo_eid('tooltips') === false)
		var property = {innerHTML: '&nbsp;'};
	else
	{
		str = str.replace(/;/g,'\\;');
		str = str.replace(/\|/g,'\\|');
		str = str.replace(/:/g,'\\:');

		var property = {'className': txtclassname, 'innerHTML': str};
	}

	xivo_chg_property_attrib(xivo_eid('tooltips'),property);
}
