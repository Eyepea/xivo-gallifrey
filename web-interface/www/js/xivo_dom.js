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
	xivo = {'dom': {'node': {}}};
else if(xivo_is_undef(xivo.dom) === true)
	xivo.dom = {'node': {}};
else if(xivo_is_undef(xivo.dom.node) === true)
	xivo.dom.node = {};

xivo.dom.eids = {};
xivo.dom.table_list = {};
xivo.dom.callback_onload = [];

xivo.dom.eid = function(id,forcereload)
{
	if(Boolean(forcereload) === false && xivo_is_undef(xivo.dom.eids[id]) === false)
		return(xivo.dom.eids[id]);
	else if((get = document.getElementById(id)))
		return((xivo.dom.eids[id] = get));

	return(false);
}

xivo_eid = xivo.dom.eid;

xivo.dom.add_cssclass = function(obj,classname)
{
	if(xivo_is_object(obj) === false
	|| xivo_is_undef(obj.className) === true)
		return(false);

	if(xivo_has_len(classname) === true)
		var list = xivo_object_flip(classname.split(/\s+/));
	else if(xivo_is_array(classname) === true)
		var list = xivo_object_flip(classname);
	else
		return(false);

	var rs = obj.className.split(/\s+/);

	objlist = xivo_object_flip(rs);

	for(var property in list)
	{
		if(xivo_has_len(property) === true
		&& xivo_is_undef(objlist[property]) === true)
			rs.push(property);
	}

	obj.className = rs.join(' ');

	return(true);
}

xivo.dom.remove_cssclass = function(obj,classname)
{
	if(xivo_is_object(obj) === false
	|| xivo_is_undef(obj.className) === true)
		return(false);

	if(xivo_has_len(classname) === true)
		var list = xivo_object_flip(classname.split(/\s+/));
	else if(xivo_is_array(classname) === true)
		var list = xivo_object_flip(classname);
	else
		return(false);

	var rs = [];

	objlist = xivo_object_flip(obj.className.split(/\s+/));

	for(var property in objlist)
	{
		if(xivo_is_undef(list[property]) === true)
			rs.push(property);
	}

	obj.className = rs.join(' ');

	return(true);
}

xivo.dom.add_event = function(type,obj,fn)
{
	if(xivo_is_string(type) === false
	|| xivo_is_object(obj) === false
	|| xivo_is_function(fn) === false)
		return(false);
	else if(xivo_is_undef(obj.addEventListener) === false)
		return(obj.addEventListener(type,fn,false));
	else if(xivo_is_undef(obj.attachEvent) === false)
		return(obj.attachEvent('on' + type,fn));

	return(false);
}

xivo.dom.remove_event = function(type,obj,fn)
{
	if(xivo_is_string(type) === false
	|| xivo_is_object(obj) === false
	|| xivo_is_function(fn) === false)
		return(false);
	else if(xivo_is_undef(obj.removeEventListener) === false)
		return(obj.removeEventListener(type,fn,false));
	else if(xivo_is_undef(obj.detachEvent) === false)
		return(obj.detachEvent('on' + type,fn));

	return(false);
}

xivo.dom.create_element = function(tag,attr,content,html)
{
	if(xivo_is_string(tag) === false)
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

xivo.dom.remove_element = function(obj,nb)
{
	if(xivo_is_object(obj) === false)
		return(false);
	else if(xivo_is_uint(nb) === false || nb < 1)
		nb = 1;

	var pnode = obj;

	for(var i = 0;i < nb;i++)
	{
		if(xivo_is_undef(pnode.parentNode) === true)
			return(false);

		pnode = pnode.parentNode;
	}

	if(xivo_is_object(pnode) === false
	|| xivo_is_undef(pnode.removeChild) === true)
		return(false);

	return(pnode.removeChild(obj));
}

xivo.dom.etag = function(tag,obj,nb)
{
	if(xivo_is_string(tag) === false)
		return(false);
	else if(obj === null || xivo_is_undef(obj) === true)
		obj = document;

	if(xivo_is_object(obj) === false
	|| xivo_is_undef(obj.getElementsByTagName) === true)
		return(false);
	else if(xivo_is_undef(nb) === true)
		return(obj.getElementsByTagName(tag));
	else if(xivo_is_uint(nb) === false)
		nb = 0;

	if(xivo_is_undef(obj.getElementsByTagName(tag)[nb]) === true)
		return(false);

	return(obj.getElementsByTagName(tag)[nb]);
}

xivo.dom.ename = function(name,obj,nb)
{
	if(xivo_is_string(name) === false)
		return(false);
	else if(xivo_is_undef(obj) === true)
		obj = document;

	if(xivo_is_object(obj) === false
	|| xivo_is_undef(obj.getElementsByName) === true)
		return(false);
	else if(xivo_is_undef(nb) === true)
		return(obj.getElementsByName(name));
	else if(xivo_is_uint(nb) === false)
		nb = 0;

	if(xivo_is_undef(obj.getElementsByName(name)[nb]) === true)
		return(false);

	return(obj.getElementsByName(name)[nb]);
}

xivo.dom.free_focus = function()
{
	xivo.dom.etag('a',null,0).focus();

	return(false);
}

xivo.dom.get_offset_position = function(obj)
{
	if(xivo_is_object(obj) === false)
		return(false);

	ret = {'x':	0,
	       'y':	0};

	if(xivo_is_undef(obj.offsetParent) === false)
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
		if(xivo_is_undef(obj.x) === false)
			ret.x = obj.x;

		if(xivo_is_undef(obj.y) === false)
			ret.y = obj.y;
	}

	return(ret);
}

xivo.dom.get_parent_by_tag = function(obj,tag)
{
	if(xivo_is_object(obj) === false
	|| xivo_is_string(tag) === false)
		return(false);

	tag = tag.toLowerCase();

	for(var i = 0;i < 10;i++)
	{
		if(xivo_is_undef(obj.parentNode) === true
		|| xivo_is_undef(obj.tagName) === true)
			return(false);

		obj = obj.parentNode;

		if(obj.tagName.toLowerCase() === tag)
			return(obj);
	}

	return(false);
}

xivo.dom.get_table_idcnt = function(name)
{
	if(xivo_is_undef(xivo.dom.table_list[name]) === true
	|| xivo_type_object(xivo.dom.table_list[name]) === false
	|| xivo_is_undef(xivo.dom.table_list[name]['idcnt']) === true)
		return(false);

	return(xivo.dom.table_list[name]['idcnt']);
}

xivo.dom.get_table_cnt = function(name)
{
	if(xivo_is_undef(xivo.dom.table_list[name]) === true
	|| xivo_type_object(xivo.dom.table_list[name]) === false
	|| xivo_is_undef(xivo.dom.table_list[name]['cnt']) === true)
		return(false);

	return(xivo.dom.table_list[name]['cnt']);
}

xivo.dom.set_table_list = function(name,cnt)
{
	if(xivo_is_uint(cnt) === false)
		cnt = 0;

	xivo.dom.table_list[name] = {'cnt': Number(cnt)};
}

xivo.dom.make_table_list = function(name,obj,del,idcnt)
{
	if(xivo_is_undef(xivo.dom.table_list[name]) === true
	|| xivo_type_object(xivo.dom.table_list[name]) === false)
		xivo.dom.table_list[name] = {'cnt':	0,
					     'node':	''};

	if(xivo_is_undef(xivo.dom.table_list[name]['cnt']) === true)
		xivo.dom.table_list[name]['cnt'] = 0;

	if(xivo_is_undef(xivo.dom.table_list[name]['idcnt']) === true)
		xivo.dom.table_list[name]['idcnt'] = xivo.dom.table_list[name]['cnt'];

	if(xivo_is_undef(xivo.dom.table_list[name]['node']) === true)
		xivo.dom.table_list[name]['node'] = '';

	var ref = xivo.dom.table_list[name];

	if(ref['node'] === false
	|| (ref['node'] === ''
	   && (ref['node'] = xivo.dom.etag('tr',xivo_eid('ex-'+name),0)) === false) === true)
		return(false);

	if(Boolean(del) === true)
	{
		if(xivo_is_object(obj) === false
		|| xivo_is_object(obj.parentNode) === false
		|| xivo_is_object(obj.parentNode.parentNode) === false
		|| xivo_is_object(obj.parentNode.parentNode.parentNode) === false)
			return(false);

		node = obj.parentNode.parentNode;
		node.parentNode.removeChild(node);
		node = null;

		if(--ref['cnt'] < 1)
		{
			ref['cnt'] = 0;
			xivo_eid('no-'+name).style.display = 'table-row';
		}
	}
	else
	{
		if(xivo_eid('ex-'+name) === false)
			return(false);

		if(++ref['cnt'] === 1)
			xivo_eid('no-'+name).style.display = 'none';

		var xivo_node_clone = ref['node'].cloneNode(true);

		xivo_fm_field_disabled(xivo_node_clone,false);
		xivo_fm_onfocus_onblur(xivo_node_clone);

		if(Boolean(idcnt) === true)
			xivo_fm_field_id_counter(xivo_node_clone,
						 ++xivo.dom.table_list[name]['idcnt']);

		xivo_eid(name).appendChild(xivo_node_clone);
	}

	return(true);
}

xivo.dom.node.empty = function(node)
{
	if(node.nodeType === 8
	|| (node.nodeType === 3
	   && node.data.match(/[^\n\r\t ]/) === null) === true)
	   	return(true);

	return(false);
}

xivo.dom.node.previous = function(node)
{
	while((node = node.previousSibling))
	{
		if(xivo.dom.node.empty(node) === false)
			return(node);
	}

	return(false);
}

xivo.dom.node.next = function(node)
{
	while((node = node.nextSibling))
	{
		if(xivo.dom.node.empty(node) === false)
			return(node);
	}

	return(false);
}

xivo.dom.node.firstchild = function(node)
{
	var child = node.firstChild;

 	while(child)
  	{
 		if(xivo.dom.node.empty(child) === false)
			return(child);

		child = child.nextSibling;
	}

	return(false);
}

xivo.dom.node.lastchild = function(node)
{
	var child = node.lastChild;

 	while(child)
  	{
 		if(xivo.dom.node.empty(child) === false)
			return(child);

		child = child.previousSibling;
	}

	return(false);
}

xivo.dom.set_onload = function(fn,args)
{
	if(xivo_is_function(fn) === true)
		xivo.dom.callback_onload.push([fn,args]);
}

xivo.dom.call_onload = function()
{
	for(var property in xivo.dom.callback_onload)
		xivo.dom.callback_onload[property][0].apply(null,
							    xivo.dom.callback_onload[property][1]);
}

xivo.dom.add_event('load',window,xivo.dom.call_onload);
