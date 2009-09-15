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

var xivo_ast_callfilter_mode_elt = {
	'fd-callfilter-ringseconds':	{style:		{display: 'block'}},
	'it-callfilter-ringseconds':	{property:	{disabled: false}},
	'links':
		{link:	[['fd-callfilter-ringseconds',0,1],
			 ['it-callfilter-ringseconds',0,1]]}};

var xivo_ast_fm_callfilter_mode = {
	'bosssecretary':
		{'bossfirst-serial':	xivo_clone(xivo_ast_callfilter_mode_elt),
		 'bossfirst-simult':	xivo_clone(xivo_ast_callfilter_mode_elt),
		 'secretary-serial':	xivo_clone(xivo_ast_callfilter_mode_elt),
		 'secretary-simult':	xivo_clone(xivo_ast_callfilter_mode_elt),
		 'secretary-all':	xivo_clone(xivo_ast_callfilter_mode_elt)}};

xivo_ast_fm_callfilter_mode['bosssecretary']['bossfirst-serial']['fd-callfilter-ringseconds']['style'] = {display: 'none'};
xivo_ast_fm_callfilter_mode['bosssecretary']['bossfirst-serial']['it-callfilter-ringseconds']['property'] = {disabled: false};

xivo_ast_fm_callfilter_mode['bosssecretary']['secretary-serial']['fd-callfilter-ringseconds']['style'] = {display: 'none'};
xivo_ast_fm_callfilter_mode['bosssecretary']['secretary-serial']['it-callfilter-ringseconds']['property'] = {disabled: false};

xivo_attrib_register('fm_callfilter_mode-bosssecretary-bossfirst-serial',
		     xivo_ast_fm_callfilter_mode['bosssecretary']['bossfirst-serial']);
xivo_attrib_register('fm_callfilter_mode-bosssecretary-bossfirst-simult',
		     xivo_ast_fm_callfilter_mode['bosssecretary']['bossfirst-simult']);
xivo_attrib_register('fm_callfilter_mode-bosssecretary-secretary-serial',
		     xivo_ast_fm_callfilter_mode['bosssecretary']['secretary-serial']);
xivo_attrib_register('fm_callfilter_mode-bosssecretary-secretary-simult',
		     xivo_ast_fm_callfilter_mode['bosssecretary']['secretary-simult']);
xivo_attrib_register('fm_callfilter_mode-bosssecretary-secretary-all',
		     xivo_ast_fm_callfilter_mode['bosssecretary']['secretary-all']);

function xivo_callfilter_chg_mode(modename,mode)
{
	if(xivo_is_object(mode) === false
	|| xivo_is_undef(mode.value) === true
	|| xivo_is_undef(xivo_ast_fm_callfilter_mode[modename]) === true
	|| xivo_is_undef(xivo_ast_fm_callfilter_mode[modename][mode.value]) === true)
		return(false);

	xivo_chg_attrib('fm_callfilter_mode-' + modename + '-' + mode.value,'links',0,1);
}

function xivo_callfilter_onload()
{
	xivo_callfilter_chg_mode('bosssecretary',xivo_eid('it-callfilter-bosssecretary'));
	xivo_ast_build_dialaction_array('noanswer');
	xivo_ast_dialaction_onload();
}

xivo.dom.set_onload(xivo_callfilter_onload);
