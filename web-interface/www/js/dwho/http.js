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

if(typeof(dwho) === 'undefined')
	var dwho = {'http': {}};
else if(typeof(dwho.http) === 'undefined')
	dwho.http = {};

dwho.http = function(url,options,data,send)
{
	this._url		= null;
	this._urlparsed		= null;
	this._uri		= null;
	this._request		= null;
	this._completed		= false;
	this._status		= 0;
	this._data		= null;
	this._datatosend	= null;
	this._intervalid	= null;
	this._timeoutid		= null;
	this._content_type	= 'application/x-www-form-urlencoded';
	this._options		= {'type':	null,
				   'method':	'get',
				   'async':	true,
				   'username':	null,
				   'password':	null,
				   'cache':	true,
				   'timeout':	0,
				   'charset':	'utf-8',
				   'scriptid':	null,
				   'callbackcomplete':	null,
				   'callbackerror':	null,
				   'callbacksuccess':	null};
	this._accepts_type	= {
		'xml':		['application/xml','text/xml'],
		'html':		['text/html'],
		'script':	['text/javascript','application/javascript'],
		'json':		['application/json','text/javascript'],
		'text':		['text/plain']};

	this._uri = new dwho.uri();
	this._urlparsed = this._uri.parse(url);

	if(dwho_is_object(options) === true)
	{
		for(var property in options)
			this.set_option(property,options[property]);
	}

	if(this._options.type === 'script')
		this._options.method = 'get';

	this._set_data(data);
	this._prepare_request();

	if(Boolean(send) === true)
		this.send();
}

dwho.http.prototype.set_option = function(name,value)
{
	if(dwho_is_undef(this._options[name]) === true)
		return(false);

	switch(name)
	{
		case 'type':
			if(this.valid_type(value) === false)
				return(false);
			break;
		case 'method':
			if(dwho_is_string(value) === false)
				return(false);
			else if(value.toLowerCase() !== 'post')
				value = 'get';
			else
				value = 'post';
			break;
		case 'async':
		case 'cache':
			value = Boolean(value);
			break;
		case 'timeout':
			if(dwho_is_uint(value) === false)
				return(false);
			break;
		case 'charset':
			if(dwho_has_len(value) === false)
				value = null;

			value = dwho_string(value);

			if(value.match(/^[a-zA-Z0-9-_]+$/) === null)
				return(false);
			break;
		case 'username':
			if(dwho_has_len(value) === false)
				return(false);

			user = this._uri.encode(value,'user');

			if(this._uri.valid_user(user) === false)
				return(false);
			break;
		case 'password':
			if(dwho_has_len(value) === false)
				return(false);

			passwd = this._uri.encode(value,'password');

			if(this._uri.valid_password(passwd) === false)
				return(false);
			break;
		case 'scriptid':
			if(dwho_has_len(value) === false)
				return(false);

			value = dwho_string(value);

			if(value.match(/^[a-zA-Z-_][a-zA-Z0-9-_]*$/) === null)
				return(false);
			break;
		case 'callbackcomplete':
		case 'callbackerror':
		case 'callbacksuccess':
			if(dwho_is_function(value) === false)
				return(false);
			break;
		default:
			return(false);
	}

	this._options[name] = value;

	return(true);
}

dwho.http.prototype.get_option = function(name)
{
	if(dwho_is_undef(this._options[name]) === true)
		return(false);

	return(this._options[name]);
}

dwho.http.prototype.valid_type = function(type)
{
	return((dwho_is_undef(this._accepts_type[type]) === false));
}

dwho.http.prototype.get_accept = function()
{
	if(this._options.type === null)
		return('*/*');

	return(this._accepts_type[this._options.type].join(', ') + ', */*');
}

dwho.http.prototype.get_request = function()
{
	return(this._request);
}

dwho.http.prototype.get_transport = function()
{
	if(dwho_is_undef(window.ActiveXObject) === false)
		return(new ActiveXObject('Microsoft.XMLHTTP'));
	else
		return(new XMLHttpRequest());
}

dwho.http.prototype._set_data = function(data)
{
	this._data = null;

	if(dwho_has_len(data) === true)
	{
		data = dwho_string(data);

		if(data[0] !== '?')
			data = '?' + data;

		if((data = this._uri.parse(data)) === false)
			return(false);
		else
			this._data = data;
		return(true);
	} else if(dwho_type_object(data) === false)
		return(false);

	this._data = [];
	this._prepare_data(data,null);
}

dwho.http.prototype._prepare_data = function(data,prevkey)
{
	if(dwho_type_object(data) === true)
	{
		for(var vkey in data)
		{
			if(dwho_has_len(prevkey) === true)
				nkey = prevkey + '[' + vkey + ']';
			else
				nkey = vkey;

			if(dwho_has_len(data[vkey]) === true)
				this._data.push([nkey,data[vkey]]);
			else
				this._prepare_data(data[vkey],nkey);
		}
	}
	else if(dwho_has_len(data) === true)
		this._data.push([prevkey,data]);
}

dwho.http.prototype.build_url = function()
{
	this._url		= dwho_clone(this._urlparsed);
	this._datatosend	= null;

	if(this._options.method === 'post')
	{
		if(dwho_is_array(this._data) === true)
			this._datatosend = this._uri.build_query(this._data);
		else if(dwho_is_object(this._data) === true)
		{
			this._datatosend = this._uri.build_query(this._data.query);

			if(dwho_is_string(this._data.fragment) === true)
				this._url.fragment = this._data.fragment;
		}
	}
	else if(dwho_is_array(this._data) === true)
	{
		if(dwho_is_undef(this._url.query) === false)
			this._url.query = this._url.query.concat(this._data);
		else
			this._url.query = this._data;
	}
	else if(dwho_is_object(this._data) === true)
	{
		if(dwho_is_array(this._data.query) === true)
		{
			if(dwho_is_undef(this._url.query) === false)
				this._url.query = this._url.query.concat(this._data.query);
			else
				this._url.query = this._data.query;
		}

		if(dwho_is_string(this._data.fragment) === true)
			this._url.fragment = this._data.fragment;
	}

	if(this._options.username === null
	&& dwho_has_len(this._url.user) === true)
	{
		this._options.username = this._url.user;

		if(this._options.password === null
		&& dwho_has_len(this._url.password) === true)
			this._options.password = this._url.password;
	}

	if(this._options.type === 'script')
	{
		if(this._options.username !== null)
		{
			this._url.user = this._options.username;

			if(this._options.password !== null)
				this._url.password = this._options.password;
		}
	}

	if(this._options.cache === false)
	{
		if(dwho_is_undef(this._url.query) === true
		|| dwho_is_array(this._url.query) === false)
			this._url.query = [];

		this._url.query.push([new Date().getTime()]);
	}

	return(this._uri.build(this._url));
}

dwho.http.prototype._prepare_request = function()
{
	if(this._options.type === 'script')
	{
		this._request		= document.createElement('script');
		this._request.type	= this._accepts_type['script'][0];

		if(dwho_has_len(this._options.charset) === true)
			this._request.charset = this._options.charset;

		if(dwho_has_len(this._options.scriptid) === true)
			this._request.id = this._options.scriptid;

		this._request.src = this.build_url();

		return(true);
	}

	this._request = this.get_transport();

	if(dwho_has_len(this._options.username) === false)
		this._request.open(this._options.method,
				   this.build_url(),
				   this._options.async);
	else
		this._request.open(this._options.method,
				   this.build_url(),
				   this._options.async,
				   this._options.username,
				   this._options.password);

	try
	{
		if(this._data !== null)
			this._request.setRequestHeader('Content-Type',this._content_type);

		this._request.setRequestHeader('Accept',this.get_accept());

		if(dwho_has_len(this._options.charset) === true)
			this._request.setRequestHeader('Accept-Charset',this._options.charset);

		if(this._method === 'post'
		&& dwho_is_undef(this._request.overrideMimeType) === false)
			this._request.setRequestHeader('Connection','close');
	}
	catch(e) {}

	this._request.onreadystatechange = this.onreadystatechange();

	if(this._options.async === true)
	{
		this.startinterval();
		this.starttimeout();
	}
}

dwho.http.prototype.startinterval = function()
{
	this.deleteinterval();

	var xhptr = this;
	this._intervalid = window.setInterval(function() { xhptr.onreadystatechange(); },10);
}

dwho.http.prototype.deleteinterval = function()
{
	if(this._intervalid !== null)
		window.clearInterval(this._intervalid);

	this._intervalid = null;
}

dwho.http.prototype.starttimeout = function ()
{
	this.deletetimeout();

	if(this._options.timeout === 0)
		return(false);

	var xhptr = this;
	this._timeoutid = window.setTimeout(
				function()
				{
					if(xhptr.iscompleted() === false)
						xhptr.onreadystatechange(true);
				},
				this._options.timeout);
}

dwho.http.prototype.deletetimeout = function ()
{
	if(this._timeoutid !== null)
		window.clearTimeout(this._timeoutid);

	this._timeoutid = null;
}

dwho.http.prototype.iscompleted = function()
{
	return(Boolean(this._completed));
}

dwho.http.prototype.get_status = function()
{
	return(this._status);
}

dwho.http.prototype.success = function()
{
	if(this._request === null)
		return(null);

	this._status = this._request.status;

	if(dwho_is_empty(this._request.status) === true
	&& window.location.protocol === 'file:')
		this._status = 200;
	else if(this._request.status === 1223)
		this._status = 204;

	if((this._status >= 200
	   && this._status < 300) === true
	|| this._status === 304)
		return(true);

	return(false);
}

dwho.http.prototype.onreadystatechange = function(timeout)
{
	timeout = Boolean(timeout);

	if(this._completed === true || this._request === null)
		return(null);
	else if(this._request.readyState === 0)
	{
		this.deleteinterval();
		return(null);
	}
	else if(timeout === false && this._request.readyState !== 4)
		return(null);

	this._completed = true;

	this.deleteinterval();

	if(timeout === false)
	{
		if(this.success() === true)
		{
			if(this._options.callbacksuccess !== null)
				this._options.callbacksuccess(this._request);
		}
		else if(this._options.callbackerror !== null)
			this._options.callbackerror(this._request);
	}

	if(this._options.callbackcomplete !== null)
		this._options.callbackcomplete(this._request);

	if(timeout === true)
		this._request.abort();

	if(this._options.async === true)
		this._request = null;

	return(true);
}

dwho.http.prototype.send = function(cache)
{
	if(this._options.type === 'script')
	{
		if(dwho_has_len(this._request.id) === true)
			dwho.dom.remove_element(dwho_eid(this._request.id,true));

		dwho.dom.etag('head',null,0).appendChild(this._request);
		return(true);
	}

	try
	{
		this._request.send(this._datatosend);
	}
	catch(e)
	{
		if(this._options.callbackerror !== null)
			this._options.callbackerror(this._request);

		return(false);
	}

	if(this._options.async === false)
		this.onreadystatechange();

	return(true);
}

dwho.http.prototype.reload = function(cache)
{
	cache = dwho_is_undef(cache) === true ? this._options.cache : Boolean(cache);

	this._completed = false;

	if(this._request === null || cache === false)
		this._prepare_request();

	return(this.send(cache));
}
