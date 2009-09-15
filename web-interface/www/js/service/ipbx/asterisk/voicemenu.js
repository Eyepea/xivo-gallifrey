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

xivo_ast_voicemenuevent_type = {
				'0':	'0',
				'1':	'1',
				'2':	'2',
				'3':	'3',
				'4':	'4',
				'5':	'5',
				'6':	'6',
				'7':	'7',
				'8':	'8',
				'9':	'9',
				'*':	'star',
				'#':	'sharp',
				'a':	'a',
				'o':	'o',
				't':	't',
				'T':	'tt',
				'i':	'i',
				'h':	'h'};

function xivo_ast_voicemenu_flow_display()
{
	if((from = xivo_eid('it-voicemenu-flow-hidden')) === false
	|| (to = xivo_eid('it-voicemenu-flow')) === false
	|| (from.type !== 'select-one'
	   && from.type !== 'select-multiple') === true
	|| (to.type !== 'select-one'
	   && to.type !== 'select-multiple') === true)
		return(false);

	nextoption:

	for(i = 0, j = to.options.length; i < from.options.length; i++)
	{
		for(property in xivo_ast_defapplication)
		{
			if((defapplication = xivo_ast_voicemenu_get_defapplication(property,from.options[i].value,'voicemenuflow')) === false)
				continue;

			to.options[j++] = new Option(j+'. '+defapplication['text'],defapplication['value']);

			if(from.options[i].text === 'ERR' || defapplication['error'] === true)
				to.options[j-1].className = 'fm-error';

			continue nextoption;
		}

		for(property in xivo_ast_application)
		{
			if((application = xivo_ast_voicemenu_get_application(property,from.options[i].value,'voicemenuflow')) === false)
				continue;

			to.options[j++] = new Option(j+'. '+application['text'],application['value']);

			if(from.options[i].text === 'ERR' || application['error'] === true)
				to.options[j-1].className = 'fm-error';

			continue nextoption;
		}
	}
}

function xivo_ast_voicemenu_event_display()
{
	nextevent:

	for(eventtype in xivo_ast_voicemenuevent_type)
	{
		eventname = xivo_ast_voicemenuevent_type[eventtype];

		if((itevent = xivo_eid('it-voicemenuevent-'+eventname)) === false
		|| itevent.tagName.toLowerCase() !== 'input'
		|| (infoevent = xivo_eid('voicemenuevent-'+eventname)) === false)
			continue;

		for(property in xivo_ast_defapplication)
		{
			if((defapplication = xivo_ast_voicemenu_get_defapplication(property,itevent.value,'voicemenuevent')) === false)
				continue;

			itevent.value = defapplication['value'];

			infoevent.innerHTML = '<a href="#" onclick="return(xivo.dom.free_focus());" title="'+
					      xivo_htmlsc(defapplication['text'])+'">'+
					      xivo_htmlsc(xivo_trunc(defapplication['text'],40,'...',false))+'</a>';

			if(defapplication['error'] === true)
				infoevent.parentNode.className = 'l-infos-error';

			continue nextevent;
		}

		for(property in xivo_ast_application)
		{
			if((application = xivo_ast_voicemenu_get_application(property,itevent.value,'voicemenuevent')) === false)
				continue;

			itevent.value = application['value'];

			if(application['error'] === true)
				infoevent.parentNode.className = 'l-infos-error';

			infoevent.innerHTML = '<a href="#" onclick="return(xivo.dom.free_focus());" title="'+
					      xivo_htmlsc(application['text'])+'">'+
					      xivo_htmlsc(xivo_trunc(application['text'],40,'...',false))+'</a>';

			continue nextevent;
		}
	}
}

function xivo_ast_voicemenu_get_defapplication(application,value,dialevent)
{
	if(xivo_strcasecmp(value,application,application.length) !== 0
	|| ((lastchar = value.charAt(application.length)) !== ','
	   && lastchar !== ''
	   && lastchar !== '|') === true
	|| (displayname = xivo_ast_get_defapplication_displayname(application)) === false)
		return(false);

	var r = {
		'text':		displayname,
		'value':	value.substring(0,application.length),
		'error':	false};

	if(lastchar === '')
	{
		r['text'] += '()';
		return(r);
	}

	var narg = value.substring(application.length+1);
	r['value'] += ','+narg;

	if(application === 'macro|playsound')
	{
		var pipearg = narg.split('|');

		if((spos = pipearg[0].lastIndexOf('/')) > -1)
			pipearg[0] = pipearg[0].substring(spos+1);

		narg = pipearg.join('|');
	}

	var optarg = narg.replace(/\|/g,',');

	if((identityfunc = xivo_ast_get_defapplication_identityfunc(application)) !== false)
	{
		var args = optarg.split(',');

		if((identity = identityfunc.apply(null,[dialevent,args[0]])) !== false)
		{
			args[0] = '\''+identity+'\'';
			optarg = args.join(',');
		}
		else
			r['error'] = true;
	}

	r['text'] += '('+optarg+')';

	return(r);
}

function xivo_ast_voicemenu_get_application(application,value,dialevent)
{
	if(xivo_strcasecmp(value,application,application.length) !== 0
	|| ((lastchar = value.charAt(application.length)) !== ','
	   && lastchar !== ''
	   && lastchar !== '|') === true
	|| (displayname = xivo_ast_get_application_displayname(application)) === false)
		return(false);

	var r = {
		'text':		displayname,
		'value':	value.substring(0,application.length),
		'error':	false};

	if(lastchar === '')
	{
		r['text'] += '()';
		return(r);
	}

	var narg = value.substring(application.length+1);

	if(application === 'background' || application === 'playback' || application === 'record')
	{
		var pipearg = narg.split('|');

		if((spos = pipearg[0].lastIndexOf('/')) > -1)
			pipearg[0] = pipearg[0].substring(spos+1);

		narg = pipearg.join('|');

		if(application === 'record')
			narg = narg.replace(/\|+$/,'');
	}

	r['value'] += ','+narg;
	var optarg = narg.replace(/\|/g,',');

	if((identityfunc = xivo_ast_get_application_identityfunc(application)) !== false)
	{
		var args = optarg.split(',');

		if((identity = identityfunc.apply(null,[args[0]])) !== false)
		{
			args[0] = '\''+identity+'\'';
			optarg = args.join(',');
		}
		else
			r['error'] = true;
	}

	r['text'] += '('+optarg+')';

	return(r);
}

function xivo_ast_voicemenuevent_get_eventname(eventtype)
{
	eventtype = xivo_string(eventtype);

	if(xivo_is_undef(xivo_ast_voicemenuevent_type[eventtype]) === false)
		return(xivo_ast_voicemenuevent_type[eventtype]);

	return(false);
}

function xivo_ast_voicemenuevent_defapplication(action)
{
	if((eventtype = xivo_eid('it-voicemenuevent-type')) === false
	|| (eventname = xivo_ast_voicemenuevent_get_eventname(eventtype.value)) === false)
		return(false);

	var targetid = 'voicemenuevent-'+eventname;

	if((target = xivo_eid(targetid)) === false)
		return(false);

	switch(action)
	{
		case 'none':
			if(target.type === 'text')
			{
				var r = true;
				target.value = '';
			}
			else if(xivo_is_undef(target.type) === true)
			{
				var r = true;
				target.innerHTML = '-';

				if((ittarget = xivo_eid('it-'+targetid)) !== false
				&& ittarget.tagName.toLowerCase() === 'input')
					ittarget.value = '';
			}
			else
				var r = false;

			return(r);
		case 'endcall':
			var r = xivo_ast_defapplication_endcall('voicemenuevent',targetid);
			break;
		case 'user':
			var r = xivo_ast_defapplication_user('voicemenuevent',targetid);
			break;
		case 'group':
			var r = xivo_ast_defapplication_group('voicemenuevent',targetid);
			break;
		case 'queue':
			var r = xivo_ast_defapplication_queue('voicemenuevent',targetid);
			break;
		case 'meetme':
			var r = xivo_ast_defapplication_meetme('voicemenuevent',targetid);
			break;
		case 'voicemail':
			var r = xivo_ast_defapplication_voicemail('voicemenuevent',targetid);
			break;
		case 'voicemenu':
			var r = xivo_ast_defapplication_voicemenu('voicemenuevent',targetid);
			break;
		case 'schedule':
			var r = xivo_ast_defapplication_schedule('voicemenuevent',targetid);
			break;
		case 'extension':
			var r = xivo_ast_defapplication_extension('voicemenuevent',targetid);
			break;
		case 'application':
			var r = xivo_ast_defapplication_application('voicemenuevent',targetid);
			break;
		case 'sound':
			var r = xivo_ast_defapplication_sound('voicemenuevent',targetid);
			break;
		default:
			return(false);
	}

	if(r === false)
		return(false);
	else if((html = target.innerHTML) === '-')
		return(true);

	target.innerHTML = '<a href="#" onclick="return(xivo.dom.free_focus());" title="'+
			   xivo_htmlsc(html)+'">'+
			   xivo_htmlsc(xivo_trunc(html,40,'...',false))+'</a>';

	return(true);
}

function xivo_ast_voicemenu_onload()
{
	xivo_elt_dialaction['voicemenuflow']	= {};
	xivo_elt_dialaction['voicemenuevent']	= {};
	xivo_fm_dialaction['voicemenuflow']	= {};
	xivo_fm_dialaction['voicemenuevent']	= {};

	xivo_ast_build_dialaction_array('voicemenuevent');

	xivo_dialaction_actiontype['ipbxapplication'] = ['actiontype'];
	xivo_ast_build_dialaction_array('voicemenuflow');

	xivo_ast_chg_ipbxapplication(null);

	if(xivo_is_undef(xivo_fm['fm-voicemenu']) === false)
		xivo_fm_set_disable_submit_onenter(xivo_fm['fm-voicemenu']);

	xivo_ast_dialaction_onload();
	xivo_ast_voicemenu_flow_display();
	xivo_ast_voicemenu_event_display();
}

xivo.dom.set_onload(xivo_ast_voicemenu_onload);
