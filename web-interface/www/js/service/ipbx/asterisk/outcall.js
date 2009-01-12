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

var xivo_imode = 0;

var xivo_elt_mode = new Array();

xivo_elt_mode['fd-outcall-prefix'] = new Array();
xivo_elt_mode['fd-outcall-prefix']['style'] = {display: 'none'};
xivo_elt_mode['it-outcall-prefix'] = new Array();
xivo_elt_mode['it-outcall-prefix']['property'] = {disabled: true};
xivo_elt_mode['fd-outcall-numlen'] = new Array();
xivo_elt_mode['fd-outcall-numlen']['style'] = {display: 'none'};
xivo_elt_mode['it-outcall-numlen'] = new Array();
xivo_elt_mode['it-outcall-numlen']['property'] = {disabled: true};
xivo_elt_mode['fd-outcall-exten'] = new Array();
xivo_elt_mode['fd-outcall-exten']['style'] = {display: 'none'};

xivo_elt_mode['links'] = new Array();
xivo_elt_mode['links']['link'] = new Array();
xivo_elt_mode['links']['link'][xivo_imode++] = new Array('fd-outcall-prefix',0,1);
xivo_elt_mode['links']['link'][xivo_imode++] = new Array('it-outcall-prefix',0,1);
xivo_elt_mode['links']['link'][xivo_imode++] = new Array('fd-outcall-numlen',0,1);
xivo_elt_mode['links']['link'][xivo_imode++] = new Array('it-outcall-numlen',0,1);
xivo_elt_mode['links']['link'][xivo_imode++] = new Array('fd-outcall-exten',0,1);
xivo_elt_mode['links']['link'][xivo_imode++] = new Array('it-outcall-exten',0,1);

var xivo_fm_mode = new Array();

xivo_fm_mode['wizard'] = xivo_clone(xivo_elt_mode);
xivo_fm_mode['wizard']['fd-outcall-prefix']['style'] = {display: 'block'};
xivo_fm_mode['wizard']['it-outcall-prefix']['property'] = {disabled: false};
xivo_fm_mode['wizard']['fd-outcall-numlen']['style'] = {display: 'block'};
xivo_fm_mode['wizard']['it-outcall-numlen']['property'] = {disabled: false};

xivo_attrib_register('fm_mode-wizard',xivo_fm_mode['wizard']);

xivo_fm_mode['extension'] = xivo_clone(xivo_elt_mode);
xivo_fm_mode['extension']['fd-outcall-exten']['style'] = {display: 'block'};

xivo_attrib_register('fm_mode-extension',xivo_fm_mode['extension']);

function xivo_chgmode(mode)
{
	if(xivo_is_undef(xivo_fm_mode[mode.value]) == true)
		return(false);

	xivo_chg_attrib('fm_mode-'+mode.value,'links',0,1);
}

function xivo_wizard_exten(prefix,numlen,result)
{
	if((objpre = xivo_eid(prefix)) == false
	|| (objnum = xivo_eid(numlen)) == false
	|| (objres = xivo_eid(result)) == false
	|| xivo_is_undef(objpre.value) == true
	|| xivo_is_undef(objnum.value) == true
	|| xivo_is_undef(objres.value) == true)
		return(false);

	if(objpre.value.match(/^\+?[0-9#\*]*$/) == null)
		objpre.value = '';
	else
		objres.value = objpre.value;

	if(objnum.value == '*' && objres.value.length == 0)
		return(false);

	if(objnum.value == '*')
	{
		objres.value += '.';
		return(true);
	}

	option = Number(objnum.value);

	if(option > 0 && option < 40)
	{
		objres.value += xivo_str_repeat('X',option);
		return(true);
	}

	return(false);
}

function xivo_exten_wizard(prefix,numlen,result)
{
	if((objpre = xivo_eid(prefix)) == false
	|| (objnum = xivo_eid(numlen)) == false
	|| (objres = xivo_eid(result)) == false
	|| xivo_is_undef(objpre.value) == true
	|| xivo_is_undef(objnum.value) == true
	|| xivo_is_undef(objres.value) == true)
		return(false);

	objres.value = xivo_substr(objres.value,0,40);

	if((match = objres.value.match(/^\+?[0-9\*]*/)) == null)
		objpre.value = '';
	else
		objpre.value = match[0];

	if((objnum.value = xivo_get_exten_buffer('X',objres.value)) == false)
		objnum.value = '';

	return(true);
}

function xivo_outcall_onload()
{
	xivo_exten_wizard('it-outcall-prefix','it-outcall-numlen','it-outcall-exten');

	if(xivo_eid('it-outcall-mode') != false)
		xivo_chgmode(xivo_eid('it-outcall-mode'));
}

xivo_winload.push('xivo_outcall_onload();');
