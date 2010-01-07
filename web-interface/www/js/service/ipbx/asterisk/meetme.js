/*
 * XiVO Web-Interface
 * Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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

function xivo_ast_meetme_http_search_user(dwsptr)
{
	new dwho.http('/service/ipbx/ui.php/pbx_settings/meetme/users/search/?' + dwho_sess_str,
		      {'callbackcomplete':	function(xhr) { dwsptr.set(xhr,dwsptr.get_search_value()); },
		       'method':		'post',
		       'cache':			false},
		      {'search':	dwsptr.get_search_value()},
		      true);
}

var xivo_ast_meetme_suggest_user = new dwho.suggest({'requestor': xivo_ast_meetme_http_search_user});

var xivo_ast_fm_meetme_admin_typefrom_elt = {
	'it-meetmefeatures-admin-internalid':			{property:	{disabled: true}}, 
	'fd-meetme-admin-suggest':				{style:		{display: 'none'}}, 
	'it-meetme-admin-suggest':				{property:	{disabled: true}}, 
	'fd-meetmefeatures-admin-externalid':			{style:		{display: 'none'}},
	'it-meetmefeatures-admin-externalid':			{property:	{disabled: true}},
	'it-meetmeroom-pinadmin':				{property:	{disabled: false, className: 'it-enabled'}},
	'it-meetmefeatures-admin-identification':		{property:	{disabled: false, className: 'it-enabled'}},
	'it-meetmefeatures-admin-mode':				{property:	{disabled: false, className: 'it-enabled'}},
	'it-meetmefeatures-admin-announceusercount':		{property:	{disabled: false, className: 'it-enabled'}},
	'it-meetmefeatures-admin-announcejoinleave':		{property:	{disabled: false, className: 'it-enabled'}},
	'it-meetmefeatures-admin-moderationmode':		{property:	{disabled: false, className: 'it-enabled'}},
	'it-meetmefeatures-admin-initiallymuted':		{property:	{disabled: false, className: 'it-enabled'}},
	'it-meetmefeatures-admin-musiconhold':			{property:	{disabled: false, className: 'it-enabled'}},
	'it-meetmefeatures-admin-poundexit':			{property:	{disabled: false, className: 'it-enabled'}},
	'it-meetmefeatures-admin-quiet':			{property:	{disabled: false, className: 'it-enabled'}},
	'it-meetmefeatures-admin-starmenu':			{property:	{disabled: false, className: 'it-enabled'}},
	'it-meetmefeatures-admin-closeconflastmarkedexit':	{property:	{disabled: false, className: 'it-enabled'}},
	'it-meetmefeatures-admin-enableexitcontext':		{property:	{disabled: false, className: 'it-enabled'}},
	'it-meetmefeatures-admin-exitcontext':			{property:	{disabled: false, className: 'it-enabled'}},
	'links':
		{link:	[['it-meetmefeatures-admin-internalid',0,1],
			 ['fd-meetme-admin-suggest',0,1],
			 ['it-meetme-admin-suggest',0,1],
			 ['fd-meetmefeatures-admin-externalid',0,1],
			 ['it-meetmefeatures-admin-externalid',0,1],
			 ['it-meetmeroom-pinadmin',0,1],
			 ['it-meetmefeatures-admin-identification',0,1],
			 ['it-meetmefeatures-admin-mode',0,1],
			 ['it-meetmefeatures-admin-announceusercount',0,1],
			 ['it-meetmefeatures-admin-announcejoinleave',0,1],
			 ['it-meetmefeatures-admin-moderationmode',0,1],
			 ['it-meetmefeatures-admin-initiallymuted',0,1],
			 ['it-meetmefeatures-admin-musiconhold',0,1],
			 ['it-meetmefeatures-admin-poundexit',0,1],
			 ['it-meetmefeatures-admin-quiet',0,1],
			 ['it-meetmefeatures-admin-starmenu',0,1],
			 ['it-meetmefeatures-admin-closeconflastmarkedexit',0,1],
			 ['it-meetmefeatures-admin-enableexitcontext',0,1],
			 ['it-meetmefeatures-admin-exitcontext',0,1]]}
};

var xivo_ast_fm_meetme_admin_typefrom = {};

xivo_ast_fm_meetme_admin_typefrom['none'] = dwho_clone(xivo_ast_fm_meetme_admin_typefrom_elt);
xivo_ast_fm_meetme_admin_typefrom['none']
	['it-meetmefeatures-admin-identification']['property'] = {disabled: true,
								  className: 'it-disabled'};
xivo_ast_fm_meetme_admin_typefrom['none']
	['it-meetmeroom-pinadmin']['property'] = {disabled: true,
						  className: 'it-disabled'};
xivo_ast_fm_meetme_admin_typefrom['none']
	['it-meetmefeatures-admin-mode']['property'] = {disabled: true,
							className: 'it-disabled'};
xivo_ast_fm_meetme_admin_typefrom['none']
	['it-meetmefeatures-admin-announceusercount']['property'] = {disabled: true,
								     className: 'it-disabled'};
xivo_ast_fm_meetme_admin_typefrom['none']
	['it-meetmefeatures-admin-announcejoinleave']['property'] = {disabled: true,
								     className: 'it-disabled'};
xivo_ast_fm_meetme_admin_typefrom['none']
	['it-meetmefeatures-admin-moderationmode']['property'] = {disabled: true,
								  className: 'it-disabled'};
xivo_ast_fm_meetme_admin_typefrom['none']
	['it-meetmefeatures-admin-initiallymuted']['property'] = {disabled: true,
								  className: 'it-disabled'};
xivo_ast_fm_meetme_admin_typefrom['none']
	['it-meetmefeatures-admin-musiconhold']['property'] = {disabled: true,
							       className: 'it-disabled'};
xivo_ast_fm_meetme_admin_typefrom['none']
	['it-meetmefeatures-admin-poundexit']['property'] = {disabled: true,
							     className: 'it-disabled'};
xivo_ast_fm_meetme_admin_typefrom['none']
	['it-meetmefeatures-admin-quiet']['property'] = {disabled: true,
							 className: 'it-disabled'};
xivo_ast_fm_meetme_admin_typefrom['none']
	['it-meetmefeatures-admin-starmenu']['property'] = {disabled: true,
							    className: 'it-disabled'};
xivo_ast_fm_meetme_admin_typefrom['none']
	['it-meetmefeatures-admin-closeconflastmarkedexit']['property'] = {disabled: true,
									   className: 'it-disabled'};
xivo_ast_fm_meetme_admin_typefrom['none']
	['it-meetmefeatures-admin-enableexitcontext']['property'] = {disabled: true,
								     className: 'it-disabled'};
xivo_ast_fm_meetme_admin_typefrom['none']
	['it-meetmefeatures-admin-exitcontext']['property'] = {disabled: true,
							       className: 'it-disabled'};

xivo_attrib_register('ast_fm_meetme_admin_typefrom-none',
		     xivo_ast_fm_meetme_admin_typefrom['none']);

xivo_ast_fm_meetme_admin_typefrom['internal'] = dwho_clone(xivo_ast_fm_meetme_admin_typefrom_elt);
xivo_ast_fm_meetme_admin_typefrom['internal']
	['it-meetmefeatures-admin-internalid']['property'] = {disabled: false};
xivo_ast_fm_meetme_admin_typefrom['internal']
	['fd-meetme-admin-suggest']['style'] = {display: 'block'};
xivo_ast_fm_meetme_admin_typefrom['internal']
	['it-meetme-admin-suggest']['property'] = {disabled: false};

xivo_attrib_register('ast_fm_meetme_admin_typefrom-internal',
		     xivo_ast_fm_meetme_admin_typefrom['internal']);

xivo_ast_fm_meetme_admin_typefrom['external'] = dwho_clone(xivo_ast_fm_meetme_admin_typefrom_elt);
xivo_ast_fm_meetme_admin_typefrom['external']
	['fd-meetmefeatures-admin-externalid']['style'] = {display: 'block'};
xivo_ast_fm_meetme_admin_typefrom['external']
	['it-meetmefeatures-admin-externalid']['property'] = {disabled: false};

xivo_attrib_register('ast_fm_meetme_admin_typefrom-external',
		     xivo_ast_fm_meetme_admin_typefrom['external']);

xivo_ast_fm_meetme_admin_typefrom['undefined'] = dwho_clone(xivo_ast_fm_meetme_admin_typefrom_elt);
xivo_ast_fm_meetme_admin_typefrom['undefined']
	['it-meetmefeatures-admin-identification']['property'] = {disabled: true,
								  className: 'it-readonly',
								  selectedIndex: 1};

xivo_attrib_register('ast_fm_meetme_admin_typefrom-undefined',
		     xivo_ast_fm_meetme_admin_typefrom['undefined']);

var xivo_ast_fm_meetme_admin_enableexitcontext = {
	'it-meetmefeatures-admin-exitcontext':
		{property: [{disabled: true, className: 'it-disabled'},
			    {disabled: false, className: 'it-enabled'}]}};

xivo_attrib_register('ast_fm_meetme_admin_enableexitcontext',
		     xivo_ast_fm_meetme_admin_enableexitcontext);

var xivo_ast_fm_meetme_user_enableexitcontext = {
	'it-meetmefeatures-user-exitcontext':
		{property: [{disabled: true, className: 'it-disabled'},
			    {disabled: false, className: 'it-enabled'}]}};

xivo_attrib_register('ast_fm_meetme_user_enableexitcontext',
		     xivo_ast_fm_meetme_user_enableexitcontext);

function xivo_ast_meetme_chg_admin_typefrom()
{
	if((typefrom = dwho_eid('it-meetmefeatures-admin-typefrom')) !== false)
	{
		xivo_chg_attrib('ast_fm_meetme_admin_typefrom-' + typefrom.value,
				'links',
				0,
				1);

		if(typefrom.value !== 'none')
			xivo_ast_meetme_chg_admin_enableexitcontext();
	}
}

function xivo_ast_meetme_chg_admin_moderationmode()
{
	if((moderationmode = dwho_eid('it-meetmefeatures-admin-moderationmode')) === false)
		return(false);
	else if(moderationmode.checked === true)
	{
		dwho_eid('it-meetmefeatures-admin-announcejoinleave').selectedIndex = 2;
		dwho_eid('it-meetmefeatures-user-announcejoinleave').selectedIndex = 2;
	}
	else
	{
		dwho.form.reset_field(dwho_eid('it-meetmefeatures-admin-announcejoinleave'));
		dwho.form.reset_field(dwho_eid('it-meetmefeatures-user-announcejoinleave'));
	}

	return(true);
}

function xivo_ast_meetme_chg_admin_enableexitcontext()
{
	if((enableexitcontext = dwho_eid('it-meetmefeatures-admin-enableexitcontext')) !== false)
		xivo_chg_attrib('ast_fm_meetme_admin_enableexitcontext',
				'it-meetmefeatures-admin-exitcontext',
				Number(enableexitcontext.checked));
}

function xivo_ast_meetme_chg_user_enableexitcontext()
{
	if((enableexitcontext = dwho_eid('it-meetmefeatures-user-enableexitcontext')) !== false)
		xivo_chg_attrib('ast_fm_meetme_user_enableexitcontext',
				'it-meetmefeatures-user-exitcontext',
				Number(enableexitcontext.checked));
}

function xivo_ast_meetme_suggest_event_admin()
{
	xivo_ast_meetme_suggest_user.set_option(
		'result_field',
		'it-meetmefeatures-admin-internalid');
	xivo_ast_meetme_suggest_user.set_option(
		'result_onclearfield',
		function() { dwho.form.set_text_helper('it-meetme-admin-suggest',true); });

	xivo_ast_meetme_suggest_user.set_field(this.id);
}

function xivo_ast_meetme_onload()
{
	dwho.dom.add_event('change',
			   dwho_eid('it-meetmefeatures-admin-typefrom'),
			   xivo_ast_meetme_chg_admin_typefrom);

	if(dwho_has_len(xivo_fm_meetme_admin_suggest) === true)
		dwho_eid('it-meetme-admin-suggest').value = xivo_fm_meetme_admin_suggest;

	dwho_eid('it-meetme-admin-suggest').setAttribute('autocomplete','off');

	dwho.dom.add_event('focus',
			   dwho_eid('it-meetme-admin-suggest'),
			   xivo_ast_meetme_suggest_event_admin);

	dwho.form.set_events_text_helper('it-meetme-admin-suggest',true);

	dwho.dom.add_event('change',
			   dwho_eid('it-meetmefeatures-admin-moderationmode'),
			   xivo_ast_meetme_chg_admin_moderationmode);

	dwho.dom.add_event('change',
			   dwho_eid('it-meetmefeatures-admin-enableexitcontext'),
			   xivo_ast_meetme_chg_admin_enableexitcontext);

	dwho.dom.add_event('change',
			   dwho_eid('it-meetmefeatures-user-enableexitcontext'),
			   xivo_ast_meetme_chg_user_enableexitcontext);

	xivo_ast_meetme_chg_admin_typefrom();
	xivo_ast_meetme_chg_user_enableexitcontext();
}

dwho.dom.set_onload(xivo_ast_meetme_onload);
