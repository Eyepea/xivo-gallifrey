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

if(typeof(xivo) === 'undefined')
	xivo = {'uri': {}};
else if(xivo_is_undef(xivo.uri) === true)
	xivo.uri = {};

xivo.uri = function()
{
	this._encode	= {};
	this._host	= {'ipliteral':	1,
			   'ipv4':	2,
			   'reg_name':	3};
	this._regexp	= {};

	var gen_delims	= ':\\/\\?#\\[\\]@';
	var sub_delims	= '\\!\\$&\\\'\\(\\)\\*\\+,;=';
	var reserved	= gen_delims + sub_delims;
	var unreserved	= 'a-zA-Z0-9-\\._~';
	var ipvfuture	= 'v[\\da-fA-F]+\\.[' + unreserved + sub_delims + ':]+';
	var port	= '[0-9]{0,5}';

	this._encode.user	= new RegExp('([^' + unreserved + sub_delims + ']+)');
	this._encode.password	= new RegExp('([^' + unreserved + sub_delims + ':]+)');
	this._encode.reg_name	= new RegExp('([^' + unreserved + sub_delims + ']+)');
	this._encode.path	= new RegExp('([^' + unreserved + sub_delims + ':@\\/]+)');
	this._encode.query_key	= new RegExp('([^' + unreserved + '\\!\\$\\\'\\(\\)\\*,;:@\\/\\? ]+)');
	this._encode.query_val	= new RegExp('([^' + unreserved + '\\!\\$\\\'\\(\\)\\*,;:@\\/\\? =]+)');
	this._encode.fragment	= new RegExp('([^' + unreserved + sub_delims + ':@\\/\\?]+)');

	this._regexp.rfc3986	= new RegExp('^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?$');
	this._regexp.scheme	= new RegExp('^[a-zA-Z][a-zA-Z0-9\\+\\-\\.]*$');
	this._regexp.user	= new RegExp('^[' + unreserved + sub_delims + '%]+$');
	this._regexp.host	= this._regexp.user;
	this._regexp.password	= new RegExp('^[' + unreserved + sub_delims + '%:]+$');
	this._regexp.port	= new RegExp('^' + port + '$');
	this._regexp.path	= new RegExp('^[' + unreserved + sub_delims + '%:@\\/]+$');
	this._regexp.query	= new RegExp('^[' + unreserved + sub_delims + '%:@\\/\\?]+$');
	this._regexp.ipvfuture	= new RegExp('^' + ipvfuture + '$');

	this._reipvfutureport	= new RegExp('^\\[([\\da-fA-F:\\.]+|' + ipvfuture + ')\\]' +
					     '(\\:' + port + ')?$');
}

xivo.uri.prototype.build = function(obj,encode,type_host)
{
	encode = xivo_is_undef(encode) === true ? true : Boolean(encode);
	var scheme = authority = userinfo = hostport = path = query = fragment = '';

	if(xivo_is_object(obj) === false)
		return(false);
	else if(xivo_has_len(obj,'scheme') === true)
	{
		if(this.valid_scheme(obj.scheme) === false)
			return(false);

		scheme = obj.scheme;
	}

	if(xivo_is_object(obj.authority) === true)
	{
		if(xivo_is_undef(obj.authority.host) === false)
			obj.host	= obj.authority.host;

		if(xivo_is_undef(obj.authority.port) === false)
			obj.port	= obj.authority.port;

		if(xivo_is_undef(obj.authority.user) === false)
			obj.user	= obj.authority.user;

		if(xivo_is_undef(obj.authority.password) === false)
			obj.password	= obj.authority.password;
	}

	if(xivo_has_len(obj,'host') === true)
	{
		obj.host = xivo_string(obj.host);

		switch((xivo_is_empty(type_host) === true ? this.get_host_type(obj.host) : type_host))
		{
			case this._host.ipliteral:
				if(this.valid_ipliteral(obj.host) === false)
					return(false);
				break;
			case this._host.ipv4:
				if(this.valid_ipv4(obj.host) === false)
					return(false);
				break;
			case this._host.reg_name:
			default:
				if(encode === true)
					obj.host = this.encode(obj.host,'reg_name');
				break;
		}

		hostport = obj.host;
	}

	if(xivo_has_len(obj,'port') === true)
	{
		if(this.valid_port(obj.port) === false)
			return(false);

		hostport += ':' + obj.port;
	}

	if(xivo_has_len(obj,'user') === true)
	{
		obj.user = xivo_string(obj.user);
		if(encode === true)
			obj.user = this.encode(obj.user,'user');

		userinfo = obj.user;
	}

	if(xivo_has_len(obj,'password') === true)
	{
		obj.password = xivo_string(obj.password);
		if(encode === true)
			obj.password = this.encode(obj.password,'password');

		userinfo += ':' + obj.password;
	}

	if(userinfo !== '' && hostport !== '')
		authority = userinfo + '@' + hostport;
	else if(userinfo !== '')
		authority = userinfo + '@';
	else if(hostport !== '')
		authority = hostport;

	if(xivo_has_len(obj,'path') === true)
	{
		obj.path = xivo_string(obj.path);

		if(authority !== '' && obj.path[0] !== '/')
			return(false);
		else if(encode === false)
			path = obj.path;
		else
		{
			path = this.encode(obj.path,'path');

			if(authority === ''
			&& scheme === ''
			&& (pos = path.indexOf('/')) !== -1)
				path = path.substring(0,pos).replace(/:/g,'%3A') +
				       path.substring(pos);
		}
	}

	if(xivo_is_array(obj.query) === true)
		query = this.build_query(obj.query,encode);
	
	if(xivo_has_len(obj,'fragment') === true)
	{
		obj.fragment = xivo_string(obj.fragment);
		if(encode === true)
			fragment = this.encode(obj.fragment,'fragment');
		else
			fragment = obj.fragment;
	}

	var r = '';

	if(scheme !== '')
		r += scheme + ':';
	
	if(authority !== '')
		r += '//' + authority;
	
	if(path !== '')
	{
		if(authority === '' && path.substr(0,2) === '//')
			r += '//';

		r += path;
	}

	if(query !== '')
		r += '?' + query;
	
	if(fragment !== '')
		r += '#' + fragment;

	return(r);
}

xivo.uri.prototype.parse = function(uri,decode)
{
	decode = xivo_is_undef(decode) === true ? true : Boolean(decode);

	if((uri = this.split(uri)) === false)
		return(false);
	else if(xivo_is_undef(uri.authority) === false
	&& (uri.authority = this.split_authority(uri.authority)) === false)
		return(false);
	else if(xivo_is_undef(uri.query) === false
	&& (uri.query = this.split_query(uri.query)) === false)
		return(false);
	else if(xivo_is_undef(uri.scheme) === false
	&& this.valid_scheme(uri.scheme) === false)
		return(false);

	var authority = null;

	if(xivo_is_object(uri.authority) === true)
	{
		authority = uri.authority;

		if(xivo_is_undef(authority.user) === false)
		{
			if(this.valid_user(authority.user) === false)
				return(false);
			else if(decode === true)
				authority.user = decodeURIComponent(authority.user);
		}

		if(xivo_is_undef(authority.password) === false)
		{
			if(this.valid_password(authority.password) === false)
				return(false);
			else if(decode === true)
				authority.password = decodeURIComponent(authority.password);
		}

		if(xivo_is_undef(authority.host) === false)
		{
			if(this.valid_host(authority.host) === false)
				return(false);
			else if(decode === true)
				authority.host = decodeURIComponent(authority.host);
		}

		if(xivo_is_undef(authority.port) === false
		&& this.valid_port(authority.port) === false)
			return(false);
	}

	if(xivo_is_undef(uri.path) === false)
	{
		if(this.valid_path(uri.path) === false)
			return(false);
		else if(authority !== null && uri.path[0] !== '/')
			return(false);
		else if((authority === null
		   || xivo_is_undef(uri.scheme) === true) === true
		&& (pos = uri.path.indexOf(':')) !== -1
		&& (poslash = uri.path.indexOf('/')) !== -1
		&& pos < poslash)
			return(false);

		if(decode === true)
			uri.path = decodeURIComponent(uri.path);
	}

	if(xivo_is_array(uri.query) === true && (nb = uri.query.length) !== 0)
	{
		for(var i = 0;i < nb;i++)
		{
			ref = uri.query[i];

			if(xivo_has_len(ref,0) === true)
			{
				if(this.valid_query(ref[0]) === false)
					return(false);
				else if(decode === true)
					ref[0] = decodeURIComponent(ref[0].replace(/\+/g,' '));
			}

			if(xivo_has_len(ref,1) === false)
				continue;
			else if(this.valid_query(ref[1]) === false)
				return(false);
			else if(decode === true)
				ref[1] = decodeURIComponent(ref[1].replace(/\+/g,' '));
		}
	}

	if(xivo_is_undef(uri.fragment) === false)
	{
		if(this.valid_fragment(uri.fragment) === false)
			return(false);
		else if(decode === true)
			uri.fragment = decodeURIComponent(uri.fragment);
	}

	return(uri);
}

xivo.uri.prototype.split = function(uri)
{
	if(xivo_is_scalar(uri) === false
	|| (rs = xivo_string(uri).match(this._regexp.rfc3986)) === null)
		return(false);

	var r = {};

	if(xivo_has_len(rs,2) === true)
		r.scheme	= rs[2];

	if(xivo_has_len(rs,4) === true)
		r.authority	= rs[4];

	if(xivo_has_len(rs,5) === true)
		r.path		= rs[5];

	if(xivo_has_len(rs,7) === true)
		r.query		= rs[7];

	if(xivo_has_len(rs,9) === true)
		r.fragment	= rs[9];

	return(r);
}

xivo.uri.prototype.split_authority = function(authority)
{
	authority = xivo_string(authority);

	if(authority === '')
		return(null);
	
	var r = {};

	if((pos = authority.indexOf('@')) === -1)
		var hostport = authority;
	else if(pos === 0)
		var hostport = authority.substring(1);
	else
	{
		var userinfo = authority.substring(0,pos);
		var hostport = authority.substring(pos + 1);

		if((pos = userinfo.indexOf(':')) === -1)
			r.user = userinfo;
		else
		{
			if(pos !== 0)
				r.user = userinfo.substring(0,pos);

			if(xivo_is_undef(userinfo[pos + 1]) === false)
				r.password = userinfo.substring(pos + 1);
		}
	}

	if(hostport === '')
		return(r);
	else if(hostport[0] === '[')
	{
		if((rs = hostport.match(this._reipvfutureport)) === null)
			return(false);

		r.host = '[' + rs[1] + ']';

		if(xivo_is_undef(rs[2]) === false && xivo_is_undef(rs[2][1]) === false)
			r.port = rs[2].substring(1);
	}
	else if((pos = hostport.indexOf(':')) !== -1)
	{
		if(pos !== 0)
			r.host = hostport.substring(0,pos);

		if(xivo_is_undef(hostport[pos + 1]) === false)
			r.port = hostport.substring(pos + 1);
	}
	else
		r.host = hostport;

	return(r);
}

xivo.uri.prototype.split_query = function(query)
{
	query = xivo_string(query);

	if(query === '')
		return(false);

	var assignments = query.split(/&/);
	var nb = assignments.length;
	var r = [];

	for(var i = 0;i < nb;i++)
	{
		if(assignments[i] === '')
			continue;

		var satmp = assignments[i].split(/=/);

		var sa = satmp.splice(0,1);

		if(assignments[i] !== sa[0])
			sa.push(satmp.join('='));

		r.push(sa);
	}

	if(r.length === 0)
		return(false);
	
	return(r);
}

xivo.uri.prototype.build_query = function(query,encode)
{
	encode = xivo_is_undef(encode) === true ? true : Boolean(encode);

	if(xivo_is_array(query) === false
	|| query.length === 0)
		return('');

	var r = [];

	for(var key in query)
	{
		var value = query[key];
		var assignments = '';

		if(xivo_is_array(value) === false)
			continue;

		if(xivo_has_len(value,0) === true)
		{
			value[0] = xivo_string(value[0]);

			if(encode === true)
				assignments = this.encode(value[0],'query_key');
			else
				assignments = value[0];
		}

		if(xivo_has_len(value,1) === false)
		{
			if(assignments !== '')
				r.push(assignments);
			continue;
		}

		value[1] = xivo_string(value[1]);

		if(encode === true)
			assignments += '=' + this.encode(value[1],'query_val');
		else
			assignments += '=' + value[1];

		if(assignments !== '')
			r.push(assignments);
	}

	return(r.join('&'));
}

xivo.uri.prototype.get_host_type = function(host)
{
	if(xivo_has_len(host) === false)
		return(this._host.reg_name);

	host = xivo_string(host);

	if(host[0] === '[')
		return(this._host.ipliteral);
	else if(this.valid_ipv4(host) === true)
		return(this._host.ipv4);
	else
		return(this._host.reg_name);
}

xivo.uri.prototype.encode = function(str,type)
{
	str	= xivo_string(str);
	type	= xivo_string(type);

	var encode_uri = function(str)
	{
		return(encodeURIComponent(xivo_string(str)).
		       replace(/!/g,'%21').
		       replace(/'/g,'%27').
		       replace(/\(/g,'%28').
		       replace(/\)/g,'%29').
		       replace(/\*/g,'%2A').
		       replace(/~/g,'%7E'));
	}

	if(xivo_is_undef(this._encode[type]) === true)
		return(encode_uri(str));

	var rs = str.split(this._encode[type]);

	if((nb = rs.length) === 1)
	{
		if(type === 'query_key' || type === 'query_val')
			return(rs[0].replace(/ /g,'+'));

		return(rs[0]);
	}

	var r = '';

	for(var i = 0;i < nb;i += 2)
	{
		r += rs[i];

		if((i + 1) < nb)
			r += encode_uri(rs[i + 1]);
	}

	if(type === 'query_key' || type === 'query_val')
		return(r.replace(/ /g,'+'));

	return(r);
}

xivo.uri.prototype.valid_scheme = function(scheme)
{
	if(xivo_is_scalar(scheme) === false)
		return(false);
	
	return((xivo_string(scheme).match(this._regexp.scheme) !== null));
}

xivo.uri.prototype.valid_ipv4 = function(ip)
{
	return((xivo_ip2long(ip) !== false));
}

xivo.uri.prototype.valid_ipv6 = function(ip)
{
	if(xivo_is_scalar(ip) === false)
		return(false);

	ip = xivo_string(ip);

	if((pos = ip.indexOf('::')) === -1)
		return((this._valid_rightipv6(ip) === 8));
	else if((ip.split(/::/).length - 1) !== 1
	|| (right = this._valid_rightipv6(ip.substring(pos + 2))) === false
	|| (left = this._valid_leftipv6(ip.substring(0,pos))) === false)
		return(false);

	return((right + left < 8));
}

xivo.uri.prototype._valid_h16 = function(h16)
{
	h16 = xivo_string(h16);

	if(((b16 = parseInt(h16,16).toString(10)) !== '0'
	   || h16.match(/^[0-9]+$/) !== null) === true
	&& b16 >= 0 && b16 <= 65535)
		return(true);
	
	return(false);
}

xivo.uri.prototype._valid_rightipv6 = function(ip)
{
	if((ip = xivo_string(ip)) === '')
		return(0);

	var arrtmp = ip.split(/:/);

	var arr = arrtmp.splice(0,8);

	if(ip !== arr[0])
		arr.push(arrtmp.join(':'));

	var nb = arr.length;

	if(nb > 8 || (nb > 7 && ip.indexOf('.') !== -1) === true
	|| (nb > 1 && arr.slice(0,-1).join('').match(/^[\da-fA-F]+$/) !== null) === true)
		return(false);
	
	var h16_cnt = 0;

	if(arr[nb - 1].indexOf('.') !== -1)
	{
		if(this.valid_ipv4(arr[nb - 1]) === false)
			return(false);

		h16_cnt = 2;

		if(nb === 1)
			return(h16_cnt);

		nb--;

		arr = arr.slice(0,-1);
	}

	for(var i = 0;i < nb;i++)
	{
		if(this._valid_h16(arr[i]) === false)
			return(false);
	}

	return(h16_cnt + nb);
}

xivo.uri.prototype._valid_leftipv6 = function(ip)
{
	if((ip = xivo_string(ip)) === '')
		return(0);

	var arrtmp = ip.split(/:/);

	var arr = arrtmp.splice(0,8);

	if(ip !== arr[0])
		arr.push(arrtmp.join(':'));

	var nb = arr.length;

	if(nb > 7
	|| (nb > 1 && arr.slice(0,-1).join('').match(/^[\da-fA-F]+$/) !== null) === true)
		return(false);

	for(var i = 0;i < nb;i++)
	{
		if(this._valid_h16(arr[i]) === false)
			return(false);
	}

	return(nb);
}

xivo.uri.prototype.valid_ipvfuture = function(ip)
{
	if(xivo_is_scalar(ip) === true
	&& ip.match(this._regexp.ipvfuture) !== null)
		return(true);
	
	return(false);
}

xivo.uri.prototype.valid_ipliteral = function(ip)
{
	if(xivo_is_scalar(ip) === false)
		return(false);

	ip = xivo_string(ip);

	var len = ip.length;

	if(len < 2 || ip[0] !== '[' || ip[len - 1] !== ']')
		return(false);
	
	ip = xivo_substr(ip,1,-1);

	if(this.valid_ipv6(ip) === true || this.valid_ipvfuture(ip) === true)
		return(true);
	
	return(false);
}

xivo.uri.prototype.valid_host = function(host)
{
	switch(this.get_host_type(host))
	{
		case this._host.reg_name:
			if(xivo_is_scalar(host) === true
			&& xivo_string(host).match(this._regexp.host) !== null)
				return(true);
			break;
		case this._host.ipliteral:
			if(this.valid_ipliteral(host) === true)
				return(true);
			break;
		default:
			return(true);
	}

	return(false);
}

xivo.uri.prototype.valid_authority = function(host,user,password,port)
{
	if(xivo_is_undef(host) === true)
		host = '';
	else if(xivo_is_scalar(host) === false)
		return(false);
	
	if(xivo_is_undef(user) === true)
		user = '';
	else if(xivo_is_scalar(user) === false)
		return(false);
	
	if(xivo_is_undef(password) === true)
		password = '';
	else if(xivo_is_scalar(password) === false)
		return(false);
	
	if(xivo_is_undef(port) === true)
		port = '';
	else if(xivo_is_scalar(port) === false)
		return(false);

	host		= xivo_string(host);
	user		= xivo_string(user);
	password	= xivo_string(password);
	port		= xivo_string(port);

	if((host !== ''
	   && this.valid_host(host) === false) === true
	|| (user !== ''
	   && this.valid_user(user) === false) === true
	|| (password !== ''
	   && this.valid_password(password) === false) === true
	|| (port !== ''
	   && this.valid_port(port) === false) === true
	|| (host === ''
	   && user === ''
	   && password === ''
	   && port === '') === true)
		return(false);
	
	return(true);
}

xivo.uri.prototype.valid_user = function(user)
{
	if(xivo_is_scalar(user) === true
	&& user.match(this._regexp.user) !== null)
		return(true);

	return(false);
}

xivo.uri.prototype.valid_password = function(password)
{
	if(xivo_is_scalar(password) === true
	&& password.match(this._regexp.password) !== null)
		return(true);

	return(false);
}

xivo.uri.prototype.valid_port = function(port)
{
	if(xivo_is_scalar(port) === true
	&& port.match(this._regexp.port) !== null)
		return(true);

	return(false);
}

xivo.uri.prototype.valid_path = function(path)
{
	if(xivo_is_scalar(path) === true
	&& path.match(this._regexp.path) !== null)
		return(true);

	return(false);
}

xivo.uri.prototype.valid_query = function(query)
{
	if(xivo_is_scalar(query) === true
	&& query.match(this._regexp.query) !== null)
		return(true);

	return(false);
}

xivo.uri.prototype.valid_fragment = function(fragment)
{
	return(this.valid_query(fragment));
}
