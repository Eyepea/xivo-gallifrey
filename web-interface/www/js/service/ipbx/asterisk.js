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

function xivo_ast_inqueue()
{
	dwho.form.move_selected('it-queuelist','it-queue');

	if((queuelist = dwho_eid('it-queue')) === false
	|| (len = queuelist.length) < 1)
		return(false);

	for(i = 0;i < len;i++)
	{
		if((queue = dwho_eid('queue-'+queuelist[i].value)) !== false)
			queue.style.display = 'table-row';
	}

	if(dwho_eid('it-queue').length > 0)
		dwho_eid('no-queue').style.display = 'none';

	return(true);
}

function xivo_ast_outqueue()
{
	dwho.form.move_selected('it-queue','it-queuelist');

	if((queuelist = dwho_eid('it-queuelist')) === false
	|| (len = queuelist.length) < 1)
		return(false);

	for(i = 0;i < len;i++)
	{
		if((queue = dwho_eid('queue-'+queuelist[i].value)) !== false)
			queue.style.display = 'none';
	}

	if(dwho_eid('it-queue').length === 0)
		dwho_eid('no-queue').style.display = 'table-row';

	return(true);
}

function xivo_exten_pattern(id,option)
{
	if((id = dwho_eid(id)) === false || dwho_is_undef(id.value) === true)
		return(false);

	var value = id.value;

	if(value.charAt(0) === '_')
		value = dwho_substr(value,1);

	value = value.replace(/[X\.]/gi,'');

	if(option === '*')
	{
		id.value = value + '.';
		return(true);
	}

	option = Number(option);

	if(option > 0 && option < 40)
	{
		id.value = value + dwho_str_repeat('X',option);
		return(true);
	}

	return(false);
}

function xivo_get_exten_buffer(letter,value)
{
	if(dwho_substr(value,-1) === '.')
		return('*');

	var chr = '';

	if(letter.indexOf('N') > -1)
		chr += 'N';

	if(letter.indexOf('X') > -1)
		chr += 'X';

	if(letter.indexOf('Z') > -1)
		chr += 'Z';

	if(chr.length === 0)
		return(false);

	var regstr = new RegExp('['+chr+']*$','i');

	if((buffer = value.match(regstr)) === null)
		return(false);

	return(buffer[0].length);
}

function xivo_chk_exten_pattern(value)
{
	if(dwho_is_undef(value) === true || dwho_is_string(value) === false)
		return(false);

	var len = value.length;

	if(len === 0 || len > 40)
		return(false);

	if(value.charAt(0) === '_')
		value = dwho_substr(value,1);

	if(value.match(/^[0-9NXZ\*#\-\[\]]+[\.\!]?$/) === null)
		return(false);

	return(value);
}

function xivo_fm_select_add_exten(id,value)
{
	if((pattern = xivo_chk_exten_pattern(value)) === false)
		return(false);

	return(dwho.form.select_add_entry(id,pattern,pattern));
}
