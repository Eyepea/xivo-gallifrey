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

function xivo_empty_data(data)
{
	return(data.match(/[^\n\r\t ]/) === null);
}

function xivo_empty_node(node)
{
	if(node.nodeType === 8
	|| (node.nodeType === 3
	   && xivo_empty_data(node.data) === true) === true)
	   	return(true);

	return(false);
}

function xivo_previous_node(node)
{
	while((node = node.previousSibling))
	{
		if(xivo_empty_node(node) === false)
			return(node);
	}

	return(false);
}

function xivo_next_node(node)
{
	while((node = node.nextSibling))
	{
		if(xivo_empty_node(node) === false)
			return(node);
	}

	return(false);
}

function xivo_firstchild(obj)
{
	var child = obj.firstChild;

 	while(child)
  	{
 		if(xivo_empty_node(child) === false)
			return(child);

		child = child.nextSibling;
	}

	return(false);
}

function xivo_lastchild(obj)
{
	var child = obj.lastChild;

 	while(child)
  	{
 		if(xivo_empty_node(child) === false)
			return(child);

		child = child.previousSibling;
	}

	return(false);
}
