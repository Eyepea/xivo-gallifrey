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

function dwho_trunc(str,nb,end,chr)
{
	var r = dwho_string(str);
	nb = Number(nb);
	end = dwho_string(end);
	chr = chr === null || dwho_is_undef(chr) === true ? ' ' : dwho_string(chr);

	if(nb < 1 || nb > r.length || (sub = dwho_substr(r,0,nb)) === '')
		return(r);

	r = sub;

	if(chr !== '' && (spos = sub.lastIndexOf(chr)) > -1)
		r = dwho_substr(r,0,spos);

	if(end.length > 0)
		r += end;

	return(r);
}

function dwho_str_repeat(str,len)
{
	var r = '';
	str = dwho_string(str);
	len = Number(len);

	if(len < 1)
		return(r);

	for(var i = 0;i < len;i++)
		r += str;

	return(r);
}

function dwho_htmlen(str,quote_style)
{
	var span = document.createElement('span');
	span.appendChild(document.createTextNode(str));

	return(dwho_htmlsc(span.innerHTML,quote_style));
}

function dwho_htmlsc(str,quote_style)
{
	str = dwho_string(str);
	quote_style = dwho_string(quote_style);

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

function dwho_bool(bool)
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

function dwho_string(str)
{
	var r = '';

	if(dwho_is_undef(str) === true)
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

function dwho_is_boolean(b)
{
	return((typeof(b) === 'boolean'));
}

function dwho_is_string(s)
{
	return((typeof(s) === 'string'));
}

function dwho_is_array(a)
{
	return((a instanceof Array));
}

function dwho_is_function(f)
{
	return((typeof(f) === 'function'));
}

function dwho_type_object(o)
{
	return((o !== null && typeof(o) === 'object'));
}

function dwho_is_object(o)
{
	return((dwho_type_object(o) === true && dwho_is_array(o) === false));
}

function dwho_is_undef(v)
{
	return((typeof(v) === 'undefined'));
}

function dwho_is_number(n)
{
	return((dwho_is_undef(Number(n))));
}

function dwho_is_int(i)
{
	var y = parseInt(i);

	if(isNaN(y) === true)
		return(false);

	return((i == y && i.toString() === y.toString()));
}

function dwho_is_float(i)
{
	var y = parseFloat(i);

	if(isNaN(y) === true || i != y)
		return(false);

	return((i.toString().replace(/\.00*$/,'') === y.toString()));
}

function dwho_is_uint(i)
{
	return(((dwho_is_int(i) === true && i >= 0)));
}

function dwho_is_ufloat(i)
{
	return(((dwho_is_float(i) === true && i >= 0)));
}

function dwho_is_scalar(val)
{
	switch(typeof(val))
	{
		case 'object':
		case 'undefined':
			return(false);
	}

	return(true);
}

function dwho_is_empty(val)
{
	if(dwho_is_undef(val) === true)
		return(true);
	else if(dwho_is_array(val) === true)
		return((val.length === 0));
	else if(dwho_is_object(val) === true)
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

function dwho_has_len(val,key)
{
	if(dwho_is_undef(key) === false
	&& dwho_type_object(val) === true
	&& dwho_is_undef(val[key]) === false)
		val = val[key];

	if(dwho_is_scalar(val) === false)
		return(false);

	val = dwho_string(val);

	return((val.length > 0));
}


function dwho_chk_ipv4_strict(value)
{
	if(dwho_is_string(value) === false
	|| value !== dwho_long2ip(dwho_ip2long(value)))
		return(false);

	return(value);
}

function dwho_long2ip(value)
{
	if(dwho_is_int(value) === false || value > 0xFFFFFFFF)
		value = 0xFFFFFFFF;

	var r	= ((value >> 24) & 0xFF) + '.'
		+ ((value >> 16) & 0xFF) + '.'
		+ ((value >> 8) & 0xFF) + '.'
		+ ((value & 0xFF));

	return(r);
}

function dwho_ip2long(value)
{
	if(dwho_is_scalar(value) === false)
		return(false);

	var split = dwho_string(value).split('.');

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

function dwho_chk_host(value)
{
	if(dwho_is_string(value) === false
	|| value.length < 4
	|| value.length > 255)
		return(false);

	if(value.match(/^[a-z0-9-]+(?:\.[a-z0-9-]+)*\.[a-z]{2,4}$/i) === null)
		return(false);

	return(value);
}

function dwho_chk_ipv4_subnet(value)
{
	if(dwho_is_string(value) === false
	|| (pos = value.indexOf('/')) < 7)
		return(false);

	var mask = dwho_substr(value,pos+1);
	var ip = dwho_substr(value,0,pos);

	if(dwho_chk_ipv4_netmask(mask) === false
	|| dwho_chk_ipv4_strict(ip) === false)
		return(false);

	return(true);
}

function dwho_chk_ipv4_netmask(nm)
{
	if(dwho_chk_ipv4_netmask_bit(nm) === false
	&& dwho_chk_ipv4_netmask_dotdec(nm) === false)
		return(false);

	return(true);
}

function dwho_chk_ipv4_netmask_bit(bit)
{
	if(dwho_is_uint(bit) === false
	|| bit > 32)
		return(false);

	return(true);
}

function dwho_chk_ipv4_netmask_dotdec(nm)
{
	if((nm = dwho_ip2long(nm)) === 0)
		return(0);
	else if(nm === -1)
		return(32);
	else if(nm === false
	|| (inv = (nm ^ 0xFFFFFFFF)) === 0
	|| (inv & (inv + 1)) !== 0)
		return(false);

	return((32 - parseInt(Math.log(inv + 1,2))));
}

function dwho_strcmp(str1,str2,len)
{
	if(dwho_is_scalar(str1) === false
	|| dwho_is_scalar(str2) === false)
		return(false);

	str1 = dwho_string(str1);
	str2 = dwho_string(str2);

	if(dwho_is_uint(len) === true)
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

function dwho_strcasecmp(str1,str2,len)
{
	if(dwho_is_scalar(str1) === false
	|| dwho_is_scalar(str2) === false)
		return(false);

	str1 = dwho_string(str1).toLowerCase();
	str2 = dwho_string(str2).toLowerCase();

	if(dwho_is_uint(len) === true)
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

function dwho_substr(str,beg,end)
{
	var r = '';

	if(dwho_is_scalar(str) === false)
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

function dwho_object_flip(obj)
{
	var r = {};

	for(var property in obj)
	{
		if(dwho_is_scalar(obj[property]) === true)
			r[obj[property]] = property;
	}

	return(r);
}

function dwho_clone(obj)
{
	if(dwho_is_array(obj) === true)
		var r = [];
	else if(dwho_is_object(obj) === true)
		var r = {};
	else
		return(obj);

	for(var property in obj)
		r[property] = dwho_clone(obj[property]);

	return(r);
}

function dwho_debug(obj)
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
			r += dwho_debug(obj[property]);
	}

	return(r);
}

function dwho_leadzero(n)
{
	if (n < 10)
		n = '0' + n;

	return(n);
}
