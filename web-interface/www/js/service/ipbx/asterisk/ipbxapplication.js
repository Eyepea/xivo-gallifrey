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

function xivo_ast_application_absolutetimeout()
{
	if((timeout = dwho_eid('it-ipbxapplication-absolutetimeout-timeout')) === false
	|| dwho_is_ufloat(timeout.value) === false)
		return(false);

	var args = [timeout.value];

	return(xivo_fm_select_add_ast_application('absolutetimeout',args));
}

function xivo_ast_application_agi()
{
	if((agicommand = dwho_eid('it-ipbxapplication-agi-command')) === false
	|| (agiargs = dwho_eid('it-ipbxapplication-agi-args')) === false)
		return(false);

	var agicommandvalue = xivo_ast_application_sanitize_arg(agicommand.value);

	if(agicommandvalue.length < 1)
		return(false);

	var args = [agicommandvalue];

	if(agiargs.value.length > 0)
		args.push(agiargs.value.replace(/,/g,'|'));

	return(xivo_fm_select_add_ast_application('agi',args));
}

function xivo_ast_application_answer()
{
	if((delay = dwho_eid('it-ipbxapplication-answer-delay')) === false)
		return(false);

	var args = [];

	var delayvalue = xivo_ast_application_sanitize_arg(delay.value);

	if(dwho_is_uint(delayvalue) === true)
		args.push(delayvalue);

	return(xivo_fm_select_add_ast_application('answer',args));
}

function xivo_ast_application_authenticate()
{
	if((password = dwho_eid('it-ipbxapplication-authenticate-password')) === false
	|| (passwordinterpreter = dwho_eid('it-ipbxapplication-authenticate-passwordinterpreter')) === false
	|| (option_a = dwho_eid('it-ipbxapplication-authenticate-a')) === false
	|| (option_j = dwho_eid('it-ipbxapplication-authenticate-j')) === false
	|| (option_r = dwho_eid('it-ipbxapplication-authenticate-r')) === false)
		return(false);

	var passwordvalue = xivo_ast_application_sanitize_arg(password.value);

	if(passwordvalue.length < 1)
		return(false);

	var args = [passwordvalue];

	var options = '';

	if(option_a.checked === true)
		options += 'a';

	if(option_j.checked === true)
		options += 'j';

	switch(passwordinterpreter.value)
	{
		case 'astdb':
			options += 'd';

			if(option_r.checked === true)
				options += 'r';
			break;
		case 'file':
			options += 'm';
			break;
	}

	if(options.length > 0)
		args.push(options);

	return(xivo_fm_select_add_ast_application('authenticate',args));
}

function xivo_ast_application_background()
{
	if((filename = dwho_eid('it-ipbxapplication-background-filename')) === false
	|| (option_s = dwho_eid('it-ipbxapplication-background-s')) === false
	|| (option_n = dwho_eid('it-ipbxapplication-background-n')) === false
	|| (option_m = dwho_eid('it-ipbxapplication-background-m')) === false
	|| (language = dwho_eid('it-ipbxapplication-background-language')) === false
	|| (context = dwho_eid('it-ipbxapplication-background-context')) === false)
		return(false);

	var filenamevalue = xivo_ast_application_sanitize_arg(filename.value);

	if(filenamevalue.length < 1)
		return(false);

	var args = [filenamevalue];

	var options = '';

	if(option_s.checked === true)
		options += 's';

	if(option_n.checked === true)
		options += 'n';

	if(option_m.checked === true)
		options += 'm';

	var languagevalue = xivo_ast_application_sanitize_arg(language.value);
	var contextvalue = xivo_ast_application_sanitize_arg(context.value);

	if(contextvalue.length > 0)
	{
		args.push(options);
		args.push(languagevalue);
		args.push(contextvalue);
	}
	else if(languagevalue.length > 0)
	{
		args.push(options);
		args.push(languagevalue);
	}
	else if(options.length > 0)
		args.push(options);

	return(xivo_fm_select_add_ast_application('background',args));
}

function xivo_ast_application_digittimeout()
{
	if((timeout = dwho_eid('it-ipbxapplication-digittimeout-timeout')) === false
	|| dwho_is_ufloat(timeout.value) === false)
		return(false);

	var args = [timeout.value];

	return(xivo_fm_select_add_ast_application('digittimeout',args));
}

function xivo_ast_application_goto()
{
	if((context = dwho_eid('it-ipbxapplication-goto-context')) === false
	|| (exten = dwho_eid('it-ipbxapplication-goto-exten')) === false
	|| (priority = dwho_eid('it-ipbxapplication-goto-priority')) === false)
		return(false);

	var priorityvalue = xivo_ast_application_sanitize_arg(priority.value);

	if(priorityvalue.length < 1)
		return(false);

	var args = [];

	args.push(xivo_ast_application_sanitize_arg(context.value));
	args.push(xivo_ast_application_sanitize_arg(exten.value));
	args.push(priorityvalue);

	return(xivo_fm_select_add_ast_application('goto',args));
}

function xivo_ast_application_gotoif()
{
	if((condition = dwho_eid('it-ipbxapplication-gotoif-condition')) === false
	|| (iftrue = dwho_eid('it-ipbxapplication-gotoif-iftrue')) === false
	|| (iffalse = dwho_eid('it-ipbxapplication-gotoif-iffalse')) === false)
		return(false);

	var conditionvalue = condition.value.replace(/\?/g,'');

	if(conditionvalue.length < 1)
		return(false);

	var iftruevalue = iftrue.value.replace(/:/g,'');
	var iffalsevalue = iffalse.value.replace(/:/g,'');

	if(iftruevalue.length < 1 && iffalsevalue.length < 1)
		return(false);

	var args = [conditionvalue+'?'+iftruevalue+':'+iffalsevalue];

	return(xivo_fm_select_add_ast_application('gotoif',args));
}

function xivo_ast_application_macro()
{
	if((macroname = dwho_eid('it-ipbxapplication-macro-macroname')) === false
	|| (macroargs = dwho_eid('it-ipbxapplication-macro-args')) === false)
		return(false);

	var macronamevalue = xivo_ast_application_sanitize_arg(macroname.value);

	if(macronamevalue.length < 1)
		return(false);

	var args = [macronamevalue];

	if(macroargs.value.length > 0)
		args.push(macroargs.value.replace(/,/g,'|'));

	return(xivo_fm_select_add_ast_application('macro',args));
}

function xivo_ast_application_mixmonitor()
{
	if((filename = dwho_eid('it-ipbxapplication-mixmonitor-filename')) === false
	|| (fileformat = dwho_eid('it-ipbxapplication-mixmonitor-fileformat')) === false
	|| (option_a = dwho_eid('it-ipbxapplication-mixmonitor-a')) === false
	|| (option_b = dwho_eid('it-ipbxapplication-mixmonitor-b')) === false
	|| (option_v = dwho_eid('it-ipbxapplication-mixmonitor-v')) === false
	|| (option_v_volume = dwho_eid('it-ipbxapplication-mixmonitor-v-volume')) === false
	|| (option_vv = dwho_eid('it-ipbxapplication-mixmonitor-vv')) === false
	|| (option_vv_volume = dwho_eid('it-ipbxapplication-mixmonitor-vv-volume')) === false
	|| (option_w = dwho_eid('it-ipbxapplication-mixmonitor-w')) === false
	|| (option_w_volume = dwho_eid('it-ipbxapplication-mixmonitor-w-volume')) === false)
		return(false);

	var filenamevalue = xivo_ast_application_sanitize_arg(filename.value);
	var fileformatvalue = xivo_ast_application_sanitize_arg(fileformat.value);

	if(filenamevalue.length < 1 || fileformatvalue.length < 2)
		return(false);

	var args = [filenamevalue+'.'+fileformatvalue];

	var options = '';

	if(option_a.checked === true)
		options += 'a';

	if(option_b.checked === true)
		options += 'b';

	if(option_v.checked === true
	&& option_v_volume.value > -5 && option_v_volume.value < 5)
		options += 'v('+option_v_volume.value+')';

	if(option_vv.checked === true
	&& option_vv_volume.value > -5 && option_vv_volume.value < 5)
		options += 'V('+option_vv_volume.value+')';

	if(option_w.checked === true
	&& option_w_volume.value > -5 && option_w_volume.value < 5)
		options += 'W('+option_w_volume.value+')';

	if(options.length > 0)
		args.push(options);

	return(xivo_fm_select_add_ast_application('mixmonitor',args));
}

function xivo_ast_application_monitor()
{
	if((fileformat = dwho_eid('it-ipbxapplication-monitor-fileformat')) === false
	|| (basename = dwho_eid('it-ipbxapplication-monitor-basename')) === false
	|| (option_m = dwho_eid('it-ipbxapplication-monitor-m')) === false
	|| (option_b = dwho_eid('it-ipbxapplication-monitor-b')) === false)
		return(false);

	var fileformatvalue = xivo_ast_application_sanitize_arg(fileformat.value);

	if(fileformatvalue.length < 2)
		return(false);

	var args = [fileformatvalue];

	var basenamevalue = xivo_ast_application_sanitize_arg(basename.value);

	var options = '';

	if(option_m.checked === true)
		options += 'm';

	if(option_b.checked === true)
		options += 'b';

	if(options.length > 0)
	{
		args.push(basenamevalue);
		args.push(options);
	}
	else if(basenamevalue.length > 0)
		args.push(basenamevalue);

	return(xivo_fm_select_add_ast_application('monitor',args));
}

function xivo_ast_application_noop()
{
	if((data = dwho_eid('it-ipbxapplication-noop-data')) === false)
		return(false);

	var args = [];

	if(data.value.length > 0)
		args.push(data.value);

	return(xivo_fm_select_add_ast_application('noop',args));
}

function xivo_ast_application_playback()
{
	if((filename = dwho_eid('it-ipbxapplication-playback-filename')) === false
	|| (option_skip = dwho_eid('it-ipbxapplication-playback-skip')) === false
	|| (option_noanswer = dwho_eid('it-ipbxapplication-playback-noanswer')) === false
	|| (option_j = dwho_eid('it-ipbxapplication-playback-j')) === false)
		return(false);

	var filenamevalue = xivo_ast_application_sanitize_arg(filename.value);

	if(filenamevalue.length < 1)
		return(false);

	var args = [filenamevalue];

	var options = '';

	if(option_skip.checked === true)
		options += 'skip';

	if(option_noanswer.checked === true)
		options += 'noanswer';

	if(option_j.checked === true)
		options += 'j';

	if(options.length > 0)
		args.push(options);

	return(xivo_fm_select_add_ast_application('playback',args));
}

function xivo_ast_application_read()
{
	if((variable = dwho_eid('it-ipbxapplication-read-variable')) === false
	|| (filename = dwho_eid('it-ipbxapplication-read-filename')) === false
	|| (maxdigits = dwho_eid('it-ipbxapplication-read-maxdigits')) === false
	|| (option_s = dwho_eid('it-ipbxapplication-read-s')) === false
	|| (option_i = dwho_eid('it-ipbxapplication-read-i')) === false
	|| (option_n = dwho_eid('it-ipbxapplication-read-n')) === false
	|| (attempts = dwho_eid('it-ipbxapplication-read-attempts')) === false
	|| (timeout = dwho_eid('it-ipbxapplication-read-timeout')) === false
	|| (maxdigits.value.length > 0 && dwho_is_uint(maxdigits.value) === false) === true
	|| (attempts.value.length > 0 && dwho_is_uint(attempts.value) === false) === true
	|| (timeout.value.length > 0 && dwho_is_uint(timeout.value) === false) === true)
		return(false);

	var variablevalue = xivo_ast_application_sanitize_arg(variable.value);
	var filenamevalue = xivo_ast_application_sanitize_arg(filename.value);

	if(variablevalue.length < 1)
		return(false);

	var args = [variablevalue];

	var options = '';

	if(option_s.checked === true)
		options += 's';

	if(option_i.checked === true)
		options += 'i';

	if(option_n.checked === true)
		options += 'n';

	if(timeout.value.length > 0)
	{
		args.push(filename.value);
		args.push(maxdigits.value);
		args.push(options);
		args.push(attempts.value);
		args.push(timeout.value);
	}
	else if(attempts.value.length > 0)
	{
		args.push(filename.value);
		args.push(maxdigits.value);
		args.push(options);
		args.push(attempts.value);
	}
	else if(options.length > 0)
	{
		args.push(filename.value);
		args.push(maxdigits.value);
		args.push(options);
	}
	else if(maxdigits.value.length > 0)
	{
		args.push(filename.value);
		args.push(maxdigits.value);
	}
	else if(filename.value.length > 0)
		args.push(filename.value);

	return(xivo_fm_select_add_ast_application('read',args));
}

function xivo_ast_application_record()
{
	if((filename = dwho_eid('it-ipbxapplication-record-filename')) === false
	|| (fileformat = dwho_eid('it-ipbxapplication-record-fileformat')) === false
	|| (silence = dwho_eid('it-ipbxapplication-record-silence')) === false
	|| (maxduration = dwho_eid('it-ipbxapplication-record-maxduration')) === false
	|| (option_a = dwho_eid('it-ipbxapplication-record-a')) === false
	|| (option_n = dwho_eid('it-ipbxapplication-record-n')) === false
	|| (option_q = dwho_eid('it-ipbxapplication-record-q')) === false
	|| (option_s = dwho_eid('it-ipbxapplication-record-s')) === false
	|| (option_t = dwho_eid('it-ipbxapplication-record-t')) === false
	|| (silence.value.length > 0 && dwho_is_ufloat(silence.value) === false) === true
	|| (maxduration.value.length > 0 && dwho_is_ufloat(maxduration.value) === false) === true)
		return(false);

	var filenamevalue = xivo_ast_application_sanitize_arg(filename.value);
	var fileformatvalue = xivo_ast_application_sanitize_arg(fileformat.value);

	if(filenamevalue.length < 1 || fileformatvalue.length < 2)
		return(false);

	var args = [filenamevalue+'.'+fileformatvalue];

	var options = '';

	if(option_a.checked === true)
		options += 'a';

	if(option_n.checked === true)
		options += 'n';

	if(option_q.checked === true)
		options += 'q';

	if(option_s.checked === true)
		options += 's';

	if(option_t.checked === true)
		options += 't';

	if(options.length > 0)
	{
		args.push(silence.value);
		args.push(maxduration.value);
		args.push(options);
	}
	else if(maxduration.value.length > 0)
	{
		args.push(silence.value);
		args.push(maxduration.value);
	}
	else if(silence.value.length > 0)
		args.push(silence.value);

	return(xivo_fm_select_add_ast_application('record',args));
}

function xivo_ast_application_responsetimeout()
{
	if((timeout = dwho_eid('it-ipbxapplication-responsetimeout-timeout')) === false
	|| dwho_is_ufloat(timeout.value) === false)
		return(false);

	var args = [timeout.value];

	return(xivo_fm_select_add_ast_application('responsetimeout',args));
}

function xivo_ast_application_set()
{
	if((name = dwho_eid('it-ipbxapplication-set-name')) === false
	|| (value = dwho_eid('it-ipbxapplication-set-value')) === false
	|| (option_g = dwho_eid('it-ipbxapplication-set-g')) === false)
		return(false);

	var namevalue = xivo_ast_application_sanitize_arg(name.value).replace(/=/g,'');
	var valuevalue = xivo_ast_application_sanitize_arg(value.value).replace(/=/g,'');

	if(namevalue.length < 1)
		return(false);

	var args = [namevalue+'='+valuevalue];

	if(option_g.checked === true)
		args.push('g');

	return(xivo_fm_select_add_ast_application('set',args));
}

function xivo_ast_application_setcallerid()
{
	if((callerid = dwho_eid('it-ipbxapplication-setcallerid-callerid')) === false)
		return(false);

	var calleridvalue = xivo_ast_application_sanitize_arg(callerid.value);

	if(calleridvalue.length < 1)
		return(false);

	var args = [calleridvalue];

	return(xivo_fm_select_add_ast_application('setcallerid',args));
}

function xivo_ast_application_setcidname()
{
	if((name = dwho_eid('it-ipbxapplication-setcidname-name')) === false)
		return(false);

	var args = [xivo_ast_application_sanitize_arg(name.value)];

	return(xivo_fm_select_add_ast_application('setcidname',args));
}

function xivo_ast_application_setcidnum()
{
	if((number = dwho_eid('it-ipbxapplication-setcidnum-number')) === false)
		return(false);

	var numbervalue = xivo_ast_application_sanitize_arg(number.value);

	if(numbervalue.length < 1)
		return(false);

	var args = [numbervalue];

	return(xivo_fm_select_add_ast_application('setcidnum',args));
}

function xivo_ast_application_setlanguage()
{
	if((language = dwho_eid('it-ipbxapplication-setlanguage-language')) === false)
		return(false);

	var languagevalue = xivo_ast_application_sanitize_arg(language.value);

	if(languagevalue.length < 1)
		return(false);

	var args = [languagevalue];

	return(xivo_fm_select_add_ast_application('setlanguage',args));
}

function xivo_ast_application_stopmonitor()
{
	return(xivo_fm_select_add_ast_application('stopmonitor'));
}

function xivo_ast_application_vmauthenticate()
{
	if((mailbox = dwho_eid('it-ipbxapplication-vmauthenticate-mailbox')) === false
	|| mailbox.type !== 'select-one'
	|| dwho_is_undef(mailbox.options[mailbox.selectedIndex]) === true
	|| (option_s = dwho_eid('it-ipbxapplication-vmauthenticate-s')) === false)
		return(false);

	var mailboxvalue = xivo_ast_application_sanitize_arg(mailbox.value);

	if(mailboxvalue.length < 1)
		return(false);

	var optargs = ['\''+mailbox.options[mailbox.selectedIndex].text+'\''];
	var valargs = [mailboxvalue];

	if(option_s.checked === true)
	{
		optargs.push('s');
		valargs.push('s');
	}

	return(xivo_fm_select_add_ast_application('macro|vmauthenticate',optargs,valargs));
}

function xivo_ast_application_get_vmauthenticate_identity(id)
{
	return(dwho.form.get_text_opt_select('it-ipbxapplication-vmauthenticate-mailbox',id,true));
}

function xivo_ast_application_wait()
{
	if((seconds = dwho_eid('it-ipbxapplication-wait-seconds')) === false
	|| dwho_is_ufloat(seconds.value) === false)
		return(false);

	var args = [seconds.value];

	return(xivo_fm_select_add_ast_application('wait',args));
}

function xivo_ast_application_waitexten()
{
	if((seconds = dwho_eid('it-ipbxapplication-waitexten-seconds')) === false
	|| (option_m = dwho_eid('it-ipbxapplication-waitexten-m')) === false
	|| (seconds.value.length > 0 && dwho_is_ufloat(seconds.value) === false) === true)
		return(false);

	if((musiconhold = dwho_eid('it-ipbxapplication-waitexten-musiconhold')) !== false)
		var musiconholdvalue = xivo_ast_application_sanitize_arg(musiconhold.value);
	else
		var musiconholdvalue = '';

	var args = [seconds.value];

	if(option_m.checked === true)
	{
		if(musiconholdvalue.length > 0)
			args.push('m('+musiconholdvalue+')');
		else
			args.push('m');
	}

	return(xivo_fm_select_add_ast_application('waitexten',args));
}

function xivo_ast_application_waitforring()
{
	if((timeout = dwho_eid('it-ipbxapplication-waitforring-timeout')) === false
	|| dwho_is_ufloat(timeout.value) === false)
		return(false);

	var args = [timeout.value];

	return(xivo_fm_select_add_ast_application('waitforring',args));
}

function xivo_ast_application_waitmusiconhold()
{
	if((delay = dwho_eid('it-ipbxapplication-waitmusiconhold-delay')) === false
	|| dwho_is_ufloat(delay.value) === false)
		return(false);

	var args = [delay.value];

	return(xivo_fm_select_add_ast_application('waitmusiconhold',args));
}

function xivo_fm_select_add_ast_application(app,optargs,valargs)
{
	if((appdisplayname = xivo_ast_get_application_displayname(app)) === false)
		return(false);
	else if(dwho_is_array(optargs) === true)
		var optionargs = optargs.join(',').replace(/\|/g,',');
	else
		var optionargs = '';

	if(dwho_is_array(valargs) === true)
		var valueargs = valargs.join('|');
	else if(dwho_is_array(optargs) === true)
		var valueargs = optargs.join('|');
	else
		var valueargs = '';

	var option = appdisplayname+'('+optionargs+')';
	var value = app+','+valueargs;

	return(dwho.form.select_add_entry('it-voicemenu-flow',option,value,true));
}

function xivo_ast_get_application_displayname(app)
{
	if(dwho_is_object(xivo_ast_application[app]) === true
	&& dwho_is_undef(xivo_ast_application[app].displayname) === false)
		return(xivo_ast_application[app].displayname);

	return(false);
}

function xivo_ast_get_application_identityfunc(app)
{
	if(dwho_is_object(xivo_ast_application[app]) === true
	&& dwho_is_undef(xivo_ast_application[app].identityfunc) === false)
		return(xivo_ast_application[app].identityfunc);

	return(false);
}

function xivo_ast_application_sanitize_arg(str)
{
	return(str.replace(/[\|,]/g,''));
}

function xivo_ast_chg_ipbxapplication(app)
{
	for(property in xivo_ast_application)
	{
		if((appkey = xivo_ast_get_application_displayname(property)) === false)
			continue;

		appkey = appkey.toLowerCase();

		if(app !== appkey && (appid = dwho_eid('fd-ipbxapplication-'+appkey)) !== false)
		{
			appid.style.display = 'none';
			dwho.form.reset_child_field(appid,false);
		}
	}

	if(app !== null && (appid = dwho_eid('fd-ipbxapplication-'+app)) !== false)
		appid.style.display = 'block';
}

var xivo_ast_application = {
	'macro|vmauthenticate':
				{displayname:	'VMAuthenticate',
				 identityfunc:	xivo_ast_application_get_vmauthenticate_identity},
	'absolutetimeout':	{displayname:	'AbsoluteTimeout'},
	'agi':			{displayname:	'AGI'},
	'answer':		{displayname:	'Answer'},
	'authenticate':		{displayname:	'Authenticate'},
	'background':		{displayname:	'BackGround'},
	'digittimeout':		{displayname:	'DigitTimeout'},
	'goto':			{displayname:	'Goto'},
	'gotoif':		{displayname:	'GotoIf'},
	'macro':		{displayname:	'Macro'},
	'mixmonitor':		{displayname:	'MixMonitor'},
	'monitor':		{displayname:	'Monitor'},
	'noop':			{displayname:	'NoOp'},
	'playback':		{displayname:	'Playback'},
	'record':		{displayname:	'Record'},
	'responsetimeout':	{displayname:	'ResponseTimeout'},
	'read':			{displayname:	'Read'},
	'set':			{displayname:	'Set'},
	'setcallerid':		{displayname:	'SetCallerID'},
	'setcidname':		{displayname:	'SetCIDName'},
	'setcidnum':		{displayname:	'SetCIDNum'},
	'setlanguage':		{displayname:	'SetLanguage'},
	'stopmonitor':		{displayname:	'StopMonitor'},
	'wait':			{displayname:	'Wait'},
	'waitexten':		{displayname:	'WaitExten'},
	'waitforring':		{displayname:	'WaitForRing'},
	'waitmusiconhold':	{displayname:	'WaitMusicOnHold'}};
