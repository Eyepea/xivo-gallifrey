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

var xivo_ast_outcall_mode_elt = {
	'fd-outcall-prefix':
		{style:		{display: 'none'}},
	'it-outcall-prefix':
		{property:	{disabled: true}},
	'fd-outcall-numlen':
		{style:		{display: 'none'}},
	'it-outcall-numlen':
		{property:	{disabled: true}},
	'fd-outcall-exten':
		{style:		{display: 'none'}},
	'links':
		{link:	[['fd-outcall-prefix',0,1],
			 ['it-outcall-prefix',0,1],
			 ['fd-outcall-numlen',0,1],
			 ['it-outcall-numlen',0,1],
			 ['fd-outcall-exten',0,1],
			 ['it-outcall-exten',0,1]]}};

var xivo_ast_fm_outcall_mode = {'wizard':	dwho_clone(xivo_ast_outcall_mode_elt)};
xivo_ast_fm_outcall_mode['wizard']['fd-outcall-prefix']['style'] = {display: 'block'};
xivo_ast_fm_outcall_mode['wizard']['it-outcall-prefix']['property'] = {disabled: false};
xivo_ast_fm_outcall_mode['wizard']['fd-outcall-numlen']['style'] = {display: 'block'};
xivo_ast_fm_outcall_mode['wizard']['it-outcall-numlen']['property'] = {disabled: false};

xivo_attrib_register('fm_outcall_mode-wizard',xivo_ast_fm_outcall_mode['wizard']);

xivo_ast_fm_outcall_mode['extension'] = dwho_clone(xivo_ast_outcall_mode_elt);
xivo_ast_fm_outcall_mode['extension']['fd-outcall-exten']['style'] = {display: 'block'};

xivo_attrib_register('fm_outcall_mode-extension',xivo_ast_fm_outcall_mode['extension']);

function xivo_outcall_chg_mode(mode)
{
	if(dwho_is_object(mode) === false
	|| dwho_is_undef(mode.value) === true
	|| dwho_is_undef(xivo_ast_fm_outcall_mode[mode.value]) === true)
		return(false);

	xivo_chg_attrib('fm_outcall_mode-'+mode.value,'links',0,1);
}

function xivo_outcall_wizard_exten()
{
	if((objpre = dwho_eid('it-outcall-prefix')) === false
	|| (objnum = dwho_eid('it-outcall-numlen')) === false
	|| (objres = dwho_eid('it-outcall-exten')) === false
	|| dwho_is_undef(objpre.value) === true
	|| dwho_is_undef(objnum.value) === true
	|| dwho_is_undef(objres.value) === true)
		return(false);

	if(objpre.value.match(/^\+?[0-9#\*]*$/) === null)
		objpre.value = '';
	else
		objres.value = objpre.value;

	if(objnum.value === '*' && objres.value.length === 0)
		return(false);

	if(objnum.value === '*')
	{
		objres.value += '.';
		return(true);
	}

	option = Number(objnum.value);

	if(option > 0 && option < 40)
	{
		objres.value += dwho_str_repeat('X',option);
		return(true);
	}

	return(false);
}

function xivo_outcall_exten_wizard()
{
	if((objpre = dwho_eid('it-outcall-prefix')) === false
	|| (objnum = dwho_eid('it-outcall-numlen')) === false
	|| (objres = dwho_eid('it-outcall-exten')) === false
	|| dwho_is_undef(objpre.value) === true
	|| dwho_is_undef(objnum.value) === true
	|| dwho_is_undef(objres.value) === true)
		return(false);

	objres.value = dwho_substr(objres.value,0,40);

	if((match = objres.value.match(/^\+?[0-9\*]*/)) === null)
		objpre.value = '';
	else
		objpre.value = match[0];

	if((objnum.value = xivo_get_exten_buffer('X',objres.value)) === false)
		objnum.value = '';

	return(true);
}

function xivo_outcall_onload()
{
	xivo_outcall_exten_wizard();
	xivo_outcall_chg_mode(dwho_eid('it-outcall-mode'));

	dwho.dom.add_event('change',
			   dwho_eid('it-outcall-mode'),
			   function()
			   {
				xivo_outcall_chg_mode(this);

				if(this.value === 'wizard')
					xivo_outcall_exten_wizard();
				else
					xivo_outcall_wizard_exten();
			   });

	dwho.dom.add_event('change',
			   dwho_eid('it-outcall-prefix'),
			   xivo_outcall_wizard_exten);

	dwho.dom.add_event('focus',
			   dwho_eid('it-outcall-prefix'),
			   xivo_outcall_wizard_exten);

	dwho.dom.add_event('blur',
			   dwho_eid('it-outcall-prefix'),
			   xivo_outcall_wizard_exten);

	dwho.dom.add_event('change',
			   dwho_eid('it-outcall-numlen'),
			   xivo_outcall_wizard_exten);

	dwho.dom.add_event('focus',
			   dwho_eid('it-outcall-numlen'),
			   xivo_outcall_wizard_exten);

	dwho.dom.add_event('blur',
			   dwho_eid('it-outcall-numlen'),
			   xivo_outcall_wizard_exten);

	dwho.dom.add_event('change',
			   dwho_eid('it-outcall-exten'),
			   xivo_outcall_exten_wizard);

	dwho.dom.add_event('focus',
			   dwho_eid('it-outcall-exten'),
			   xivo_outcall_exten_wizard);

	dwho.dom.add_event('blur',
			   dwho_eid('it-outcall-exten'),
			   xivo_outcall_exten_wizard);
}

dwho.dom.set_onload(xivo_outcall_onload);
