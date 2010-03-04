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

function xivo_ast_defapplication_endcall(dialevent,targetid)
{
	var optargs = valargs = '';

	if((endcall = dwho_eid('it-dialaction-'+dialevent+'-endcall-action')) === false
	|| (endcall.value !== 'hangup'
	   && endcall.value !== 'busy'
	   && endcall.value !== 'congestion') === true)
		return(false);
	else if((endcallarg1 = dwho_eid('it-dialaction-'+dialevent+'-endcall-'+endcall.value+'-actionarg1')) !== false
	&& endcall.value !== 'hangup'
	&& endcallarg1.value.length > 0)
	{
		if(dwho_is_ufloat(endcallarg1.value) === false)
			return(false);

		optargs = valargs = [endcallarg1.value];
	}

	return(xivo_ast_set_defapplication('macro|endcall|'+endcall.value,targetid,optargs,valargs));
}

function xivo_ast_defapplication_user(dialevent,targetid)
{
	if((userarg1 = dwho_eid('it-dialaction-'+dialevent+'-user-actionarg1')) === false
	|| userarg1.type !== 'select-one'
	|| dwho_is_undef(userarg1.options[userarg1.selectedIndex]) === true
	|| (userarg2 = dwho_eid('it-dialaction-'+dialevent+'-user-actionarg2')) === false)
		return(false);

	var optargs = ['\''+userarg1.options[userarg1.selectedIndex].text+'\''];
	var valargs = [userarg1.value];

	if(userarg2.value.length > 0)
	{
		if(dwho_is_ufloat(userarg2.value) === false)
			return(false);

		optargs.push(userarg2.value);
		valargs.push(userarg2.value);
	}

	return(xivo_ast_set_defapplication('macro|user',targetid,optargs,valargs));
}

function xivo_ast_defapplication_get_user_identity(dialevent,id)
{
	return(dwho.form.get_text_opt_select('it-dialaction-'+dialevent+'-user-actionarg1',id,true));
}

function xivo_ast_defapplication_group(dialevent,targetid)
{
	if((grouparg1 = dwho_eid('it-dialaction-'+dialevent+'-group-actionarg1')) === false
	|| grouparg1.type !== 'select-one'
	|| dwho_is_undef(grouparg1.options[grouparg1.selectedIndex]) === true
	|| (grouparg2 = dwho_eid('it-dialaction-'+dialevent+'-group-actionarg2')) === false)
		return(false);

	var optargs = ['\''+grouparg1.options[grouparg1.selectedIndex].text+'\''];
	var valargs = [grouparg1.value];

	if(grouparg2.value.length > 0)
	{
		if(dwho_is_ufloat(grouparg2.value) === false)
			return(false);

		optargs.push(grouparg2.value);
		valargs.push(grouparg2.value);
	}

	return(xivo_ast_set_defapplication('macro|group',targetid,optargs,valargs));
}

function xivo_ast_defapplication_get_group_identity(dialevent,id)
{
	return(dwho.form.get_text_opt_select('it-dialaction-'+dialevent+'-group-actionarg1',id,true));
}

function xivo_ast_defapplication_queue(dialevent,targetid)
{
	if((queuearg1 = dwho_eid('it-dialaction-'+dialevent+'-queue-actionarg1')) === false
	|| queuearg1.type !== 'select-one'
	|| dwho_is_undef(queuearg1.options[queuearg1.selectedIndex]) === true
	|| (queuearg2 = dwho_eid('it-dialaction-'+dialevent+'-queue-actionarg2')) === false)
		return(false);

	var optargs = ['\''+queuearg1.options[queuearg1.selectedIndex].text+'\''];
	var valargs = [queuearg1.value];

	if(queuearg2.value.length > 0)
	{
		if(dwho_is_ufloat(queuearg2.value) === false)
			return(false);

		optargs.push(queuearg2.value);
		valargs.push(queuearg2.value);
	}

	return(xivo_ast_set_defapplication('macro|queue',targetid,optargs,valargs));
}

function xivo_ast_defapplication_get_queue_identity(dialevent,id)
{
	return(dwho.form.get_text_opt_select('it-dialaction-'+dialevent+'-queue-actionarg1',id,true));
}

function xivo_ast_defapplication_queueskill(dialevent,targetid)
{
	if((value = dwho_eid('it-dialaction-'+dialevent+'-queueskill-skill')) === false
	|| (name = dwho_eid('it-dialaction-'+dialevent+'-queueskill-varname')) === false)
		return(false);

	var namevalue  = xivo_ast_application_sanitize_arg(name.value).replace(/=/g,'');
	var valuevalue = xivo_ast_application_sanitize_arg(value.value).replace(/=/g,'');

	if(namevalue.length < 1 || valuevalue.length < 1 )
		return(false);

	var args = [namevalue+'='+valuevalue];
	return(xivo_fm_select_add_ast_application('set',args));
}

function xivo_ast_defapplication_queueskill_onskillchange(dialevent,targetid)
{
	if((skill = dwho_eid('it-dialaction-'+dialevent+'-queueskill-skill')) === false
	|| (varname = dwho_eid('it-dialaction-'+dialevent+'-queueskill-varname')) === false)
		return(false);

	if(varname.value.length > 0 && varname.getAttribute('autoset') != 1)
		return true;

	varname.value = skill.options[skill.selectedIndex].parentNode.label;
	varname.setAttribute('autoset', 1);
}

function xivo_ast_defapplication_queueskillrule(dialevent,targetid)
{
	if((rule  = dwho_eid('it-dialaction-'+dialevent+'-queueskillrule-name')) === false)
		return(false);

	var args = ['XIVO_QUEUESKILLRULESET'+'='+rule.value];
	return(xivo_fm_select_add_ast_application('set',args));
}

function xivo_ast_defapplication_meetme(dialevent,targetid)
{
	if((meetme = dwho_eid('it-dialaction-'+dialevent+'-meetme-actionarg1')) === false
	|| meetme.type !== 'select-one'
	|| dwho_is_undef(meetme.options[meetme.selectedIndex]) === true)
		return(false);

	var optargs = ['\''+meetme.options[meetme.selectedIndex].text+'\''];
	var valargs = [meetme.value];

	return(xivo_ast_set_defapplication('macro|meetme',targetid,optargs,valargs));
}

function xivo_ast_defapplication_get_meetme_identity(dialevent,id)
{
	return(dwho.form.get_text_opt_select('it-dialaction-'+dialevent+'-meetme-actionarg1',id,true));
}

function xivo_ast_defapplication_voicemail(dialevent,targetid)
{
	if((voicemail = dwho_eid('it-dialaction-'+dialevent+'-voicemail-actionarg1')) === false
	|| voicemail.type !== 'select-one'
	|| dwho_is_undef(voicemail.options[voicemail.selectedIndex]) === true
	|| (option_b = dwho_eid('it-dialaction-'+dialevent+'-voicemail-actionarg2-b')) === false
	|| (option_s = dwho_eid('it-dialaction-'+dialevent+'-voicemail-actionarg2-s')) === false
	|| (option_u = dwho_eid('it-dialaction-'+dialevent+'-voicemail-actionarg2-u')) === false
	|| (option_j = dwho_eid('it-dialaction-'+dialevent+'-voicemail-actionarg2-j')) === false)
		return(false);

	var optargs = ['\''+voicemail.options[voicemail.selectedIndex].text+'\''];
	var valargs = [voicemail.value];

	var options = '';

	if(option_b.checked === true)
		options += 'b';

	if(option_s.checked === true)
		options += 's';

	if(option_u.checked === true)
		options += 'u';

	if(option_j.checked === true)
		options += 'j';

	if(options.length > 0)
	{
		optargs.push(options);
		valargs.push(options);
	}

	return(xivo_ast_set_defapplication('macro|voicemail',targetid,optargs,valargs));
}

function xivo_ast_defapplication_get_voicemail_identity(dialevent,id)
{
	return(dwho.form.get_text_opt_select('it-dialaction-'+dialevent+'-voicemail-actionarg1',id,true));
}

function xivo_ast_defapplication_schedule(dialevent,targetid)
{
	if((schedule = dwho_eid('it-dialaction-'+dialevent+'-schedule-actionarg1')) === false
	|| schedule.type !== 'select-one'
	|| dwho_is_undef(schedule.options[schedule.selectedIndex]) === true)
		return(false);

	var optargs = ['\''+schedule.options[schedule.selectedIndex].text+'\''];
	var valargs = [schedule.value];

	return(xivo_ast_set_defapplication('macro|schedule',targetid,optargs,valargs));
}

function xivo_ast_defapplication_get_schedule_identity(dialevent,id)
{
	return(dwho.form.get_text_opt_select('it-dialaction-'+dialevent+'-schedule-actionarg1',id,true));
}

function xivo_ast_defapplication_voicemenu(dialevent,targetid)
{
	if((voicemenu = dwho_eid('it-dialaction-'+dialevent+'-voicemenu-actionarg1')) === false
	|| voicemenu.type !== 'select-one'
	|| dwho_is_undef(voicemenu.options[voicemenu.selectedIndex]) === true)
		return(false);

	var optargs = ['\''+voicemenu.options[voicemenu.selectedIndex].text+'\''];
	var valargs = [voicemenu.value];

	return(xivo_ast_set_defapplication('macro|voicemenu',targetid,optargs,valargs));
}

function xivo_ast_defapplication_get_voicemenu_identity(dialevent,id)
{
	return(dwho.form.get_text_opt_select('it-dialaction-'+dialevent+'-voicemenu-actionarg1',id,true));
}

function xivo_ast_defapplication_extension(dialevent,targetid)
{
	if((actionarg1 = dwho_eid('it-dialaction-'+dialevent+'-extension-actionarg1')) === false
	|| (actionarg2 = dwho_eid('it-dialaction-'+dialevent+'-extension-actionarg2')) === false)
		return(false);

	var actionarg1value = xivo_ast_defapplication_sanitize_arg(actionarg1.value);
	var actionarg2value = xivo_ast_defapplication_sanitize_arg(actionarg2.value);

	if(actionarg1value.length < 1
	|| actionarg2value.length < 1)
		return(false);

	var optargs = valargs = [actionarg1value,actionarg2value];

	return(xivo_ast_set_defapplication('macro|extension',targetid,optargs,valargs));
}

function xivo_ast_defapplication_application(dialevent,targetid)
{
	if((application = dwho_eid('it-dialaction-'+dialevent+'-application-action')) === false)
		return(false);

	var actionarg1_id = 'it-dialaction-'+dialevent+'-application-'+application.value+'-actionarg1';
	var actionarg2_id = 'it-dialaction-'+dialevent+'-application-'+application.value+'-actionarg2';

	switch(application.value)
	{
		case 'callbackdisa':
		case 'disa':
			if((applicationarg1 = dwho_eid(actionarg1_id)) === false
			|| (applicationarg2 = dwho_eid(actionarg2_id)) === false)
				return(false);

			var applicationarg1value = xivo_ast_defapplication_sanitize_arg(applicationarg1.value);
			var applicationarg2value = xivo_ast_defapplication_sanitize_arg(applicationarg2.value);

			if(applicationarg2value.length < 1)
				return(false);
			else if(applicationarg1value.length < 1)
				applicationarg1value = 'no-password';

			var optargs = valargs = [applicationarg1value,applicationarg2value];
			break;
		case 'directory':
		case 'faxtomail':
		case 'voicemailmain':
			if((applicationarg1 = dwho_eid(actionarg1_id)) === false)
				return(false);

			var applicationarg1value = xivo_ast_defapplication_sanitize_arg(applicationarg1.value);

			if(applicationarg1value.length < 1)
				return(false);

			var optargs = valargs = [applicationarg1value];
			break;
		default:
			return(false);
	}

	return(xivo_ast_set_defapplication('macro|'+application.value,targetid,optargs,valargs));
}

function xivo_ast_defapplication_sound(dialevent,targetid)
{
	if((filename = dwho_eid('it-dialaction-'+dialevent+'-sound-actionarg1')) === false
	|| (option_skip = dwho_eid('it-dialaction-'+dialevent+'-sound-actionarg2-skip')) === false
	|| (option_noanswer = dwho_eid('it-dialaction-'+dialevent+'-sound-actionarg2-noanswer')) === false
	|| (option_j = dwho_eid('it-dialaction-'+dialevent+'-sound-actionarg2-j')) === false)
		return(false);

	var valfilenamevalue = xivo_ast_application_sanitize_arg(filename.value);

	if((spos = valfilenamevalue.lastIndexOf('/')) > -1)
		var optfilenamevalue = valfilenamevalue.substring(spos+1);
	else
		var optfilenamevalue = valfilenamevalue;

	if(optfilenamevalue.length < 1)
		return(false);

	var optargs = [optfilenamevalue];
	var valargs = [valfilenamevalue];

	var options = '';

	if(option_skip.checked === true)
		options += 'skip';

	if(option_noanswer.checked === true)
		options += 'noanswer';

	if(option_j.checked === true)
		options += 'j';

	if(options.length > 0)
	{
		optargs.push(options);
		valargs.push(options);
	}

	return(xivo_ast_set_defapplication('macro|playsound',targetid,optargs,valargs));
}

function xivo_ast_set_defapplication(app,targetid,optargs,valargs)
{
	if((appdisplayname = xivo_ast_get_defapplication_displayname(app)) === false)
		return(false);
	else if(dwho_is_array(optargs) === true)
		var optionargs = optargs.join(',').replace(/\|/g,',');
	else
		var optionargs = '';

	if(dwho_is_array(valargs) === true)
		var valueargs = valargs.join('|');
	else
		var valueargs = '';

	var option = appdisplayname+'('+optionargs+')';
	var value = app+','+valueargs;

	if((target = dwho_eid(targetid)) === false)
		return(false);
	else if(target.type === 'select-multiple' || target.type === 'select-one')
		return(dwho.form.select_add_entry(targetid,option,value,true));
	else if(target.type === 'text')
		target.value = value;
	else if(dwho_is_undef(target.type) === true)
	{
		target.innerHTML = option;

		if((ittarget = dwho_eid('it-'+targetid)) !== false
		&& ittarget.tagName.toLowerCase() === 'input')
			ittarget.value = value;

		return(true);
	}

	return(false);
}

function xivo_ast_defapplication_sanitize_arg(str)
{
	return(str.replace(/[\|,]/g,''));
}

function xivo_ast_get_defapplication_displayname(app)
{
	if(dwho_is_object(xivo_ast_defapplication[app]) === true
	&& dwho_is_undef(xivo_ast_defapplication[app].displayname) === false)
		return(xivo_ast_defapplication[app].displayname);

	return(false);
}

function xivo_ast_get_defapplication_identityfunc(app)
{
	if(dwho_is_object(xivo_ast_defapplication[app]) === true
	&& dwho_is_undef(xivo_ast_defapplication[app].identityfunc) === false)
		return(xivo_ast_defapplication[app].identityfunc);

	return(false);
}

var xivo_ast_defapplication = {
	'macro|endcall|hangup':		{displayname: 'HangUp'},
	'macro|endcall|busy':		{displayname: 'Busy'},
	'macro|endcall|congestion':	{displayname: 'Congestion'},
	'macro|user':			{displayname: 'CallUser',
					 identityfunc: xivo_ast_defapplication_get_user_identity},
	'macro|group':			{displayname: 'CallGroup',
					 identityfunc: xivo_ast_defapplication_get_group_identity},
	'macro|queue':			{displayname: 'CallQueue',
					 identityfunc: xivo_ast_defapplication_get_queue_identity},
	'macro|meetme':			{displayname: 'CallMeetMe',
					 identityfunc: xivo_ast_defapplication_get_meetme_identity},
	'macro|voicemail':		{displayname: 'VoiceMail',
					 identityfunc: xivo_ast_defapplication_get_voicemail_identity},
	'macro|schedule':		{displayname: 'GotoSchedule',
					 identityfunc: xivo_ast_defapplication_get_schedule_identity},
	'macro|voicemenu':		{displayname: 'GotoVoiceMenu',
					 identityfunc: xivo_ast_defapplication_get_voicemenu_identity},
	'macro|extension':		{displayname: 'CallExten'},
	'macro|callbackdisa':		{displayname: 'CallBackDISA'},
	'macro|disa':			{displayname: 'DISA'},
	'macro|directory':		{displayname: 'Directory'},
	'macro|faxtomail':		{displayname: 'FaxToMail'},
	'macro|voicemailmain':		{displayname: 'VoiceMailMain'},
	'macro|playsound':		{displayname: 'PlaySound'}};
