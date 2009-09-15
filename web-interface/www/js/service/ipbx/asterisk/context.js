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

function xivo_context_entity_enable_add(type,table)
{
	if(xivo_is_string(type) === false
	|| xivo_is_object(table) === false
	|| (entity = xivo_eid('it-context-entity')) === false
	|| entity.value === '')
		return(false);

	return(xivo.dom.make_table_list('contextnumbers-'+type,table));
}

function xivo_context_entity_status(form,disable)
{
	var arr = {
		'user':		['numberbeg','numberend'],
		'group':	['numberbeg','numberend'],
		'queue':	['numberbeg','numberend'],
		'meetme':	['numberbeg','numberend'],
		'incall':	['numberbeg','numberend','didlength']};

	for(var key in arr)
	{
		ref = arr[key];
		nb = ref.length;

		for(i = 0;i < nb;i++)
		{
			xivo_fm_enable_disable_field(form,
						     'contextnumbers['+key+']['+ref[i]+'][]',
						     disable,
						     'ex-contextnumbers-'+key,
						     'tbody');
		}
	}
}
