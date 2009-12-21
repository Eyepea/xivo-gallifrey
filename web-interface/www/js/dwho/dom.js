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
	var dwho = {'dom': {'node': {}}};
else if(typeof(dwho.dom) === 'undefined')
	dwho.dom = {'node': {}};
else if(typeof(dwho.dom.node) === 'undefined')
	dwho.dom.node = {};

dwho.dom.eids = {};
dwho.dom.table_list = {};
dwho.dom.callback_onload = [];

dwho.dom.eid = function(id,forcereload)
{
	if(Boolean(forcereload) === false && dwho_is_undef(dwho.dom.eids[id]) === false)
		return(dwho.dom.eids[id]);
	else if((get = document.getElementById(id)))
		return((dwho.dom.eids[id] = get));

	return(false);
}

var dwho_eid = dwho.dom.eid;

dwho.dom.has_cssclass = function(obj,classname,strict)
{
	if(dwho_is_object(obj) === false
	|| dwho_is_undef(obj.className) === true)
		return(false);

	if(dwho_has_len(classname) === true)
		var list = dwho_object_flip(classname.split(/\s+/));
	else if(dwho_is_array(classname) === true)
		var list = dwho_object_flip(classname);
	else
		return(false);

	strict = dwho_is_undef(strict) === true ? true : Boolean(strict);

	var rs = obj.className.split(/\s+/);

	objlist = dwho_object_flip(rs);

	var undef = null;

	for(var property in list)
	{
		if(dwho_has_len(property) === false
		|| (undef = dwho_is_undef(objlist[property])) !== strict)
			continue;
		else
			return((undef === false));
	}

	return(strict);
}

dwho.dom.add_cssclass = function(obj,classname,mode)
{
	if(dwho_is_object(obj) === false
	|| dwho_is_undef(obj.className) === true)
		return(false);
	else if(mode !== 'unshift' && mode !== 'push')
		mode = 'push';

	if(dwho_has_len(classname) === true)
		var list = dwho_object_flip(classname.split(/\s+/));
	else if(dwho_is_array(classname) === true)
		var list = dwho_object_flip(classname);
	else
		return(false);

	var rs = obj.className.split(/\s+/);

	objlist = dwho_object_flip(rs);

	for(var property in list)
	{
		if(dwho_has_len(property) === false)
			continue;
		else if(dwho_is_undef(objlist[property]) === false)
			rs.splice(objlist[property],1);

		rs[mode](property);
	}

	obj.className = rs.join(' ');

	return(true);
}

dwho.dom.unshift_cssclass = function(obj,classname)
{
	return(dwho.dom.add_cssclass(obj,classname,'unshift'));
}

dwho.dom.push_cssclass = function(obj,classname)
{
	return(dwho.dom.add_cssclass(obj,classname,'push'));
}

dwho.dom.remove_cssclass = function(obj,classname)
{
	if(dwho_is_object(obj) === false
	|| dwho_is_undef(obj.className) === true)
		return(false);

	if(dwho_has_len(classname) === true)
		var list = dwho_object_flip(classname.split(/\s+/));
	else if(dwho_is_array(classname) === true)
		var list = dwho_object_flip(classname);
	else
		return(false);

	var rs = [];

	objlist = dwho_object_flip(obj.className.split(/\s+/));

	for(var property in objlist)
	{
		if(dwho_is_undef(list[property]) === true)
			rs.push(property);
	}

	obj.className = rs.join(' ');

	return(true);
}

dwho.dom.add_event = function(type,obj,fn)
{
	if(dwho_is_string(type) === false
	|| dwho_is_object(obj) === false
	|| dwho_is_function(fn) === false)
		return(false);
	else if(dwho_is_undef(obj.addEventListener) === false)
		return(obj.addEventListener(type,fn,false));
	else if(dwho_is_undef(obj.attachEvent) === false)
		return(obj.attachEvent('on' + type,fn));

	return(false);
}

dwho.dom.remove_event = function(type,obj,fn)
{
	if(dwho_is_string(type) === false
	|| dwho_is_object(obj) === false
	|| dwho_is_function(fn) === false)
		return(false);
	else if(dwho_is_undef(obj.removeEventListener) === false)
		return(obj.removeEventListener(type,fn,false));
	else if(dwho_is_undef(obj.detachEvent) === false)
		return(obj.detachEvent('on' + type,fn));

	return(false);
}

dwho.dom.create_element = function(tag,attr,content,html)
{
	if(dwho_is_string(tag) === false)
		return(false);

	var elt = document.createElement(tag);

	for(var property in attr)
		elt[property] = attr[property];

	if((tcontent = typeof(content)) === 'string')
	{
		if(Boolean(html) === false)
			elt.appendChild(document.createTextNode(content));
		else
			elt.innerHTML = content;
	}
	else if(tcontent === 'object')
		elt.appendChild(content)

	return(elt);
}

dwho.dom.remove_element = function(obj,nb)
{
	if(dwho_is_object(obj) === false)
		return(false);
	else if(dwho_is_uint(nb) === false || nb < 1)
		nb = 1;

	var pnode = obj;

	for(var i = 0;i < nb;i++)
	{
		if(dwho_is_undef(pnode.parentNode) === true)
			return(false);

		pnode = pnode.parentNode;
	}

	if(dwho_is_object(pnode) === false
	|| dwho_is_undef(pnode.removeChild) === true)
		return(false);

	return(pnode.removeChild(obj));
}

dwho.dom.etag = function(tag,obj,nb)
{
	if(dwho_is_string(tag) === false)
		return(false);
	else if(obj === null || dwho_is_undef(obj) === true)
		obj = document;

	if(dwho_is_object(obj) === false
	|| dwho_is_undef(obj.getElementsByTagName) === true)
		return(false);
	else if(dwho_is_undef(nb) === true)
		return(obj.getElementsByTagName(tag));
	else if(dwho_is_uint(nb) === false)
		nb = 0;

	if(dwho_is_undef(obj.getElementsByTagName(tag)[nb]) === true)
		return(false);

	return(obj.getElementsByTagName(tag)[nb]);
}

dwho.dom.ename = function(name,obj,nb)
{
	if(dwho_is_string(name) === false)
		return(false);
	else if(dwho_is_undef(obj) === true)
		obj = document;

	if(dwho_is_object(obj) === false
	|| dwho_is_undef(obj.getElementsByName) === true)
		return(false);
	else if(dwho_is_undef(nb) === true)
		return(obj.getElementsByName(name));
	else if(dwho_is_uint(nb) === false)
		nb = 0;

	if(dwho_is_undef(obj.getElementsByName(name)[nb]) === true)
		return(false);

	return(obj.getElementsByName(name)[nb]);
}

dwho.dom.free_focus = function()
{
	dwho.dom.etag('a',null,0).focus();

	return(false);
}

dwho.dom.confirm_newlocation = function(msg,newlocation)
{
	window.location = confirm(msg) === true ? newlocation : '#';
}

dwho.dom.set_confirm_newlocation = function(obj)
{
	if(dwho_is_object(obj) === false
	|| dwho_is_undef(obj.href) === true
	|| dwho_strcasecmp('javascript:',obj.href,11) === 0
	|| dwho_is_function(obj.onclick) === false
	|| (rs = obj.onclick.toString().match(/return confirm\((.*)\);/)) === null)
		return(false);

	obj.href	= 'javascript:dwho.dom.confirm_newlocation(' + rs[1] + ',\'' + obj.href + '\');';
	obj.onclick	= null;

	return(true);
}

dwho.dom.set_confirm_uri_onchild = function(id)
{
	if((obj = dwho_eid(id)) === false
	|| dwho_is_function(obj.getElementsByTagName) === false)
		return(false);

	var list = obj.getElementsByTagName('a');
	var nb = list.length;

	for(var i = 0;i < nb;i++)
		dwho.dom.set_confirm_newlocation(list[i]);

	return(true);
}

dwho.dom.get_offset_position = function(obj)
{
	if(dwho_is_object(obj) === false)
		return(false);

	var ret = {'x':	0,
		   'y':	0};

	if(dwho_is_undef(obj.offsetParent) === false)
	{
		while(obj.offsetParent)
		{
			ret.x += obj.offsetLeft;
			ret.y += obj.offsetTop;
			obj = obj.offsetParent;
		}
	}
	else
	{
		if(dwho_is_undef(obj.x) === false)
			ret.x = obj.x;

		if(dwho_is_undef(obj.y) === false)
			ret.y = obj.y;
	}

	return(ret);
}

dwho.dom.get_parent_by_tag = function(obj,tag)
{
	if(dwho_is_object(obj) === false
	|| dwho_is_string(tag) === false)
		return(false);

	tag = tag.toLowerCase();

	for(var i = 0;i < 10;i++)
	{
		if(dwho_is_undef(obj.parentNode) === true
		|| dwho_is_undef(obj.tagName) === true)
			return(false);

		obj = obj.parentNode;

		if(obj.tagName.toLowerCase() === tag)
			return(obj);
	}

	return(false);
}

dwho.dom.get_table_idcnt = function(name)
{
	if(dwho_is_undef(dwho.dom.table_list[name]) === true
	|| dwho_type_object(dwho.dom.table_list[name]) === false
	|| dwho_is_undef(dwho.dom.table_list[name]['idcnt']) === true)
		return(false);

	return(dwho.dom.table_list[name]['idcnt']);
}

dwho.dom.get_table_cnt = function(name)
{
	if(dwho_is_undef(dwho.dom.table_list[name]) === true
	|| dwho_type_object(dwho.dom.table_list[name]) === false
	|| dwho_is_undef(dwho.dom.table_list[name]['cnt']) === true)
		return(false);

	return(dwho.dom.table_list[name]['cnt']);
}

dwho.dom.set_table_list = function(name,cnt)
{
	if(dwho_is_uint(cnt) === false)
		cnt = 0;

	dwho.dom.table_list[name] = {'cnt': Number(cnt)};
}

dwho.dom.make_table_list = function(name,obj,del,idcnt)
{
	if(dwho_is_undef(dwho.dom.table_list[name]) === true
	|| dwho_type_object(dwho.dom.table_list[name]) === false)
		dwho.dom.table_list[name] = {'cnt':	0,
					     'node':	''};

	if(dwho_is_undef(dwho.dom.table_list[name]['cnt']) === true)
		dwho.dom.table_list[name]['cnt'] = 0;

	if(dwho_is_undef(dwho.dom.table_list[name]['idcnt']) === true)
		dwho.dom.table_list[name]['idcnt'] = dwho.dom.table_list[name]['cnt'];

	if(dwho_is_undef(dwho.dom.table_list[name]['node']) === true)
		dwho.dom.table_list[name]['node'] = '';

	var ref = dwho.dom.table_list[name];

	if(ref['node'] === false
	|| (ref['node'] === ''
	   && (ref['node'] = dwho.dom.etag('tr',dwho_eid('ex-'+name),0)) === false) === true)
		return(false);

	if(Boolean(del) === true)
	{
		if(dwho_is_object(obj) === false
		|| dwho_is_object(obj.parentNode) === false
		|| dwho_is_object(obj.parentNode.parentNode) === false
		|| dwho_is_object(obj.parentNode.parentNode.parentNode) === false)
			return(false);

		node = obj.parentNode.parentNode;
		node.parentNode.removeChild(node);
		node = null;

		if(--ref['cnt'] < 1)
		{
			ref['cnt'] = 0;
			dwho_eid('no-'+name).style.display = 'table-row';
		}
	}
	else
	{
		if(dwho_eid('ex-'+name) === false)
			return(false);

		if(++ref['cnt'] === 1)
			dwho_eid('no-'+name).style.display = 'none';

		var clonenode = ref['node'].cloneNode(true);

		dwho.form.field_disabled(clonenode,false);
		dwho.form.onfocus_onblur(clonenode);

		if(Boolean(idcnt) === true)
			dwho.form.field_id_counter(clonenode,
						 ++dwho.dom.table_list[name]['idcnt']);

		dwho_eid(name).appendChild(clonenode);
	}

	return(true);
}

dwho.dom.node.empty = function(node)
{
	if(node.nodeType === 8
	|| (node.nodeType === 3
	   && node.data.match(/[^\n\r\t ]/) === null) === true)
		return(true);

	return(false);
}

dwho.dom.node.previous = function(node)
{
	while((node = node.previousSibling))
	{
		if(dwho.dom.node.empty(node) === false)
			return(node);
	}

	return(false);
}

dwho.dom.node.next = function(node)
{
	while((node = node.nextSibling))
	{
		if(dwho.dom.node.empty(node) === false)
			return(node);
	}

	return(false);
}

dwho.dom.node.firstchild = function(node)
{
	var child = node.firstChild;

	while(child)
	{
		if(dwho.dom.node.empty(child) === false)
			return(child);

		child = child.nextSibling;
	}

	return(false);
}

dwho.dom.node.lastchild = function(node)
{
	var child = node.lastChild;

	while(child)
	{
		if(dwho.dom.node.empty(child) === false)
			return(child);

		child = child.previousSibling;
	}

	return(false);
}

dwho.dom.set_onload = function(fn)
{
	var args = Array.prototype.slice.call(arguments);
	args.shift();

	if(dwho_is_function(fn) === true)
		dwho.dom.callback_onload.push([fn,args]);
}

dwho.dom.call_onload = function()
{
	for(var property in dwho.dom.callback_onload)
		dwho.dom.callback_onload[property][0].apply(null,
							    dwho.dom.callback_onload[property][1]);
}

dwho.dom.add_event('load',window,dwho.dom.call_onload);
