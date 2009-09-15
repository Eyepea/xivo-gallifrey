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

var xivo_elt_dialaction = {
			'answer':	{},
			'noanswer':	{},
			'congestion':	{},
			'busy':		{},
			'chanunavail':	{},
			'inschedule':	{},
			'outschedule':	{}};

var xivo_fm_dialaction = xivo_clone(xivo_elt_dialaction);

var xivo_dialaction_actiontype = {
			'none':		['actiontype'],
			'endcall':	['action','actiontype','busy-actionarg1','congestion-actionarg1'],
			'user':		['actiontype','actionarg1','actionarg2'],
			'group':	['actiontype','actionarg1','actionarg2'],
			'queue':	['actiontype','actionarg1','actionarg2'],
			'meetme':	['actiontype','actionarg1'],
			'voicemail':	['actiontype',
					 'actionarg1',
					 'actionarg2-b',
					 'actionarg2-s',
					 'actionarg2-u',
					 'actionarg2-j'],
			'schedule':	['actiontype','actionarg1'],
			'voicemenu':	['actiontype','actionarg1'],
			'extension':	['actiontype','actionarg1','actionarg2'],
			'application':	['action',
					 'actiontype',
					 'callbackdisa-actionarg1',
					 'callbackdisa-actionarg2',
					 'disa-actionarg1',
					 'disa-actionarg2',
					 'directory-actionarg1',
					 'faxtomail-actionarg1',
					 'voicemailmain-actionarg1'],
			'custom':	['actiontype','actionarg1'],
			'sound':	['actiontype',
					 'actionarg1',
					 'actionarg2-skip',
					 'actionarg2-noanswer',
					 'actionarg2-j']};

var xivo_dialaction_actionarg = {};
xivo_dialaction_actionarg['endcall'] = {};
xivo_dialaction_actionarg['endcall']['hangup'] = [{display: false, name: 'busy-actionarg1'},
						  {display: false, name: 'congestion-actionarg1'}];
xivo_dialaction_actionarg['endcall']['busy'] = [{display: true, name: 'busy-actionarg1'},
						{display: false, name: 'congestion-actionarg1'}];
xivo_dialaction_actionarg['endcall']['congestion'] = [{display: false, name: 'busy-actionarg1'},
						      {display: true, name: 'congestion-actionarg1'}];
xivo_dialaction_actionarg['application'] = {};
xivo_dialaction_actionarg['application']['callbackdisa'] = [{display: true, name: 'callbackdisa-actionarg1'},
							    {display: true, name: 'callbackdisa-actionarg2'},
							    {display: false, name: 'disa-actionarg1'},
							    {display: false, name: 'disa-actionarg2'},
							    {display: false, name: 'directory-actionarg1'},
							    {display: false, name: 'faxtomail-actionarg1'},
							    {display: false, name: 'voicemailmain-actionarg1'}];
xivo_dialaction_actionarg['application']['disa'] = [{display: false, name: 'callbackdisa-actionarg1'},
						    {display: false, name: 'callbackdisa-actionarg2'},
						    {display: true, name: 'disa-actionarg1'},
						    {display: true, name: 'disa-actionarg2'},
						    {display: false, name: 'directory-actionarg1'},
						    {display: false, name: 'faxtomail-actionarg1'},
						    {display: false, name: 'voicemailmain-actionarg1'}];
xivo_dialaction_actionarg['application']['directory'] = [{display: false, name: 'callbackdisa-actionarg1'},
							 {display: false, name: 'callbackdisa-actionarg2'},
							 {display: false, name: 'disa-actionarg1'},
							 {display: false, name: 'disa-actionarg2'},
							 {display: true, name: 'directory-actionarg1'},
							 {display: false, name: 'faxtomail-actionarg1'},
							 {display: false, name: 'voicemailmain-actionarg1'}];
xivo_dialaction_actionarg['application']['faxtomail'] = [{display: false, name: 'callbackdisa-actionarg1'},
							 {display: false, name: 'callbackdisa-actionarg2'},
							 {display: false, name: 'disa-actionarg1'},
							 {display: false, name: 'disa-actionarg2'},
							 {display: false, name: 'directory-actionarg1'},
							 {display: true, name: 'faxtomail-actionarg1'},
							 {display: false, name: 'voicemailmain-actionarg1'}];
xivo_dialaction_actionarg['application']['voicemailmain'] = [{display: false, name: 'callbackdisa-actionarg1'},
							     {display: false, name: 'callbackdisa-actionarg2'},
							     {display: false, name: 'disa-actionarg1'},
							     {display: false, name: 'disa-actionarg2'},
							     {display: false, name: 'directory-actionarg1'},
							     {display: false, name: 'faxtomail-actionarg1'},
							     {display: true, name: 'voicemailmain-actionarg1'}];

function xivo_ast_build_dialaction_array(dialevent)
{
	if(xivo_is_undef(xivo_elt_dialaction[dialevent]) === true
	|| xivo_type_object(xivo_elt_dialaction[dialevent]) === false
	|| xivo_is_undef(xivo_fm_dialaction[dialevent]) === true)
		return(false);

	xivo_elt_dialaction[dialevent]['links'] = {'link': []};

	var i = 0;

	for(var property in xivo_dialaction_actiontype)
	{
		var ref = xivo_dialaction_actiontype[property];

		if(xivo_is_array(ref) === false || (nb = ref.length) === 0)
			continue;

		for(var j = 0;j < nb;j++)
		{
			var key = 'fd-dialaction-'+dialevent+'-'+property+'-'+ref[j];
			xivo_elt_dialaction[dialevent][key] = {'style': {display: 'none'}};
			xivo_elt_dialaction[dialevent]['links']['link'][i++] = [key,0,1];

			var key = 'it-dialaction-'+dialevent+'-'+property+'-'+ref[j];
			xivo_elt_dialaction[dialevent][key] = {'property': {disabled: true, className: 'it-disabled'}};
			xivo_elt_dialaction[dialevent]['links']['link'][i++] = [key,0,1];
		}
	}
}

function xivo_ast_register_dialaction(dialevent,actiontype)
{
	if(xivo_is_undef(xivo_dialaction_actiontype[actiontype]) === true)
		return(false);
	else if(xivo_type_object(xivo_fm_dialaction[dialevent][actiontype]) === true)
		return(true);

	var ref = xivo_dialaction_actiontype[actiontype];

	xivo_fm_dialaction[dialevent][actiontype] = xivo_clone(xivo_elt_dialaction[dialevent]);

	if(xivo_is_array(ref) === true && (nb = ref.length) > 0)
	{
		for(var i = 0;i < nb;i++)
		{
			var keyfd = 'fd-dialaction-'+dialevent+'-'+actiontype+'-'+ref[i];
			var keyit = 'it-dialaction-'+dialevent+'-'+actiontype+'-'+ref[i];

			xivo_fm_dialaction[dialevent][actiontype][keyfd]['style'] = {display: 'block'};
			xivo_fm_dialaction[dialevent][actiontype][keyit]['property'] = {disabled: false, className: 'it-enabled'};
		}
	}

	xivo_attrib_register('fm_dialaction-'+dialevent+'-'+actiontype,xivo_fm_dialaction[dialevent][actiontype]);

	return(true);
}

function xivo_ast_chg_dialaction(dialevent,actiontype)
{
	if(xivo_type_object(xivo_elt_dialaction[dialevent]) === false
	|| xivo_is_undef(actiontype.value) === true
	|| (xivo_is_undef(actiontype.disabled) === false && actiontype.disabled === true) === true
	|| xivo_ast_register_dialaction(dialevent,actiontype.value) === false)
		return(false);

	xivo_chg_attrib('fm_dialaction-'+dialevent+'-'+actiontype.value,'links',0,1);
	xivo_ast_chg_dialaction_actionarg(dialevent,actiontype.value);
}

function xivo_ast_chg_dialaction_actionarg(dialevent,actiontype)
{
	if(xivo_type_object(xivo_fm_dialaction[dialevent]) === false
	|| xivo_type_object(xivo_dialaction_actionarg[actiontype]) === false
	|| (action = xivo_eid('it-dialaction-'+dialevent+'-'+actiontype+'-action')) === false
	|| xivo_is_undef(action.value) === true
	|| xivo_is_array(xivo_dialaction_actionarg[actiontype][action.value]) === false
	|| (nb = xivo_dialaction_actionarg[actiontype][action.value].length) === 0)
		return(false);

	ref = xivo_dialaction_actionarg[actiontype][action.value];

	for(var i = 0;i < nb;i++)
	{
		if((fdactionarg = xivo_eid('fd-dialaction-'+dialevent+'-'+actiontype+'-'+ref[i].name)) !== false)
			fdactionarg.style.display = (ref[i].display === false ? 'none' : 'block');

		if((itactionarg = xivo_eid('it-dialaction-'+dialevent+'-'+actiontype+'-'+ref[i].name)) !== false)
		{
			if(ref[i].display === false)
				xivo_chg_property_attrib(itactionarg,{disabled: true, className: 'it-disabled'});
			else
				xivo_chg_property_attrib(itactionarg,{disabled: false, className: 'it-enabled'});
		}
	}
}

function xivo_ast_dialaction_onload()
{
	for(var dialevent in xivo_fm_dialaction)
	{
		if((action = xivo_eid('it-dialaction-'+dialevent+'-actiontype')) !== false)
			xivo_ast_chg_dialaction(dialevent,action);
	}
}
