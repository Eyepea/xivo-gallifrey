/*
 * XiVO Web-Interface
 * Copyright (C) 2006, 2007, 2008  Proformatique <technique@proformatique.com>
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

var xivo_elt_phonefunckey = new Array();

var xivo_fm_phonefunckey = new Array();

var xivo_phonefunckey_type = new Array();
xivo_phonefunckey_type['user'] = 1;
xivo_phonefunckey_type['group'] = 1;
xivo_phonefunckey_type['queue'] = 1;
xivo_phonefunckey_type['meetme'] = 1;
xivo_phonefunckey_type['extension'] = 1;
xivo_phonefunckey_type['bosssecretary'] = 1;
xivo_phonefunckey_type['custom'] = 1;

function xivo_build_phonefunckey_array(id)
{
	if(xivo_is_undef(xivo_fm_phonefunckey[id]) === false
	&& xivo_is_array(xivo_fm_phonefunckey[id]) === true)
		return(true);

	xivo_fm_phonefunckey[id] = new Array();

	xivo_elt_phonefunckey[id] = new Array();
	xivo_elt_phonefunckey[id]['links'] = new Array();
	xivo_elt_phonefunckey[id]['links']['link'] = new Array();

	var i = 0;

	for(property in xivo_phonefunckey_type)
	{
		key = 'fd-phonefunckey-'+property+'-typeval-'+id;
		xivo_elt_phonefunckey[id][key] = new Array();
		xivo_elt_phonefunckey[id][key]['style'] = {display: 'none'};
		xivo_elt_phonefunckey[id]['links']['link'][i++] = new Array(key,0,1);

		key = 'it-phonefunckey-supervision-'+id;
		xivo_elt_phonefunckey[id][key] = new Array();
		xivo_elt_phonefunckey[id][key]['property'] = {className: 'it-disabled'};
		xivo_elt_phonefunckey[id]['links']['link'][i++] = new Array(key,0,1);

		key = 'it-phonefunckey-'+property+'-typeval-'+id;
		xivo_elt_phonefunckey[id][key] = new Array();
		xivo_elt_phonefunckey[id][key]['style'] = {display: 'none'};
		xivo_elt_phonefunckey[id][key]['property'] = {disabled: true, className: 'it-disabled'};
		xivo_elt_phonefunckey[id]['links']['link'][i++] = new Array(key,0,1);
	}

	for(property in xivo_phonefunckey_type)
	{
		xivo_fm_phonefunckey[id][property] = xivo_clone(xivo_elt_phonefunckey[id]);
		xivo_fm_phonefunckey[id][property]['fd-phonefunckey-'+property+'-typeval-'+id]['style'] = {display: 'inline'};

		keyit = 'it-phonefunckey-'+property+'-typeval-'+id;
		xivo_fm_phonefunckey[id][property][keyit]['style'] = {display: 'inline'};
		xivo_fm_phonefunckey[id][property][keyit]['property'] = {disabled: false, className: 'it-enabled'};

		if(property === 'user'
		|| property === 'bosssecretary')
		{
			keyit = 'it-phonefunckey-supervision-'+id;
			xivo_fm_phonefunckey[id][property][keyit]['property'] = {className: 'it-enabled'};
		}

		xivo_attrib_register('fm_phonefunckey-'+id+'-'+property,xivo_fm_phonefunckey[id][property]);
	}
}

function xivo_chgphonefunckey(type)
{
	if(xivo_is_undef(type.value) === true
	|| (xivo_is_undef(type.disabled) === false && type.disabled === true) === true
	|| (rs = type.id.match(/-(\d+)$/)) === null)
		return(false);

	xivo_build_phonefunckey_array(rs[1]);

	xivo_chg_attrib('fm_phonefunckey-'+rs[1]+'-'+type.value,'links',0,1);
}

function xivo_phonefunckey_onload()
{
	if(xivo_is_undef(xivo_tlist['phonefunckey']) === true
	|| xivo_is_undef(xivo_tlist['phonefunckey']['cnt']) === true)
		return(false);

	for(var i = 0;i < xivo_tlist['phonefunckey']['cnt'];i++)
	{
		if((eid = xivo_eid('it-phonefunckey-type-'+i)) !== false)
			xivo_chgphonefunckey(eid);
	}
}

xivo_winload.push('xivo_phonefunckey_onload();');
