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

if(typeof(dwho) === 'undefined')
	var dwho = {'json': {}};
else if(typeof(dwho.json) === 'undefined')
	dwho.json = {};

dwho.json = function()
{
	this._regexp		= {};
	this._regexp.charlist	= new RegExp('[\x5C\x2F\x22\x0C-\x0D\x08-\x0A]','g');
	this._regexp.unicode	= new RegExp('[^\x20-\x7E]','g');

	var charlist = {'"':	'"',
			'\\':	'\\',
			'/':	'/',
			'\b':	'b',
			'\f':	'f',
			'\n':	'n',
			'\r':	'r',
			'\t':	't'};
	
	this._fnescchar	= function(chr) { return('\\' + charlist[chr]); }
	this._fnescuni	= function(chr) { return('\\u' + chr.charCodeAt(0).toString(16).toUpperCase()); }

	this._fnevalrfc4627	= function(text)
				  {
					return(!(/[^,:{}\[\]0-9.\-+Eaeflnr-u \n\r\t]/.test(
					         	text.replace(/"(\\.|[^"\\])*"/g, ''))) &&
						 eval('(' + text + ')'));
				  }
}

dwho.json.prototype.escaped = function(str)
{
	return(dwho_string(str).
	       replace(this._regexp.charlist,this._fnescchar).
	       replace(this._regexp.unicode,this._fnescuni));
}

dwho.json.prototype.encode = function(data)
{
	if(typeof(JSON) !== 'undefined'
	&& dwho_is_function(JSON.stringify) === true)
		return(JSON.stringify(data));

	if(data === null || data === undefined || dwho_is_function(data) === true)
		return('null');
	else if(dwho_is_array(data) === true)
	{
		var rs = [];
		var nb = data.length;

		for(var i = 0;i < nb;i++)
			rs.push(this.encode(data[i]));

		return('[' + rs.join(',') + ']');
	}

	switch(typeof(data))
	{
		case 'boolean':
			return(String(data).toLowerCase());
		case 'number':
			return((isFinite(data) === true ? String(data) : 'null'));
		case 'string':
			return('"' + this.escaped(data) + '"');
		default:
	}

	var rs = [];

	for(property in data)
		rs.push('"' + this.escaped(property) + '":' + this.encode(data[property]));

	return('{' + rs.join(',') + '}');
}

dwho.json.prototype.decode = function(data)
{
	if(typeof(JSON) !== 'undefined'
	&& dwho_is_function(JSON.parse) === true)
		return(JSON.parse(data));

	return(this._fnevalrfc4627(data));
}
