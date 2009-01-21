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

var xivo_imode = 0;

var xivo_elt_mode = new Array();

xivo_elt_mode['fd-callfilter-ringseconds'] = new Array();
xivo_elt_mode['fd-callfilter-ringseconds']['style'] = {display: 'block'};
xivo_elt_mode['it-callfilter-ringseconds'] = new Array();
xivo_elt_mode['it-callfilter-ringseconds']['property'] = {disabled: false};

xivo_elt_mode['links'] = new Array();
xivo_elt_mode['links']['link'] = new Array();
xivo_elt_mode['links']['link'][xivo_imode++] = new Array('fd-callfilter-ringseconds',0,1);
xivo_elt_mode['links']['link'][xivo_imode++] = new Array('it-callfilter-ringseconds',0,1);


var xivo_fm_mode = new Array();

xivo_fm_mode['bosssecretary'] = new Array();
xivo_fm_mode['bosssecretary']['bossfirst-serial'] = xivo_clone(xivo_elt_mode);
xivo_fm_mode['bosssecretary']['bossfirst-simult'] = xivo_clone(xivo_elt_mode);
xivo_fm_mode['bosssecretary']['secretary-serial'] = xivo_clone(xivo_elt_mode);
xivo_fm_mode['bosssecretary']['secretary-simult'] = xivo_clone(xivo_elt_mode);
xivo_fm_mode['bosssecretary']['secretary-all'] = xivo_clone(xivo_elt_mode);

xivo_fm_mode['bosssecretary']['bossfirst-serial']['fd-callfilter-ringseconds']['style'] = {display: 'none'};
xivo_fm_mode['bosssecretary']['bossfirst-serial']['it-callfilter-ringseconds']['property'] = {disabled: false};

xivo_fm_mode['bosssecretary']['secretary-serial']['fd-callfilter-ringseconds']['style'] = {display: 'none'};
xivo_fm_mode['bosssecretary']['secretary-serial']['it-callfilter-ringseconds']['property'] = {disabled: false};

xivo_attrib_register('fm_mode-bosssecretary-bossfirst-serial',xivo_fm_mode['bosssecretary']['bossfirst-serial']);
xivo_attrib_register('fm_mode-bosssecretary-bossfirst-simult',xivo_fm_mode['bosssecretary']['bossfirst-simult']);
xivo_attrib_register('fm_mode-bosssecretary-secretary-serial',xivo_fm_mode['bosssecretary']['secretary-serial']);
xivo_attrib_register('fm_mode-bosssecretary-secretary-simult',xivo_fm_mode['bosssecretary']['secretary-simult']);
xivo_attrib_register('fm_mode-bosssecretary-secretary-all',xivo_fm_mode['bosssecretary']['secretary-all']);

function xivo_chgmode(modename,mode)
{
	if(xivo_is_undef(xivo_fm_mode[modename]) == true
	|| xivo_is_undef(xivo_fm_mode[modename][mode.value]) == true)
		return(false);

	xivo_chg_attrib('fm_mode-'+modename+'-'+mode.value,'links',0,1);
}

function xivo_callfilter_onload()
{
	if(xivo_eid('it-callfilter-bosssecretary') != false)
		xivo_chgmode('bosssecretary',xivo_eid('it-callfilter-bosssecretary'));

	xivo_ast_build_dialaction_array('noanswer');
	xivo_ast_dialaction_onload();
}

xivo_winload.push('xivo_callfilter_onload();');
