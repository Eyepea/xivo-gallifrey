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

function xivo_fm_select_add_attrldap(id,value)
{
	if(xivo_chk_attrldap(value) === false)
		return(false);

	return(xivo_fm_select_add_entry(id,value,value));
}

function xivo_chk_attrldap(value)
{
	if(xivo_is_string(value) === false
	|| value.match(/^(?:[a-zA-Z0-9-]+|[0-9]+(?:\.[0-9]+)*)$/) === null)
		return(false);

	return(value);
}

function xivo_chg_additionaltype(type)
{
	var display = 'none';
	var disabled = true;

	if(type === 'custom')
	{
		display = 'block';
		disabled = false;
	}

	xivo_eid('fd-ldapfilter-additionaltext').style.display = display;
	xivo_eid('it-ldapfilter-additionaltext').disabled = disabled;
}

function xivo_ldapfilter_onload()
{
	if(xivo_eid('it-ldapfilter-additionaltype') !== false)
		xivo_chg_additionaltype(xivo_eid('it-ldapfilter-additionaltype').value);
}

xivo.dom.set_onload(xivo_ldapfilter_onload);
