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

var xivo_elt_phonefunckey = {};
var xivo_fm_phonefunckey = {};

function xivo_http_search_agents(dwsptr)
{
	new dwho.http('/service/ipbx/ui.php/pbx_settings/users/agents/search/?' + dwho_sess_str,
		      {'callbackcomplete':	function(xhr) { dwsptr.set(xhr,dwsptr.get_search_value()); },
		       'method':		'post',
		       'cache':			false},
		      {'search':	dwsptr.get_search_value()},
		      true);
}

var xivo_phonefunckey_suggest_agents = new dwho.suggest({'requestor': xivo_http_search_agents});

function xivo_http_search_users(dwsptr)
{
	new dwho.http('/service/ipbx/ui.php/pbx_settings/users/search/?' + dwho_sess_str,
		      {'callbackcomplete':	function(xhr) { dwsptr.set(xhr,dwsptr.get_search_value()); },
		       'method':		'post',
		       'cache':			false},
		      {'search':	dwsptr.get_search_value(),
		       'except':	dwho_eid('xivo_user_id').value},
		      true);
}

var xivo_phonefunckey_suggest_users = new dwho.suggest({'requestor': xivo_http_search_users});

function xivo_http_search_groups(dwsptr)
{
	new dwho.http('/service/ipbx/ui.php/pbx_settings/users/groups/search/?' + dwho_sess_str,
		      {'callbackcomplete':	function(xhr) { dwsptr.set(xhr,dwsptr.get_search_value()); },
		       'method':		'post',
		       'cache':			false},
		      {'search':	dwsptr.get_search_value()},
		      true);
}

var xivo_phonefunckey_suggest_groups = new dwho.suggest({'requestor': xivo_http_search_groups});

function xivo_http_search_queues(dwsptr)
{
	new dwho.http('/service/ipbx/ui.php/pbx_settings/users/queues/search/?' + dwho_sess_str,
		      {'callbackcomplete':	function(xhr) { dwsptr.set(xhr,dwsptr.get_search_value()); },
		       'method':		'post',
		       'cache':			false},
		      {'search':	dwsptr.get_search_value()},
		      true);
}

var xivo_phonefunckey_suggest_queues = new dwho.suggest({'requestor': xivo_http_search_queues});

function xivo_http_search_meetme(dwsptr)
{
	new dwho.http('/service/ipbx/ui.php/pbx_settings/users/meetme/search/?' + dwho_sess_str,
		      {'callbackcomplete':	function(xhr) { dwsptr.set(xhr,dwsptr.get_search_value()); },
		       'method':		'post',
		       'cache':			false},
		      {'search':	dwsptr.get_search_value()},
		      true);
}

var xivo_phonefunckey_suggest_meetme = new dwho.suggest({'requestor': xivo_http_search_meetme});

function xivo_phonefunckey_suggest_event_agent()
{
	if((rs = this.id.match(/^it-phonefunckey-([a-z0-9-_]+)-suggest-(\d+)$/)) === null)
		return(false);

	xivo_phonefunckey_suggest_agents.set_option(
		'result_field',
		'it-phonefunckey-' + rs[1] + '-typeval-' + rs[2]);
	xivo_phonefunckey_suggest_agents.set_field(this.id);
}

function xivo_phonefunckey_suggest_event_user()
{
	if((rs = this.id.match(/^it-phonefunckey-([a-z0-9-_]+)-suggest-(\d+)$/)) === null)
		return(false);

	xivo_phonefunckey_suggest_users.set_option(
		'result_field',
		'it-phonefunckey-' + rs[1] + '-typeval-' + rs[2]);
	xivo_phonefunckey_suggest_users.set_field(this.id);
}

function xivo_phonefunckey_suggest_event_group()
{
	if((rs = this.id.match(/^it-phonefunckey-([a-z0-9-_]+)-suggest-(\d+)$/)) === null)
		return(false);

	xivo_phonefunckey_suggest_groups.set_option(
		'result_field',
		'it-phonefunckey-' + rs[1] + '-typeval-' + rs[2]);
	xivo_phonefunckey_suggest_groups.set_field(this.id);
}

function xivo_phonefunckey_suggest_event_queue()
{
	if((rs = this.id.match(/^it-phonefunckey-([a-z0-9-_]+)-suggest-(\d+)$/)) === null)
		return(false);

	xivo_phonefunckey_suggest_queues.set_option(
		'result_field',
		'it-phonefunckey-' + rs[1] + '-typeval-' + rs[2]);
	xivo_phonefunckey_suggest_queues.set_field(this.id);
}

function xivo_phonefunckey_suggest_event_meetme()
{
	if((rs = this.id.match(/^it-phonefunckey-([a-z0-9-_]+)-suggest-(\d+)$/)) === null)
		return(false);

	xivo_phonefunckey_suggest_meetme.set_option(
		'result_field',
		'it-phonefunckey-' + rs[1] + '-typeval-' + rs[2]);
	xivo_phonefunckey_suggest_meetme.set_field(this.id);
}

var xivo_phonefunckey_suggest_type = {
	'user':					xivo_phonefunckey_suggest_event_user,
	'group':				xivo_phonefunckey_suggest_event_group,
	'queue':				xivo_phonefunckey_suggest_event_queue,
	'meetme':				xivo_phonefunckey_suggest_event_meetme,
	'extenfeatures-agentstaticlogtoggle':	xivo_phonefunckey_suggest_event_agent,
	'extenfeatures-agentstaticlogin':	xivo_phonefunckey_suggest_event_agent,
	'extenfeatures-agentstaticlogoff':	xivo_phonefunckey_suggest_event_agent,
	'extenfeatures-agentdynamiclogin':	xivo_phonefunckey_suggest_event_agent,
	'extenfeatures-grouptogglemember':	xivo_phonefunckey_suggest_event_group,
	'extenfeatures-groupaddmember':		xivo_phonefunckey_suggest_event_group,
	'extenfeatures-groupremovemember':	xivo_phonefunckey_suggest_event_group,
	'extenfeatures-queuetogglemember':	xivo_phonefunckey_suggest_event_queue,
	'extenfeatures-queueaddmember':		xivo_phonefunckey_suggest_event_queue,
	'extenfeatures-queueremovemember':	xivo_phonefunckey_suggest_event_queue};

function xivo_build_phonefunckey_array(id)
{
	if(dwho_is_undef(xivo_fm_phonefunckey[id]) === false
	&& dwho_type_object(xivo_fm_phonefunckey[id]) === true)
		return(true);

	xivo_fm_phonefunckey[id]	= {};
	xivo_elt_phonefunckey[id]	= {'links': {'link': []}};

	var i = 0;

	for(var property in xivo_phonefunckey_type)
	{
		var name = property;

		if(dwho_is_undef(xivo_phonefunckey_type[property]['extension']) === false
		&& dwho_bool(xivo_phonefunckey_type[property]['extension']) === true
		&& dwho_eid('it-phonefunckey-' + property + '-typeval') === false
		&& dwho_eid('it-phonefunckey-' + property + '-typeval-' + id) === false)
			name = 'extension';

		var keyit = 'it-phonefunckey-' + name + '-typeval-' + id;

		if(dwho_is_undef(xivo_elt_phonefunckey[id][keyit]) === false)
			continue;

		var key = 'fd-phonefunckey-' + name + '-typeval-' + id;
		xivo_elt_phonefunckey[id][key] = {'style': {display: 'none'}};
		xivo_elt_phonefunckey[id]['links']['link'][i++] = [key,0,1];

		key = 'it-phonefunckey-supervision-' + id;
		xivo_elt_phonefunckey[id][key] = {'property': {className: 'it-disabled'}};
		xivo_elt_phonefunckey[id]['links']['link'][i++] = [key,0,1];

		if(dwho_is_undef(xivo_phonefunckey_suggest_type[name]) === false)
		{
			xivo_elt_phonefunckey[id][keyit] = {'property': {disabled: true}};
			xivo_elt_phonefunckey[id]['links']['link'][i++] = [keyit,0,1];

			keyit = 'it-phonefunckey-' + name + '-suggest-' + id;
		}

		xivo_elt_phonefunckey[id][keyit] = {'style': {'display': 'none'},
						    'property':
							{readOnly:	true,
							 disabled:	true,
							 className:	'it-disabled'}};
		xivo_elt_phonefunckey[id]['links']['link'][i++] = [keyit,0,1];
	}

	for(var property in xivo_phonefunckey_type)
	{
		var name = property;
		var propertyit = {readOnly:	false,
				  disabled:	false,
				  className:	'it-enabled'};

		if(dwho_is_undef(xivo_phonefunckey_type[property]['extension']) === false
		&& dwho_bool(xivo_phonefunckey_type[property]['extension']) === true
		&& dwho_eid('it-phonefunckey-'+property+'-typeval') === false)
		{
			if(dwho_eid('it-phonefunckey-'+property+'-typeval-'+id) === false)
				name = 'extension';

			if(dwho_is_undef(xivo_phonefunckey_type[property]['destination']) === true
			|| dwho_bool(xivo_phonefunckey_type[property]['destination']) === false)
				 propertyit = {readOnly:	true,
					       disabled:	false,
					       className:	'it-readonly'};
		}

		xivo_fm_phonefunckey[id][property] = dwho_clone(xivo_elt_phonefunckey[id]);
		xivo_fm_phonefunckey[id][property]['fd-phonefunckey-' + name + '-typeval-' + id]['style'] = {display: 'inline'};

		var keyit = 'it-phonefunckey-' + name + '-typeval-' + id;

		if(dwho_is_undef(xivo_phonefunckey_suggest_type[name]) === false)
		{
			xivo_fm_phonefunckey[id][property][keyit]['property'] = {disabled: false};

			keyit = 'it-phonefunckey-' + name + '-suggest-' + id;

			dwho_eid(keyit).setAttribute('autocomplete','off');

			dwho.dom.add_event('focus',
					   dwho_eid(keyit),
					   xivo_phonefunckey_suggest_type[name]);
		}

		xivo_fm_phonefunckey[id][property][keyit]['style'] = {display: 'inline'};
		xivo_fm_phonefunckey[id][property][keyit]['property'] = propertyit;

		if(dwho_is_undef(xivo_phonefunckey_type[property]['supervisable']) === false
		&& dwho_bool(xivo_phonefunckey_type[property]['supervisable']) === true)
		{
			keyit = 'it-phonefunckey-supervision-' + id;
			xivo_fm_phonefunckey[id][property][keyit]['property'] = {className: 'it-enabled'};
		}

		xivo_attrib_register('fm_phonefunckey-' + id + '-' + property,
				     xivo_fm_phonefunckey[id][property]);
	}
}

function xivo_phonefunckey_incr_fknum(idcnt)
{
	if((curfknum = dwho_eid('it-phonefunckey-fknum-' + idcnt)) === false
	|| dwho_is_undef(curfknum.selectedIndex) === true)
		return(false);

	var prevfknum = false;

	for(var i = idcnt - 1;i > -1;i--)
	{
		if((prevfknum = dwho_eid('it-phonefunckey-fknum-' + i,true)) !== false
		&& dwho_is_undef(prevfknum.selectedIndex) === false)
		{
			if(curfknum.options.length > prevfknum.selectedIndex + 1)
				curfknum.selectedIndex = prevfknum.selectedIndex + 1;
			break;
		}
	}

	if(prevfknum === false
	&& (simultcalls = dwho_eid('it-userfeatures-simultcalls')) !== false
	&& dwho_is_undef(simultcalls.value) === false
	&& dwho_is_uint(simultcalls.value) === true)
		curfknum.selectedIndex = Number(simultcalls.value);
}

function xivo_phonefunckey_chg_type(type)
{
	if(dwho_is_undef(type.value) === true
	|| (dwho_is_undef(type.disabled) === false && type.disabled === true) === true
	|| (rs = type.id.match(/-(\d+)$/)) === null)
		return(false);

	xivo_build_phonefunckey_array(rs[1]);

	xivo_chg_attrib('fm_phonefunckey-' + rs[1] + '-' + type.value,'links',0,1);
}

function xivo_phonefunckey_add(tableobj)
{
	dwho.dom.make_table_list('phonefunckey',tableobj,0,true);
	idcnt = dwho.dom.get_table_idcnt('phonefunckey');
	xivo_phonefunckey_incr_fknum(idcnt);
	xivo_phonefunckey_chg_type(dwho_eid('it-phonefunckey-type-' + idcnt));
}

function xivo_phonefunckey_onload()
{
	if((cnt = dwho.dom.get_table_cnt('phonefunckey')) === false)
		return(false);

	for(var i = 0;i < cnt;i++)
	{
		if((eid = dwho_eid('it-phonefunckey-type-' + i)) !== false)
			xivo_phonefunckey_chg_type(eid);
	}
}

dwho.dom.set_onload(xivo_phonefunckey_onload);
