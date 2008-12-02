var xivo_ast_defapplication = new Array();
xivo_ast_defapplication['macro|endcall|hangup'] = {displayname: 'HangUp'};
xivo_ast_defapplication['macro|endcall|busy'] = {displayname: 'Busy'};
xivo_ast_defapplication['macro|endcall|congestion'] = {displayname: 'Congestion'};
xivo_ast_defapplication['macro|user'] = {displayname: 'CallUser',
					 identityfunc: 'xivo_ast_defapplication_get_user_identity'};
xivo_ast_defapplication['macro|group'] = {displayname: 'CallGroup',
					  identityfunc: 'xivo_ast_defapplication_get_group_identity'};
xivo_ast_defapplication['macro|queue'] = {displayname: 'CallQueue',
					  identityfunc: 'xivo_ast_defapplication_get_queue_identity'};
xivo_ast_defapplication['macro|meetme'] = {displayname: 'CallMeetMe',
					   identityfunc: 'xivo_ast_defapplication_get_meetme_identity'};
xivo_ast_defapplication['macro|voicemail'] = {displayname: 'VoiceMail',
					      identityfunc: 'xivo_ast_defapplication_get_voicemail_identity'};
xivo_ast_defapplication['macro|schedule'] = {displayname: 'GotoSchedule',
					     identityfunc: 'xivo_ast_defapplication_get_schedule_identity'};
xivo_ast_defapplication['macro|voicemenu'] = {displayname: 'GotoVoiceMenu',
					      identityfunc: 'xivo_ast_defapplication_get_voicemenu_identity'};
xivo_ast_defapplication['macro|extension'] = {displayname: 'CallExten'};
xivo_ast_defapplication['macro|callbackdisa'] = {displayname: 'CallBackDISA'};
xivo_ast_defapplication['macro|disa'] = {displayname: 'DISA'};
xivo_ast_defapplication['macro|directory'] = {displayname: 'Directory'};
xivo_ast_defapplication['macro|faxtomail'] = {displayname: 'FaxToMail'};
xivo_ast_defapplication['macro|voicemailmain'] = {displayname: 'VoiceMailMain'};
xivo_ast_defapplication['macro|playsound'] = {displayname: 'PlaySound'};

function xivo_ast_defapplication_endcall(dialevent,targetid)
{
	var optargs = valargs = '';

	if((endcall = xivo_eid('it-dialaction-'+dialevent+'-endcall-action')) === false
	|| (endcall.value !== 'hangup'
	   && endcall.value !== 'busy'
	   && endcall.value !== 'congestion') === true)
		return(false);
	else if((endcallarg1 = xivo_eid('it-dialaction-'+dialevent+'-endcall-'+endcall.value+'-actionarg1')) !== false
	&& endcall.value !== 'hangup'
	&& endcallarg1.value.length > 0)
	{
		if(xivo_is_ufloat(endcallarg1.value) === false)
			return(false);

		optargs = valargs = new Array(endcallarg1.value);
	}

	return(xivo_ast_set_defapplication('macro|endcall|'+endcall.value,targetid,optargs,valargs));
}

function xivo_ast_defapplication_user(dialevent,targetid)
{
	if((userarg1 = xivo_eid('it-dialaction-'+dialevent+'-user-actionarg1')) === false
	|| userarg1.type !== 'select-one'
	|| xivo_is_undef(userarg1.options[userarg1.selectedIndex]) === true
	|| (userarg2 = xivo_eid('it-dialaction-'+dialevent+'-user-actionarg2')) === false)
		return(false);

	var optargs = new Array('\''+userarg1.options[userarg1.selectedIndex].text+'\'');
	var valargs = new Array(userarg1.value);

	if(userarg2.value.length > 0)
	{
		if(xivo_is_ufloat(userarg2.value) === false)
			return(false);

		optargs.push(userarg2.value);
		valargs.push(userarg2.value);
	}

	return(xivo_ast_set_defapplication('macro|user',targetid,optargs,valargs));
}

function xivo_ast_defapplication_get_user_identity(id)
{
	return(xivo_fm_get_text_opt_select('it-dialaction-'+dialevent+'-user-actionarg1',id,true));
}

function xivo_ast_defapplication_group(dialevent,targetid)
{
	if((grouparg1 = xivo_eid('it-dialaction-'+dialevent+'-group-actionarg1')) === false
	|| grouparg1.type !== 'select-one'
	|| xivo_is_undef(grouparg1.options[grouparg1.selectedIndex]) === true
	|| (grouparg2 = xivo_eid('it-dialaction-'+dialevent+'-group-actionarg2')) === false)
		return(false);

	var optargs = new Array('\''+grouparg1.options[grouparg1.selectedIndex].text+'\'');
	var valargs = new Array(grouparg1.value);

	if(grouparg2.value.length > 0)
	{
		if(xivo_is_ufloat(grouparg2.value) === false)
			return(false);

		optargs.push(grouparg2.value);
		valargs.push(grouparg2.value);
	}

	return(xivo_ast_set_defapplication('macro|group',targetid,optargs,valargs));
}

function xivo_ast_defapplication_get_group_identity(id)
{
	return(xivo_fm_get_text_opt_select('it-dialaction-'+dialevent+'-group-actionarg1',id,true));
}

function xivo_ast_defapplication_queue(dialevent,targetid)
{
	if((queuearg1 = xivo_eid('it-dialaction-'+dialevent+'-queue-actionarg1')) === false
	|| queuearg1.type !== 'select-one'
	|| xivo_is_undef(queuearg1.options[queuearg1.selectedIndex]) === true
	|| (queuearg2 = xivo_eid('it-dialaction-'+dialevent+'-queue-actionarg2')) === false)
		return(false);

	var optargs = new Array('\''+queuearg1.options[queuearg1.selectedIndex].text+'\'');
	var valargs = new Array(queuearg1.value);

	if(queuearg2.value.length > 0)
	{
		if(xivo_is_ufloat(queuearg2.value) === false)
			return(false);

		optargs.push(queuearg2.value);
		valargs.push(queuearg2.value);
	}

	return(xivo_ast_set_defapplication('macro|queue',targetid,optargs,valargs));
}

function xivo_ast_defapplication_get_queue_identity(id)
{
	return(xivo_fm_get_text_opt_select('it-dialaction-'+dialevent+'-queue-actionarg1',id,true));
}

function xivo_ast_defapplication_meetme(dialevent,targetid)
{
	if((meetme = xivo_eid('it-dialaction-'+dialevent+'-meetme-actionarg1')) === false
	|| meetme.type !== 'select-one'
	|| xivo_is_undef(meetme.options[meetme.selectedIndex]) === true)
		return(false);

	var optargs = new Array('\''+meetme.options[meetme.selectedIndex].text+'\'');
	var valargs = new Array(meetme.value);

	return(xivo_ast_set_defapplication('macro|meetme',targetid,optargs,valargs));
}

function xivo_ast_defapplication_get_meetme_identity(id)
{
	return(xivo_fm_get_text_opt_select('it-dialaction-'+dialevent+'-meetme-actionarg1',id,true));
}

function xivo_ast_defapplication_voicemail(dialevent,targetid)
{
	if((voicemail = xivo_eid('it-dialaction-'+dialevent+'-voicemail-actionarg1')) === false
	|| voicemail.type !== 'select-one'
	|| xivo_is_undef(voicemail.options[voicemail.selectedIndex]) === true)
		return(false);

	var optargs = new Array('\''+voicemail.options[voicemail.selectedIndex].text+'\'');
	var valargs = new Array(voicemail.value);

	return(xivo_ast_set_defapplication('macro|voicemail',targetid,optargs,valargs));
}

function xivo_ast_defapplication_get_voicemail_identity(id)
{
	return(xivo_fm_get_text_opt_select('it-dialaction-'+dialevent+'-voicemail-actionarg1',id,true));
}

function xivo_ast_defapplication_schedule(dialevent,targetid)
{
	if((schedule = xivo_eid('it-dialaction-'+dialevent+'-schedule-actionarg1')) === false
	|| schedule.type !== 'select-one'
	|| xivo_is_undef(schedule.options[schedule.selectedIndex]) === true)
		return(false);

	var optargs = new Array('\''+schedule.options[schedule.selectedIndex].text+'\'');
	var valargs = new Array(schedule.value);

	return(xivo_ast_set_defapplication('macro|schedule',targetid,optargs,valargs));
}

function xivo_ast_defapplication_get_schedule_identity(id)
{
	return(xivo_fm_get_text_opt_select('it-dialaction-'+dialevent+'-schedule-actionarg1',id,true));
}

function xivo_ast_defapplication_voicemenu(dialevent,targetid)
{
	if((voicemenu = xivo_eid('it-dialaction-'+dialevent+'-voicemenu-actionarg1')) === false
	|| voicemenu.type !== 'select-one'
	|| xivo_is_undef(voicemenu.options[voicemenu.selectedIndex]) === true)
		return(false);

	var optargs = new Array('\''+voicemenu.options[voicemenu.selectedIndex].text+'\'');
	var valargs = new Array(voicemenu.value);

	return(xivo_ast_set_defapplication('macro|voicemenu',targetid,optargs,valargs));
}

function xivo_ast_defapplication_get_voicemenu_identity(id)
{
	return(xivo_fm_get_text_opt_select('it-dialaction-'+dialevent+'-voicemenu-actionarg1',id,true));
}

function xivo_ast_defapplication_extension(dialevent,targetid)
{
	if((actionarg1 = xivo_eid('it-dialaction-'+dialevent+'-extension-actionarg1')) === false
	|| (actionarg2 = xivo_eid('it-dialaction-'+dialevent+'-extension-actionarg2')) === false)
		return(false);

	var actionarg1value = xivo_ast_defapplication_sanitize_arg(actionarg1.value);
	var actionarg2value = xivo_ast_defapplication_sanitize_arg(actionarg2.value);

	if(actionarg1value.length < 1
	|| actionarg2value.length < 1)
		return(false);

	var optargs = valargs = new Array(actionarg1value,actionarg2value);

	return(xivo_ast_set_defapplication('macro|extension',targetid,optargs,valargs));
}

function xivo_ast_defapplication_application(dialevent,targetid)
{
	if((application = xivo_eid('it-dialaction-'+dialevent+'-application-action')) === false)
		return(false);

	var actionarg1_id = 'it-dialaction-'+dialevent+'-application-'+application.value+'-actionarg1';
	var actionarg2_id = 'it-dialaction-'+dialevent+'-application-'+application.value+'-actionarg2';

	switch(application.value)
	{
		case 'callbackdisa':
		case 'disa':
			if((applicationarg1 = xivo_eid(actionarg1_id)) === false
			|| (applicationarg2 = xivo_eid(actionarg2_id)) === false)
				return(false);

			var applicationarg1value = xivo_ast_defapplication_sanitize_arg(applicationarg1.value);
			var applicationarg2value = xivo_ast_defapplication_sanitize_arg(applicationarg2.value);

			if(applicationarg2value.length < 1)
				return(false);
			else if(applicationarg1value.length < 1)
				applicationarg1value = 'no-password';

			var optargs = valargs = new Array(applicationarg1value,applicationarg2value);
			break;
		case 'directory':
		case 'faxtomail':
		case 'voicemailmain':
			if((applicationarg1 = xivo_eid(actionarg1_id)) === false)
				return(false);

			var applicationarg1value = xivo_ast_defapplication_sanitize_arg(applicationarg1.value);

			if(applicationarg1value.length < 1)
				return(false);

			var optargs = valargs = new Array(applicationarg1value);
			break;
		default:
			return(false);
	}

	return(xivo_ast_set_defapplication('macro|'+application.value,targetid,optargs,valargs));
}

function xivo_ast_defapplication_sound(dialevent,targetid)
{
	if((filename = xivo_eid('it-dialaction-'+dialevent+'-sound-actionarg1')) === false
	|| (option_skip = xivo_eid('it-dialaction-'+dialevent+'-sound-actionarg2-skip')) === false
	|| (option_noanswer = xivo_eid('it-dialaction-'+dialevent+'-sound-actionarg2-noanswer')) === false
	|| (option_j = xivo_eid('it-dialaction-'+dialevent+'-sound-actionarg2-j')) === false)
		return(false);

	var valfilenamevalue = xivo_ast_application_sanitize_arg(filename.value);

	if((spos = valfilenamevalue.lastIndexOf('/')) > -1)
		var optfilenamevalue = valfilenamevalue.substring(spos+1);
	else
		var optfilenamevalue = valfilenamevalue;

	if(optfilenamevalue.length < 1)
		return(false);

	var optargs = new Array(optfilenamevalue);
	var valargs = new Array(valfilenamevalue);

	var options = '';

	if(option_skip.checked == true)
		options += 'skip';

	if(option_noanswer.checked == true)
		options += 'noanswer';

	if(option_j.checked == true)
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
	else if(xivo_is_array(optargs) === true)
		var optionargs = optargs.join(',').replace(/\|/g,',');
	else
		var optionargs = '';

	if(xivo_is_array(valargs) === true)
		var valueargs = valargs.join('|');
	else
		var valueargs = '';

	var option = appdisplayname+'('+optionargs+')';
	var value = app+','+valueargs;

	if((target = xivo_eid(targetid)) === false)
		return(false);
	else if(target.type === 'select-multiple' || target.type === 'select-one')
		return(xivo_fm_select_add_entry(targetid,option,value,true));
	else if(target.type === 'text')
		target.value = value;
	else if(xivo_is_undef(target.type) === true)
	{
		target.innerHTML = option;

		if((ittarget = xivo_eid('it-'+targetid)) !== false
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
	if(xivo_is_object(xivo_ast_defapplication[app]) === true
	&& xivo_is_undef(xivo_ast_defapplication[app].displayname) === false)
		return(xivo_ast_defapplication[app].displayname);

	return(false);
}

function xivo_ast_get_defapplication_identityfunc(app)
{
	if(xivo_is_object(xivo_ast_defapplication[app]) === true
	&& xivo_is_undef(xivo_ast_defapplication[app].identityfunc) === false)
		return(xivo_ast_defapplication[app].identityfunc);

	return(false);
}
