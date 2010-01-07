/*
 * XiVO Web-Interface
 * Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
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

var xivo_ast_trunks_elt = {'links': {'link': []}};
var xivo_ast_fm_trunks = {};

var xivo_ast_fm_trunk_host = {
	'fd-protocol-host-static':
		{style: [{display: 'none'}, {display: 'block'}],
		 link: 'it-protocol-host-static'},
	'it-protocol-host-static':
		{property: [{disabled: true}, {disabled: false}]}};

xivo_attrib_register('ast_fm_trunk_host',xivo_ast_fm_trunk_host);

var xivo_ast_fm_trunk_codec = {
	'it-protocol-disallow':
		{property: [{disabled: false, className: 'it-enabled'},
			    {disabled: true, className: 'it-disabled'}],
		 link: 'it-codeclist'},
	'it-codeclist':
		{property: [{disabled: false, className: 'it-enabled'},
			    {disabled: true, className: 'it-disabled', selectedIndex: -1}],
		 link: 'it-codec'},
	'it-codec':
		{property: [{disabled: false, className: 'it-enabled'},
			    {disabled: true, className: 'it-disabled', selectedIndex: -1}]}};

xivo_attrib_register('ast_fm_trunk_codec',xivo_ast_fm_trunk_codec);

xivo_ast_fm_trunk_register = {
	'it-register-username':
		{property: [{disabled: false, className: 'it-enabled'},
			    {disabled: true, className: 'it-disabled'}],
		 link: 'it-register-password'},
	'it-register-password':
		{property: [{disabled: false, className: 'it-enabled'},
			    {disabled: true, className: 'it-disabled'}],
		 link: 'it-register-authuser'},
	'it-register-authuser':
		{property: [{disabled: false, className: 'it-enabled'},
			    {disabled: true, className: 'it-disabled'}],
		 link: 'it-register-host'},
	'it-register-host':
		{property: [{disabled: false, className: 'it-enabled'},
			    {disabled: true, className: 'it-disabled'}],
		 link: 'it-register-port'},
	'it-register-port':
		{property: [{disabled: false, className: 'it-enabled'},
			    {disabled: true, className: 'it-disabled'}],
		 link: 'it-register-contact'},
	'it-register-contact':
		{property: [{disabled: false, className: 'it-enabled'},
			    {disabled: true, className: 'it-disabled', selectedIndex: -1}]}};

xivo_attrib_register('ast_fm_trunk_register',xivo_ast_fm_trunk_register);

function xivo_ast_get_trunk_type_elt_info(trunktype,element)
{
	if(dwho_type_object(xivo_ast_trunk_type_elt) === false
	|| dwho_type_object(xivo_ast_trunk_type_elt[trunktype]) === false
	|| dwho_is_undef(xivo_ast_trunk_type_elt[trunktype][trunktype+'-'+element]) === true)
		return(false);

	return(xivo_ast_trunk_type_elt[trunktype][trunktype+'-'+element]);
}

function xivo_ast_build_trunks_elt()
{
	if(dwho_type_object(xivo_ast_trunks_elt_default) === false
	|| dwho_type_object(xivo_ast_trunks_elt) === false)
		return(false);

	var it_true = {property: {disabled: false, className: 'it-enabled'}};
	var fd_true = {style: {display: 'block'}};

	var it_false = {property: {disabled: true, className: 'it-disabled'}};
	var fd_false = {style: {display: 'none'}};

	var i = 0;

	for(property in xivo_ast_trunks_elt_default)
	{
		fd_trunktype = fd_trunktype = changed = false;

		key = xivo_ast_trunks_elt_default[property];

		for(type_elt in xivo_ast_trunk_type_elt)
		{
			type_elt_info = xivo_ast_get_trunk_type_elt_info(type_elt,property);

			if(dwho_is_undef(key['it']) === false)
			{
				if(type_elt_info !== false
				&& dwho_is_undef(type_elt_info['it']) === false)
				{
					fd_trunktype = true;
					it_key = 'it-'+trunktype+'-'+property;

					if(key['it'] === true)
						xivo_ast_trunks_elt[it_key] = dwho_clone(it_true);
					else if(key['it'] === false)
						xivo_ast_trunks_elt[it_key] = dwho_clone(it_false);
					else
						xivo_ast_trunks_elt[it_key] = dwho_clone(key['it']);

					xivo_ast_trunks_elt['links']['link'][i++] = [it_key,0,1];
				}
			}

			if(dwho_is_undef(key['fd']) === false)
			{
				if(type_elt_info !== false
				&& dwho_is_undef(type_elt_info['fd']) === false)
				{
					fd_trunktype = true;
					fd_key = 'fd-'+trunktype+'-'+property;

					if(key['fd'] === true)
						xivo_ast_trunks_elt[fd_key] = dwho_clone(fd_true);
					else if(key['fd'] === false)
						xivo_ast_trunks_elt[fd_key] = dwho_clone(fd_false);
					else
						xivo_ast_trunks_elt[fd_key] = dwho_clone(key['fd']);

					xivo_ast_trunks_elt['links']['link'][i++] = [fd_key,0,1];
				}
			}
		}

		if(fd_trunktype === false && dwho_is_undef(key['it']) === false)
		{
			changed = true;

			it_key = 'it-'+property;

			if(key['it'] === true)
				xivo_ast_trunks_elt[it_key] = dwho_clone(it_true);
			else if(key['it'] === false)
				xivo_ast_trunks_elt[it_key] = dwho_clone(it_false);
			else
				xivo_ast_trunks_elt[it_key] = dwho_clone(key['it']);

			xivo_ast_trunks_elt['links']['link'][i++] = [it_key,0,1];
		}

		if(fd_trunktype === false && dwho_is_undef(key['fd']) === false)
		{
			changed = true;

			fd_key = 'fd-'+property;

			if(key['fd'] === true)
				xivo_ast_trunks_elt[fd_key] = dwho_clone(fd_true);
			else if(key['fd'] === false)
				xivo_ast_trunks_elt[fd_key] = dwho_clone(fd_false);
			else
				xivo_ast_trunks_elt[fd_key] = dwho_clone(key['fd']);

			xivo_ast_trunks_elt['links']['link'][i++] = [fd_key,0,1];
		}

		if(changed === false && dwho_type_object(key) === true)
		{
			xivo_ast_trunks_elt[property] = dwho_clone(key);
			xivo_ast_trunks_elt['links']['link'][i++] = [property,0,1];
		}
	}
}

function xivo_ast_build_trunk_type_array(trunktype)
{
	if(dwho_type_object(xivo_ast_trunks_elt_default) === false
	|| dwho_type_object(xivo_ast_trunks_elt) === false
	|| dwho_type_object(xivo_ast_trunk_type_elt[trunktype]) === false)
		return(false);

	var it_true = {property: {disabled: false, className: 'it-enabled'}};
	var fd_true = {style: {display: 'block'}};

	var it_false = {property: {disabled: true, className: 'it-disabled'}};
	var fd_false = {style: {display: 'none'}};

	xivo_ast_fm_trunks[trunktype] = dwho_clone(xivo_ast_trunks_elt);

	for(property in xivo_ast_trunk_type_elt[trunktype])
	{
		key = xivo_ast_trunk_type_elt[trunktype][property];

		if(dwho_is_undef(key['it']) === false)
		{
			it_key = 'it-'+property;

			if(key['it'] === true)
				xivo_ast_fm_trunks[trunktype][it_key] = dwho_clone(it_true);
			else if(key['it'] === false)
				xivo_ast_fm_trunks[trunktype][it_key] = dwho_clone(it_false);
			else
				xivo_ast_fm_trunks[trunktype][it_key] = dwho_clone(key['it']);
		}

		if(dwho_is_undef(key['fd']) === false)
		{
			fd_key = 'fd-'+property;

			if(key['fd'] === true)
				xivo_ast_fm_trunks[trunktype][fd_key] = dwho_clone(fd_true);
			else if(key['fd'] === false)
				xivo_ast_fm_trunks[trunktype][fd_key] = dwho_clone(fd_false);
			else
				xivo_ast_fm_trunks[trunktype][fd_key] = dwho_clone(key['fd']);
		}
	}

	xivo_attrib_register('ast_fm_trunk_type-'+trunktype,xivo_ast_fm_trunks[trunktype]);
}

function xivo_ast_chg_trunk_type(trunktype)
{
	if(dwho_is_undef(xivo_ast_trunk_type_elt[trunktype]) === true)
		return(false);

	if(dwho_is_undef(xivo_ast_fm_trunks[trunktype]) === true)
		xivo_ast_build_trunk_type_array(trunktype);

	xivo_chg_attrib('ast_fm_trunk_type-'+trunktype,'links',0,1);

	if(trunktype === 'user')
		return(true);

	if((host_type = dwho_eid('it-protocol-host-type')) !== false)
		xivo_chg_attrib('ast_fm_trunk_host',
				'fd-protocol-host-static',
				Number(host_type.value === 'static'));
}

function xivo_ast_trunk_onload()
{
	xivo_ast_build_trunks_elt();

	if(dwho_eid('it-protocol-type') !== false)
		xivo_ast_chg_trunk_type(dwho_eid('it-protocol-type').value);

	if((regactive = dwho_eid('it-register-active')) !== false)
		xivo_chg_attrib('ast_fm_trunk_register',
				'it-register-username',
				Number(regactive.checked === false));

	if((codec_active = dwho_eid('it-codec-active')) !== false)
		xivo_chg_attrib('ast_fm_trunk_codec',
				'it-protocol-disallow',
				Number(codec_active.checked === false));
}

dwho.dom.set_onload(xivo_ast_trunk_onload);
