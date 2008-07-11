var xivo_ast_application = new Array();
xivo_ast_application['macro|vmauthenticate'] = {displayname: 'VMAuthenticate',
						identityfunc: 'xivo_ast_application_get_vmauthenticate_identity'};
xivo_ast_application['absolutetimeout'] = {displayname: 'AbsoluteTimeout'};
xivo_ast_application['agi'] = {displayname: 'AGI'};
xivo_ast_application['answer'] = {displayname: 'Answer'};
xivo_ast_application['authenticate'] = {displayname: 'Authenticate'};
xivo_ast_application['background'] = {displayname: 'BackGround'};
xivo_ast_application['digittimeout'] = {displayname: 'DigitTimeout'};
xivo_ast_application['goto'] = {displayname: 'Goto'};
xivo_ast_application['gotoif'] = {displayname: 'GotoIf'};
xivo_ast_application['macro'] = {displayname: 'Macro'};
xivo_ast_application['mixmonitor'] = {displayname: 'MixMonitor'};
xivo_ast_application['monitor'] = {displayname: 'Monitor'};
xivo_ast_application['noop'] = {displayname: 'NoOp'};
xivo_ast_application['playback'] = {displayname: 'Playback'};
xivo_ast_application['record'] = {displayname: 'Record'};
xivo_ast_application['responsetimeout'] = {displayname: 'ResponseTimeout'};
xivo_ast_application['set'] = {displayname: 'Set'};
xivo_ast_application['setcallerid'] = {displayname: 'SetCallerID'};
xivo_ast_application['setcidname'] = {displayname: 'SetCIDName'};
xivo_ast_application['setcidnum'] = {displayname: 'SetCIDNum'};
xivo_ast_application['setlanguage'] = {displayname: 'SetLanguage'};
xivo_ast_application['stopmonitor'] = {displayname: 'StopMonitor'};
xivo_ast_application['wait'] = {displayname: 'Wait'};
xivo_ast_application['waitexten'] = {displayname: 'WaitExten'};
xivo_ast_application['waitforring'] = {displayname: 'WaitForRing'};
xivo_ast_application['waitmusiconhold'] = {displayname: 'WaitMusicOnHold'};

function xivo_ast_application_absolutetimeout()
{
	if((timeout = xivo_eid('it-ipbxapplication-absolutetimeout-timeout')) === false
	|| xivo_is_ufloat(timeout.value) === false)
		return(false);

	var args = new Array(timeout.value);

	return(xivo_fm_select_add_ast_application('absolutetimeout',args));
}

function xivo_ast_application_agi()
{
	if((agicommand = xivo_eid('it-ipbxapplication-agi-command')) === false
	|| (agiargs = xivo_eid('it-ipbxapplication-agi-args')) === false)
		return(false);

	var agicommandvalue = xivo_ast_application_sanitize_arg(agicommand.value);

	if(agicommandvalue.length < 1)
		return(false);

	var args = new Array(agicommandvalue);

	if(agiargs.value.length > 0)
		args.push(agiargs.value.replace(/,/g,'|'));

	return(xivo_fm_select_add_ast_application('agi',args));
}

function xivo_ast_application_answer()
{
	if((delay = xivo_eid('it-ipbxapplication-answer-delay')) === false)
		return(false);

	var args = new Array();

	var delayvalue = xivo_ast_application_sanitize_arg(delay.value);

	if(xivo_is_uint(delayvalue) === true)
		args.push(delayvalue);
	
	return(xivo_fm_select_add_ast_application('answer',args));
}

function xivo_ast_application_authenticate()
{
	if((password = xivo_eid('it-ipbxapplication-authenticate-password')) === false
	|| (passwordinterpreter = xivo_eid('it-ipbxapplication-authenticate-passwordinterpreter')) === false
	|| (option_a = xivo_eid('it-ipbxapplication-authenticate-a')) === false
	|| (option_j = xivo_eid('it-ipbxapplication-authenticate-j')) === false
	|| (option_r = xivo_eid('it-ipbxapplication-authenticate-r')) === false)
		return(false);

	var passwordvalue = xivo_ast_application_sanitize_arg(password.value);

	if(passwordvalue.length < 1)
		return(false);

	var args = new Array(passwordvalue);

	var options = '';

	if(option_a.checked == true)
		options += 'a';
	
	if(option_j.checked == true)
		options += 'j';
	
	switch(passwordinterpreter.value)
	{
		case 'astdb':
			options += 'd';

			if(option_r.checked == true)
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
	if((filename = xivo_eid('it-ipbxapplication-background-filename')) === false
	|| (option_s = xivo_eid('it-ipbxapplication-background-s')) === false
	|| (option_n = xivo_eid('it-ipbxapplication-background-n')) === false
	|| (option_m = xivo_eid('it-ipbxapplication-background-m')) === false
	|| (language = xivo_eid('it-ipbxapplication-background-language')) === false
	|| (context = xivo_eid('it-ipbxapplication-background-context')) === false)
		return(false);

	var filenamevalue = xivo_ast_application_sanitize_arg(filename.value);

	if(filenamevalue.length < 1)
		return(false);

	var args = new Array(filenamevalue);

	var options = '';

	if(option_s.checked == true)
		options += 's';
	
	if(option_n.checked == true)
		options += 'n';
	
	if(option_m.checked == true)
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
	if((timeout = xivo_eid('it-ipbxapplication-digittimeout-timeout')) === false
	|| xivo_is_ufloat(timeout.value) === false)
		return(false);

	var args = new Array(timeout.value);

	return(xivo_fm_select_add_ast_application('digittimeout',args));
}

function xivo_ast_application_goto()
{
	if((context = xivo_eid('it-ipbxapplication-goto-context')) === false
	|| (exten = xivo_eid('it-ipbxapplication-goto-exten')) === false
	|| (priority = xivo_eid('it-ipbxapplication-goto-priority')) === false)
		return(false);

	var priorityvalue = xivo_ast_application_sanitize_arg(priority.value);

	if(priorityvalue.length < 1)
		return(false);

	var args = new Array();

	args.push(xivo_ast_application_sanitize_arg(context.value));
	args.push(xivo_ast_application_sanitize_arg(exten.value));
	args.push(priorityvalue);
	
	return(xivo_fm_select_add_ast_application('goto',args));
}

function xivo_ast_application_gotoif()
{
	if((condition = xivo_eid('it-ipbxapplication-gotoif-condition')) === false
	|| (iftrue = xivo_eid('it-ipbxapplication-gotoif-iftrue')) === false
	|| (iffalse = xivo_eid('it-ipbxapplication-gotoif-iffalse')) === false)
		return(false);

	var conditionvalue = condition.value.replace(/\?/g,'');

	if(conditionvalue.length < 1)
		return(false);

	var iftruevalue = iftrue.value.replace(/:/g,'');
	var iffalsevalue = iffalse.value.replace(/:/g,'');

	if(iftruevalue.length < 1 && iffalsevalue.length < 1)
		return(false);

	var args = new Array(conditionvalue+'?'+iftruevalue+':'+iffalsevalue);

	return(xivo_fm_select_add_ast_application('gotoif',args));
}

function xivo_ast_application_macro()
{
	if((macroname = xivo_eid('it-ipbxapplication-macro-macroname')) === false
	|| (macroargs = xivo_eid('it-ipbxapplication-macro-args')) === false)
		return(false);

	var macronamevalue = xivo_ast_application_sanitize_arg(macroname.value);

	if(macronamevalue.length < 1)
		return(false);

	var args = new Array(macronamevalue);

	if(macroargs.value.length > 0)
		args.push(macroargs.value.replace(/,/g,'|'));

	return(xivo_fm_select_add_ast_application('macro',args));
}

function xivo_ast_application_mixmonitor()
{
	if((filename = xivo_eid('it-ipbxapplication-mixmonitor-filename')) === false
	|| (fileformat = xivo_eid('it-ipbxapplication-mixmonitor-fileformat')) === false
	|| (option_a = xivo_eid('it-ipbxapplication-mixmonitor-a')) === false
	|| (option_b = xivo_eid('it-ipbxapplication-mixmonitor-b')) === false
	|| (option_v = xivo_eid('it-ipbxapplication-mixmonitor-v')) === false
	|| (option_v_volume = xivo_eid('it-ipbxapplication-mixmonitor-v-volume')) === false
	|| (option_vv = xivo_eid('it-ipbxapplication-mixmonitor-vv')) === false
	|| (option_vv_volume = xivo_eid('it-ipbxapplication-mixmonitor-vv-volume')) === false
	|| (option_w = xivo_eid('it-ipbxapplication-mixmonitor-w')) === false
	|| (option_w_volume = xivo_eid('it-ipbxapplication-mixmonitor-w-volume')) === false)
		return(false);

	var filenamevalue = xivo_ast_application_sanitize_arg(filename.value);
	var fileformatvalue = xivo_ast_application_sanitize_arg(fileformat.value);

	if(filenamevalue.length < 1 || fileformatvalue.length < 2)
		return(false);

	var args = new Array(filenamevalue+'.'+fileformatvalue);

	var options = '';

	if(option_a.checked == true)
		options += 'a';

	if(option_b.checked == true)
		options += 'b';

	if(option_v.checked == true
	&& option_v_volume.value > -5 && option_v_volume.value < 5)
		options += 'v('+option_v_volume.value+')';

	if(option_vv.checked == true
	&& option_vv_volume.value > -5 && option_vv_volume.value < 5)
		options += 'V('+option_vv_volume.value+')';

	if(option_w.checked == true
	&& option_w_volume.value > -5 && option_w_volume.value < 5)
		options += 'W('+option_w_volume.value+')';

	if(options.length > 0)
		args.push(options);

	return(xivo_fm_select_add_ast_application('mixmonitor',args));
}

function xivo_ast_application_monitor()
{
	if((fileformat = xivo_eid('it-ipbxapplication-monitor-fileformat')) === false
	|| (basename = xivo_eid('it-ipbxapplication-monitor-basename')) === false
	|| (option_m = xivo_eid('it-ipbxapplication-monitor-m')) === false
	|| (option_b = xivo_eid('it-ipbxapplication-monitor-b')) === false)
		return(false);

	var fileformatvalue = xivo_ast_application_sanitize_arg(fileformat.value);

	if(fileformatvalue.length < 2)
		return(false);

	var args = new Array(fileformatvalue);

	var basenamevalue = xivo_ast_application_sanitize_arg(basename.value);

	var options = '';

	if(option_m.checked == true)
		options += 'm';

	if(option_b.checked == true)
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
	if((data = xivo_eid('it-ipbxapplication-noop-data')) === false)
		return(false);

	var args = new Array();

	if(data.value.length > 0)
		args.push(data.value);

	return(xivo_fm_select_add_ast_application('noop',args));
}

function xivo_ast_application_playback()
{
	if((filename = xivo_eid('it-ipbxapplication-playback-filename')) === false
	|| (option_skip = xivo_eid('it-ipbxapplication-playback-skip')) === false
	|| (option_noanswer = xivo_eid('it-ipbxapplication-playback-noanswer')) === false
	|| (option_j = xivo_eid('it-ipbxapplication-playback-j')) === false)
		return(false);

	var filenamevalue = xivo_ast_application_sanitize_arg(filename.value);

	if(filenamevalue.length < 1)
		return(false);

	var args = new Array(filenamevalue);

	var options = '';

	if(option_skip.checked == true)
		options += 'skip';
	
	if(option_noanswer.checked == true)
		options += 'noanswer';
	
	if(option_j.checked == true)
		options += 'j';

	if(options.length > 0)
		args.push(options);

	return(xivo_fm_select_add_ast_application('playback',args));
}

function xivo_ast_application_record()
{
	if((filename = xivo_eid('it-ipbxapplication-record-filename')) === false
	|| (fileformat = xivo_eid('it-ipbxapplication-record-fileformat')) === false
	|| (silence = xivo_eid('it-ipbxapplication-record-silence')) === false
	|| (maxduration = xivo_eid('it-ipbxapplication-record-maxduration')) === false
	|| (option_a = xivo_eid('it-ipbxapplication-record-a')) === false
	|| (option_n = xivo_eid('it-ipbxapplication-record-n')) === false
	|| (option_q = xivo_eid('it-ipbxapplication-record-q')) === false
	|| (option_s = xivo_eid('it-ipbxapplication-record-s')) === false
	|| (option_t = xivo_eid('it-ipbxapplication-record-t')) === false
	|| (silence.value.length > 0 && xivo_is_ufloat(silence.value) === false) === true
	|| (maxduration.value.length > 0 && xivo_is_ufloat(maxduration.value) === false) === true)
		return(false);

	var filenamevalue = xivo_ast_application_sanitize_arg(filename.value);
	var fileformatvalue = xivo_ast_application_sanitize_arg(fileformat.value);

	if(filenamevalue.length < 1 || fileformatvalue.length < 2)
		return(false);

	var args = new Array(filenamevalue+'.'+fileformatvalue);

	var options = '';

	if(option_a.checked == true)
		options += 'a';

	if(option_n.checked == true)
		options += 'n';

	if(option_q.checked == true)
		options += 'q';

	if(option_s.checked == true)
		options += 's';

	if(option_t.checked == true)
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
	if((timeout = xivo_eid('it-ipbxapplication-responsetimeout-timeout')) === false
	|| xivo_is_ufloat(timeout.value) === false)
		return(false);

	var args = new Array(timeout.value);

	return(xivo_fm_select_add_ast_application('responsetimeout',args));
}

function xivo_ast_application_set()
{
	if((name = xivo_eid('it-ipbxapplication-set-name')) === false
	|| (value = xivo_eid('it-ipbxapplication-set-value')) === false
	|| (option_g = xivo_eid('it-ipbxapplication-set-g')) === false)
		return(false);

	var namevalue = xivo_ast_application_sanitize_arg(name.value).replace(/=/g,'');
	var valuevalue = xivo_ast_application_sanitize_arg(value.value).replace(/=/g,'');

	if(namevalue.length < 1)
		return(false);

	var args = new Array(namevalue+'='+valuevalue);

	if(option_g.checked == true)
		args.push('g');

	return(xivo_fm_select_add_ast_application('set',args));
}

function xivo_ast_application_setcallerid()
{
	if((callerid = xivo_eid('it-ipbxapplication-setcallerid-callerid')) === false)
		return(false);

	var calleridvalue = xivo_ast_application_sanitize_arg(callerid.value);

	if(calleridvalue.length < 1)
		return(false);

	var args = new Array(calleridvalue);

	return(xivo_fm_select_add_ast_application('setcallerid',args));
}

function xivo_ast_application_setcidname()
{
	if((name = xivo_eid('it-ipbxapplication-setcidname-name')) === false)
		return(false);

	var args = new Array(xivo_ast_application_sanitize_arg(name.value));

	return(xivo_fm_select_add_ast_application('setcidname',args));
}

function xivo_ast_application_setcidnum()
{
	if((number = xivo_eid('it-ipbxapplication-setcidnum-number')) === false)
		return(false);

	var numbervalue = xivo_ast_application_sanitize_arg(number.value);

	if(numbervalue.length < 1)
		return(false);

	var args = new Array(numbervalue);

	return(xivo_fm_select_add_ast_application('setcidnum',args));
}

function xivo_ast_application_setlanguage()
{
	if((language = xivo_eid('it-ipbxapplication-setlanguage-language')) === false)
		return(false);

	var languagevalue = xivo_ast_application_sanitize_arg(language.value);

	if(languagevalue.length < 1)
		return(false);

	var args = new Array(languagevalue);

	return(xivo_fm_select_add_ast_application('setlanguage',args));
}

function xivo_ast_application_stopmonitor()
{
	return(xivo_fm_select_add_ast_application('stopmonitor'));
}

function xivo_ast_application_vmauthenticate()
{
	if((mailbox = xivo_eid('it-ipbxapplication-vmauthenticate-mailbox')) === false
	|| mailbox.type !== 'select-one'
	|| xivo_is_undef(mailbox.options[mailbox.selectedIndex]) === true
	|| (option_s = xivo_eid('it-ipbxapplication-vmauthenticate-s')) === false)
		return(false);

	var mailboxvalue = xivo_ast_application_sanitize_arg(mailbox.value);

	if(mailboxvalue.length < 1)
		return(false);

	var optargs = new Array('\''+mailbox.options[mailbox.selectedIndex].text+'\'');
	var valargs = new Array(mailboxvalue);

	if(option_s.checked == true)
	{
		optargs.push('s');
		valargs.push('s');
	}

	return(xivo_fm_select_add_ast_application('macro|vmauthenticate',optargs,valargs));
}

function xivo_ast_application_get_vmauthenticate_identity(id)
{
	return(xivo_fm_get_text_opt_select('it-ipbxapplication-vmauthenticate-mailbox',id,true));
}

function xivo_ast_application_wait()
{
	if((seconds = xivo_eid('it-ipbxapplication-wait-seconds')) === false
	|| xivo_is_ufloat(seconds.value) === false)
		return(false);
	
	var args = new Array(seconds.value);

	return(xivo_fm_select_add_ast_application('wait',args));
}

function xivo_ast_application_waitexten()
{
	if((seconds = xivo_eid('it-ipbxapplication-waitexten-seconds')) === false
	|| (option_m = xivo_eid('it-ipbxapplication-waitexten-m')) === false
	|| (seconds.value.length > 0 && xivo_is_ufloat(seconds.value) === false) === true)
		return(false);
	
	if((musiconhold = xivo_eid('it-ipbxapplication-waitexten-musiconhold')) !== false)
		var musiconholdvalue = xivo_ast_application_sanitize_arg(musiconhold.value);
	else
		var musiconholdvalue = '';

	var args = new Array(seconds.value);

	if(option_m.checked == true)
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
	if((timeout = xivo_eid('it-ipbxapplication-waitforring-timeout')) === false
	|| xivo_is_ufloat(timeout.value) === false)
		return(false);
	
	var args = new Array(timeout.value);

	return(xivo_fm_select_add_ast_application('waitforring',args));
}

function xivo_ast_application_waitmusiconhold()
{
	if((delay = xivo_eid('it-ipbxapplication-waitmusiconhold-delay')) === false
	|| xivo_is_ufloat(delay.value) === false)
		return(false);
	
	var args = new Array(delay.value);

	return(xivo_fm_select_add_ast_application('waitmusiconhold',args));
}

function xivo_fm_select_add_ast_application(app,optargs,valargs)
{
	if((appdisplayname = xivo_ast_get_application_displayname(app)) === false)
		return(false);
	else if(xivo_is_array(optargs) === true)
		var optionargs = optargs.join(',').replace(/\|/g,',');
	else
		var optionargs = '';

	if(xivo_is_array(valargs) === true)
		var valueargs = valargs.join('|');
	else if(xivo_is_array(optargs) === true)
		var valueargs = optargs.join('|');
	else
		var valueargs = '';

	var option = appdisplayname+'('+optionargs+')';
	var value = app+','+valueargs;

	return(xivo_fm_select_add_entry('it-voicemenu-flow',option,value,true));
}

function xivo_ast_get_application_displayname(app)
{
	if(xivo_is_object(xivo_ast_application[app]) === true
	&& xivo_is_undef(xivo_ast_application[app].displayname) === false)
		return(xivo_ast_application[app].displayname);

	return(false);
}

function xivo_ast_get_application_identityfunc(app)
{
	if(xivo_is_object(xivo_ast_application[app]) === true
	&& xivo_is_undef(xivo_ast_application[app].identityfunc) === false)
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

		if(app !== appkey && (appid = xivo_eid('fd-ipbxapplication-'+appkey)) !== false)
		{
			appid.style.display = 'none';
			xivo_fm_reset_child_field(appid,false);
		}
	}

	if(app !== null && (appid = xivo_eid('fd-ipbxapplication-'+app)) !== false)
		appid.style.display = 'block';
}
