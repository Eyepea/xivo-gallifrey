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
	var dwho = {'submenu': {}};
else if(typeof(dwho.submenu) === 'undefined')
	dwho.submenu = {};

dwho.submenu = function(options)
{
	this._backup	= {};
	this._before	= {'id':	"",
			   'class':	""};
	this._selected	= {'id':	"",
			   'class':	""};
	this._display	= null;

	this._class	= {'blur':	"dwsm-blur",
			   'focus':	"dwsm-focus",
			   'select':	"dwsm-select"};

	this._onload	= {'tab':	"dwsm-tab-1",
			   'part':	"sb-part-first",
			   'last':	false};

	this.set_options(options);

	var dwsmptr = this;

	this.fnonload = function() { dwsmptr.load(); }
}

dwho.submenu.prototype.set_option = function(name,value)
{
	switch(name)
	{
		case 'class_onblur':
		case 'class_onfocus':
		case 'class_onselect':
			if(dwho_is_string(value) === false)
				return(false);

			this._class[dwho_substr(name,8)] = value;
			break;
		case 'onload_tab':
		case 'onload_part':
			if(dwho_is_string(value) === false)
				return(false);

			this._onload[dwho_substr(name,7)] = value;
			break;
		case 'onload_last':
			this._onload['last'] = dwho_bool(value);
			break;
	}

	return(true);
}

dwho.submenu.prototype.set_options = function(options)
{
	if(dwho_is_object(options) === false)
		return(false);

	for(var property in options)
		this.set_option(property,options[property]);
}

dwho.submenu.prototype.select = function(obj,part,last,cname)
{
	if(dwho_has_len(obj.id) === false)
		return(false);

	if(dwho_has_len(cname) === false)
		cname = this._class['select'];

	var displayobj = selectobj = beforeobj = null;
	var id = obj.id;

	if(dwho_has_len(this._display) === true
	&& (displayobj = dwho_eid(this._display)) !== false)
		displayobj.style.display = 'none';
	
	if((displayobj = dwho_eid(part)) !== false)
	{
		this._display = part;
		displayobj.style.display = 'block';
	}

	if(dwho_has_len(this._selected['id']) === true
	&& (selectobj = dwho_eid(this._selected['id'])) !== false)
		selectobj.className = this._selected['class'];
	
	if(dwho_is_undef(this._backup[id]) === false)
	{
		this._selected['id'] = id;
		this._selected['class'] = this._backup[id];
	}

	if(dwho_has_len(this._before['id']) === true
	&& (beforeobj = dwho_eid(this._before['id'])) !== false)
		beforeobj.className = this._before['class'];
	
	var rs = id.match(/^([a-zA-Z0-9-_]*)([0-9]+)$/);

	if(rs !== null)
	{
		var num = Number(rs[2]);
		var nid = rs[1] + (num - 1);
		var nidobj = null;

		if(num > 1 && (nidobj = dwho_eid(nid)) !== false)
		{
			if(dwho_is_undef(this._backup[nid]) === true)
				this._backup[nid] = nidobj.className;

			this._before['id'] = nid;
			this._before['class'] = nidobj.className;
			nidobj.className = cname + '-before';
		}
		else
			this._before['id'] = this._before['class'] = '';
	}

	obj.className = Boolean(last) === false ? cname : cname + '-last';

	dwho.dom.free_focus();
}

dwho.submenu.prototype.onstate = function(obj,last,cname,state)
{
	if(state !== 'focus')
		state = 'blur';

	if(dwho_has_len(cname) === false)
		cname = this._class[state];

	if((ul = dwho.dom.etag('ul',obj,0)) !== false)
		ul.style.display = state === 'blur' ? 'none' : 'block';

	if(dwho_has_len(obj.id) === false || this._selected['id'] === obj.id)
		return(false);

	var id = obj.id;

	if(state === 'focus' && dwho_is_undef(this._backup[id]) === true)
		this._backup[id] = obj.className;

	if(Boolean(last) === true)
	{
		obj.className = cname + '-last';
		return(true);
	}

	var rs = id.match(/^([a-zA-Z0-9-_]*)([0-9]+)$/);

	if(rs !== null)
	{
		var num = Number(rs[2]);
		var nid = rs[1] + (num + 1);

		if(this._selected['id'] === nid)
		{
			obj.className = cname + '-before';
			return(true);
		}
	}

	obj.className = cname;

	return(true);
}

dwho.submenu.prototype.blur = function(obj,last,cname)
{
	return(this.onstate(obj,last,cname,'blur'));
}

dwho.submenu.prototype.focus = function(obj,last,cname)
{
	return(this.onstate(obj,last,cname,'focus'));
}

dwho.submenu.prototype.submit_form = function(obj)
{
	if(dwho_is_object(obj) === false
	|| dwho_is_string(obj.nodeName) === false
	|| obj.nodeName.toLowerCase() !== 'form'
	|| dwho_is_undef(obj['dwsm-form-tab']) === true
	|| dwho_is_undef(obj['dwsm-form-part']) === true
	|| dwho_has_len(this._selected['id']) === false
	|| dwho_has_len(this._display) === false)
		return(false);

	obj['dwsm-form-tab'].value = this._selected['id'];
	obj['dwsm-form-part'].value = this._display;

	return(true);
}

dwho.submenu.prototype.load = function()
{
	if((tab = dwho_eid(this._onload['tab'])) === false)
		return(false);

	this._backup[this._onload['tab']] = tab.className;
	this.select(tab,this._onload['part'],this._onload['last']);
}

if(dwho_is_undef(dwho.dom) === false
&& (typeof(dwho.submenu.autoload) === 'undefined'
   || dwho_bool(dwho.submenu.autoload) === true) === true)
{
	var dwho_submenu = new dwho.submenu();
	dwho.dom.set_onload(dwho_submenu.fnonload);
}
