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

var xivo_ast_user_protocol_elt = {};
var xivo_ast_users_elt = {'links': {'link': []}};
var xivo_ast_fm_users = {};

var xivo_ast_users_elt_default = {
	'protocol-name': {it: true},
	'protocol-secret': {it: true},
	'protocol-interface': {it: false, fd: false},
	'protocol-protocol': {it: true},
	'protocol-context': {it: true},
	'protocol-language': {it: true, fd: true},
	'protocol-nat': {it: false, fd: false},
	'protocol-progressinband': {it: false, fd: false},
	'protocol-dtmfmode': {it: false, fd: false},
	'protocol-rfc2833compensate': {it: false, fd: false},
	'protocol-qualify': {it: false, fd: false},
	'protocol-qualifysmoothing': {it: false, fd: false},
	'protocol-qualifyfreqok': {it: false, fd: false},
	'protocol-qualifyfreqnotok': {it: false, fd: false},
	'protocol-rtptimeout': {it: false, fd: false},
	'protocol-rtpholdtimeout': {it: false, fd: false},
	'protocol-rtpkeepalive': {it: false, fd: false},
	'protocol-allowtransfer': {it: false, fd: false},
	'protocol-autoframing': {it: false, fd: false},
	'protocol-videosupport': {it: false, fd: false},
	'protocol-maxcallbitrate': {it: false, fd: false},
	'protocol-g726nonstandard': {it: false, fd: false},
	'protocol-jitterbuffer': {it: false, fd: false},
	'protocol-forcejitterbuffer': {it: false, fd: false},
	'protocol-codecpriority': {it: false, fd: false},
	'protocol-disallow': {it: true},
	'protocol-t38pt-udptl': {it: false},
	'protocol-t38pt-rtp': {it: false},
	'protocol-t38pt-tcp': {it: false},
	'protocol-t38pt-usertpsource': {it: false},
	'protocol-callerid': {it: true},
	'protocol-sendani': {it: false, fd: false},
	'protocol-insecure': {it: false, fd: false},
	'protocol-host-type': {it: true},
	'protocol-host-static': {it: true},
	'protocol-mask': {it: false, fd: false},
	'protocol-permit': {it: true},
	'protocol-deny': {it: true},
	'protocol-trustrpid': {it: false, fd: false},
	'protocol-sendrpid': {it: false, fd: false},
	'protocol-allowsubscribe': {it: false, fd: false},
	'protocol-allowoverlap': {it: false, fd: false},
	'protocol-promiscredir': {it: false, fd: false},
	'protocol-usereqphone': {it: false, fd: false},
	'protocol-canreinvite': {it: false, fd: false},
	'protocol-fromuser': {it: false, fd: false},
	'protocol-fromdomain': {it: false, fd: false},
	'protocol-maxauthreq': {it: false, fd: false},
	'protocol-adsi': {it: false, fd: false},
	'protocol-amaflags': {it: false, fd: false},
	'protocol-accountcode': {it: true},
	'protocol-useclientcode': {it: false, fd: false},

	'ufeatures-firstname': {it: true},
	'ufeatures-lastname': {it: true},
	'ufeatures-number': {it: true},
	'ufeatures-ringseconds': {it: true},
	'ufeatures-simultcalls': {it: true},
	'ufeatures-musiconhold': {it: true},
	'ufeatures-enableclient': {it: true},
	'ufeatures-loginclient': {it: true},
	'ufeatures-passwdclient': {it: true},
	'ufeatures-profileclient': {it: true},
	'ufeatures-enablehint': {it: true},
	'ufeatures-enablevoicemail': {it: true},
	'ufeatures-enablexfer': {it: true},
	'ufeatures-enableautomon': {it: true},
	'ufeatures-callrecord': {it: true},
	'ufeatures-callfilter': {it: true},
	'ufeatures-enablednd': {it: true},
	'ufeatures-enablerna': {it: true},
	'ufeatures-destrna': {it: true},
	'ufeatures-enablebusy': {it: true},
	'ufeatures-destbusy': {it: true},
	'ufeatures-enableunc': {it: true},
	'ufeatures-destunc': {it: true},
	'ufeatures-bsfilter': {it: true},
	'ufeatures-agentid': {it: true},
	'ufeatures-outcallerid-type': {it: true},
	'ufeatures-outcallerid-custom': {it: true},
	'ufeatures-preprocess-subroutine': {it: true},
	'ufeatures-description': {it: true},

	'ufeatures-voicemailid': {it: true},
	'voicemail-fullname': {it: false},
	'voicemail-mailbox': {it: false},
	'voicemail-password': {it: false},
	'voicemail-email': {it: false},
	'voicemail-tz': {it: false},
	'vmfeatures-skipcheckpass': {it: false},
	'voicemail-attach': {it: false},
	'voicemail-deletevoicemail': {it: false},
	'protocol-subscribemwi': {it: false, fd: false},
	'protocol-buggymwi': {it: false, fd: false},

	'codec-active': {it: true},
	'codeclist': {it: {style: {display: 'inline'}, property: {disabled: true, className: 'it-enabled'}}},
	'codec': {it: {style: {display: 'inline'}, property: {disabled: true, className: 'it-enabled'}}},

	'autoprov-modact': {it: false},
	'autoprov-vendormodel': {it: false},
	'autoprov-macaddr': {it: false},

	'grouplist': {it: true},
	'group': {it: true},

	'rightcalllist': {it: true},
	'rightcall': {it: true}};

var xivo_ast_fm_user_enableclient = {
	'it-ufeatures-loginclient':
		{property: [{readOnly: true, className: 'it-readonly'},
			    {readOnly: false, className: 'it-enabled'}],
		 link: 'it-ufeatures-passwdclient'},
	'it-ufeatures-passwdclient':
		{property: [{readOnly: true, className: 'it-readonly'},
			    {readOnly: false, className: 'it-enabled'}],
		 link: 'it-ufeatures-profileclient'},
	'it-ufeatures-profileclient':
		{property: [{disabled: true, className: 'it-readonly'},
			    {disabled: false, className: 'it-enabled'}]}};

xivo_attrib_register('ast_fm_user_enableclient',xivo_ast_fm_user_enableclient);

var xivo_ast_fm_user_enablerna = {
	'it-ufeatures-destrna':
		{property: [{readOnly: true, className: 'it-readonly'},
			    {readOnly: false, className: 'it-enabled'}]}};

xivo_attrib_register('ast_fm_user_enablerna',xivo_ast_fm_user_enablerna);

var xivo_ast_fm_user_enablebusy = {
	'it-ufeatures-destbusy':
		{property: [{readOnly: true, className: 'it-readonly'},
			    {readOnly: false, className: 'it-enabled'}]}};

xivo_attrib_register('ast_fm_user_enablebusy',xivo_ast_fm_user_enablebusy);

var xivo_ast_fm_user_enableunc = {
	'it-ufeatures-destunc':
		{property: [{readOnly: true, className: 'it-readonly'},
			    {readOnly: false, className: 'it-enabled'}]}};

xivo_attrib_register('ast_fm_user_enableunc',xivo_ast_fm_user_enableunc);

var xivo_ast_fm_user_outcallerid = {
	'fd-ufeatures-outcallerid-custom':
		{style: [{display: 'none'}, {display: 'block'}],
		 link: 'it-outcallerid-custom'},
	'it-ufeatures-outcallerid-custom':
		{property: [{disabled: true}, {disabled: false}]}};

xivo_attrib_register('ast_fm_user_outcallerid',xivo_ast_fm_user_outcallerid);

var xivo_ast_fm_user_host = {
	'fd-protocol-host-static':
		{style: [{display: 'none'}, {display: 'block'}],
		 link: 'it-protocol-host-static'},
	'it-protocol-host-static':
		{property: [{disabled: true}, {disabled: false}]}};

xivo_attrib_register('ast_fm_user_host',xivo_ast_fm_user_host);

var xivo_ast_fm_user_codec = {
	'it-protocol-disallow':
		{property: [{disabled: false, className: 'it-enabled'},
			    {disabled: true, className: 'it-disabled'}],
		 link: 'it-codeclist'},
	'it-codeclist':
		{property: [{disabled: false, className: 'it-enabled'},
			    {disabled: true, className: 'it-disabled', selectedIndex: -1}],
		 link: 'it-codec'},
	'it-codec':
		{property: [{disabled: false, className: 'it-enabled'},
			    {disabled: true, className: 'it-disabled', selectedIndex: -1}]}};

xivo_attrib_register('ast_fm_user_codec',xivo_ast_fm_user_codec);

var xivo_ast_fm_user_voicemail = {
	'it-voicemail-fullname':
		{property: [{disabled: false, className: 'it-enabled'},
			    {disabled: true, className: 'it-disabled'}],
		 link: 'it-voicemail-mailbox'},
	'it-voicemail-mailbox':
		{property: [{disabled: false, className: 'it-enabled'},
			    {disabled: true, className: 'it-disabled'}],
		 link: 'it-voicemail-password'},
	'it-voicemail-password':
		{property: [{disabled: false, className: 'it-enabled'},
			    {disabled: true, className: 'it-disabled'}],
		 link: 'it-voicemail-email'},
	'it-voicemail-email':
		{property: [{disabled: false, className: 'it-enabled'},
			    {disabled: true, className: 'it-disabled'}],
		 link: 'it-voicemail-tz'},
	'it-voicemail-tz':
		{property: [{disabled: false, className: 'it-enabled'},
			    {disabled: true, className: 'it-disabled'}],
		 link: 'it-vmfeatures-skipcheckpass'},
	'it-vmfeatures-skipcheckpass':
		{property: [{disabled: false, className: 'it-enabled'},
			    {disabled: true, className: 'it-disabled'}],
		 link: 'it-voicemail-attach'},
	'it-voicemail-attach':
		{property: [{disabled: false, className: 'it-enabled'},
			    {disabled: true, className: 'it-disabled'}],
		 link: 'it-voicemail-deletevoicemail'},
	'it-voicemail-deletevoicemail':
		{property: [{disabled: false, className: 'it-enabled'},
			    {disabled: true, className: 'it-disabled'}],
		 link: 'it-protocol-subscribemwi'},
	'it-protocol-subscribemwi':
		{property: [{disabled: false, className: 'it-enabled'},
			    {disabled: true, className: 'it-disabled'}],
		 link: 'it-protocol-buggymwi'},
	'it-protocol-buggymwi':
		{property: [{disabled: false, className: 'it-enabled'},
			    {disabled: true, className: 'it-disabled'}]}};

xivo_attrib_register('ast_fm_user_voicemail',xivo_ast_fm_user_voicemail);

xivo_ast_fm_user_enablevoicemail = xivo_clone(xivo_ast_fm_user_voicemail);
xivo_ast_fm_user_enablevoicemail['it-protocol-buggymwi']['link'] = 'it-ufeatures-enablevoicemail';
xivo_ast_fm_user_enablevoicemail['it-ufeatures-enablevoicemail'] = {property: [{checked: true},{checked: false}]};

xivo_attrib_register('ast_fm_user_enablevoicemail',xivo_ast_fm_user_enablevoicemail);

var xivo_ast_fm_user_autoprov = {
	'it-autoprov-modact':
		{property: [{disabled: false, className: 'it-enabled'}],
		 link: 'it-autoprov-vendormodel'},
	'it-autoprov-vendormodel':
		{property: [{disabled: false, className: 'it-enabled'},
			    {disabled: true, className: 'it-disabled'}],
		 link: 'it-autoprov-macaddr'},
	'it-autoprov-macaddr':
		{property: [{disabled: false, className: 'it-enabled'},
			    {disabled: true, className: 'it-disabled'}]}};

xivo_attrib_register('ast_fm_user_autoprov-sip',xivo_clone(xivo_ast_fm_user_autoprov));

var xivo_ast_fm_user_autoprov_iax = xivo_clone(xivo_ast_fm_user_autoprov);
xivo_ast_fm_user_autoprov_iax['it-autoprov-modact']['property'] = {disabled: true, className: 'it-disabled'};
xivo_ast_fm_user_autoprov_iax['it-autoprov-vendormodel']['property'] = {disabled: true, className: 'it-disabled'};
xivo_ast_fm_user_autoprov_iax['it-autoprov-macaddr']['property'] = {disabled: true, className: 'it-disabled'};

xivo_attrib_register('ast_fm_user_autoprov-iax',xivo_ast_fm_user_autoprov_iax);

var xivo_ast_fm_cpy_user_name = {'protocol-callerid': false, 'voicemail-fullname': false};

function xivo_ast_get_user_protocol_elt_info(protocol,element)
{
	if(xivo_type_object(xivo_ast_user_protocol_elt) === false
	|| xivo_type_object(xivo_ast_user_protocol_elt[protocol]) === false
	|| xivo_is_undef(xivo_ast_user_protocol_elt[protocol][protocol+'-'+element]) === true)
		return(false);

	return(xivo_ast_user_protocol_elt[protocol][protocol+'-'+element]);
}

function xivo_ast_build_users_elt()
{
	if(xivo_type_object(xivo_ast_users_elt_default) === false
	|| xivo_type_object(xivo_ast_users_elt) === false)
		return(false);

	var it_true = {property: {disabled: false, className: 'it-enabled'}};
	var fd_true = {style: {display: 'block'}};

	var it_false = {property: {disabled: true, className: 'it-disabled'}};
	var fd_false = {style: {display: 'none'}};

	var i = 0;

	for(property in xivo_ast_users_elt_default)
	{
		it_protocol = fd_protocol = changed = false;

		key = xivo_ast_users_elt_default[property];

		for(protocol in xivo_ast_user_protocol_elt)
		{
			protocol_elt_info = xivo_ast_get_user_protocol_elt_info(protocol,property);

			if(xivo_is_undef(key['it']) === false)
			{
				if(protocol_elt_info !== false
				&& xivo_is_undef(protocol_elt_info['it']) === false)
				{
					it_protocol = true;
					it_key = 'it-'+protocol+'-'+property;

					if(key['it'] === true)
						xivo_ast_users_elt[it_key] = xivo_clone(it_true);
					else if(key['it'] === false)
						xivo_ast_users_elt[it_key] = xivo_clone(it_false);
					else
						xivo_ast_users_elt[it_key] = xivo_clone(key['it']);

					xivo_ast_users_elt['links']['link'][i++] = new Array(it_key,0,1);
				}
			}

			if(xivo_is_undef(key['fd']) === false)
			{
				if(protocol_elt_info !== false
				&& xivo_is_undef(protocol_elt_info['fd']) === false)
				{
					fd_protocol = true;
					fd_key = 'fd-'+protocol+'-'+property;

					if(key['fd'] === true)
						xivo_ast_users_elt[fd_key] = xivo_clone(fd_true);
					else if(key['fd'] === false)
						xivo_ast_users_elt[fd_key] = xivo_clone(fd_false);
					else
						xivo_ast_users_elt[fd_key] = xivo_clone(key['fd']);

					xivo_ast_users_elt['links']['link'][i++] = new Array(fd_key,0,1);
				}
			}
		}

		if(it_protocol === false && xivo_is_undef(key['it']) === false)
		{
			changed = true;

			it_key = 'it-'+property;

			if(key['it'] === true)
				xivo_ast_users_elt[it_key] = xivo_clone(it_true);
			else if(key['it'] === false)
				xivo_ast_users_elt[it_key] = xivo_clone(it_false);
			else
				xivo_ast_users_elt[it_key] = xivo_clone(key['it']);

			xivo_ast_users_elt['links']['link'][i++] = new Array(it_key,0,1);
		}

		if(fd_protocol === false && xivo_is_undef(key['fd']) === false)
		{
			changed = true;

			fd_key = 'fd-'+property;

			if(key['fd'] === true)
				xivo_ast_users_elt[fd_key] = xivo_clone(fd_true);
			else if(key['fd'] === false)
				xivo_ast_users_elt[fd_key] = xivo_clone(fd_false);
			else
				xivo_ast_users_elt[fd_key] = xivo_clone(key['fd']);

			xivo_ast_users_elt['links']['link'][i++] = new Array(fd_key,0,1);
		}

		if(changed === false && xivo_type_object(key) === true)
		{
			xivo_ast_users_elt[property] = xivo_clone(key);
			xivo_ast_users_elt['links']['link'][i++] = new Array(property,0,1);
		}
	}
}

function xivo_ast_build_user_protocol_array(protocol)
{
	if(xivo_type_object(xivo_ast_users_elt_default) === false
	|| xivo_type_object(xivo_ast_users_elt) === false
	|| xivo_type_object(xivo_ast_user_protocol_elt[protocol]) === false)
		return(false);

	var it_true = {property: {disabled: false, className: 'it-enabled'}};
	var fd_true = {style: {display: 'block'}};

	var it_false = {property: {disabled: true, className: 'it-disabled'}};
	var fd_false = {style: {display: 'none'}};

	xivo_ast_fm_users[protocol] = xivo_clone(xivo_ast_users_elt);

	for(property in xivo_ast_user_protocol_elt[protocol])
	{
		key = xivo_ast_user_protocol_elt[protocol][property];

		if(xivo_is_undef(key['it']) === false)
		{
			it_key = 'it-'+property;

			if(key['it'] === true)
				xivo_ast_fm_users[protocol][it_key] = xivo_clone(it_true);
			else if(key['it'] === false)
				xivo_ast_fm_users[protocol][it_key] = xivo_clone(it_false);
			else
				xivo_ast_fm_users[protocol][it_key] = xivo_clone(key['it']);
		}

		if(xivo_is_undef(key['fd']) === false)
		{
			fd_key = 'fd-'+property;

			if(key['fd'] === true)
				xivo_ast_fm_users[protocol][fd_key] = xivo_clone(fd_true);
			else if(key['fd'] === false)
				xivo_ast_fm_users[protocol][fd_key] = xivo_clone(fd_false);
			else
				xivo_ast_fm_users[protocol][fd_key] = xivo_clone(key['fd']);
		}
	}

	xivo_attrib_register('ast_fm_user-'+protocol,xivo_ast_fm_users[protocol]);
}

function xivo_ast_cpy_user_name()
{
	if(xivo_eid('it-ufeatures-firstname') === false
	|| xivo_eid('it-ufeatures-lastname') === false
	|| xivo_eid('it-protocol-callerid') === false)
		return(false);

	var name = '';
	var firstname = xivo_eid('it-ufeatures-firstname').value;
	var lastname = xivo_eid('it-ufeatures-lastname').value;

	if(xivo_is_undef(firstname) === false && firstname.length > 0)
		name += firstname;

	if(xivo_is_undef(lastname) === false && lastname.length > 0)
		name += name.length === 0 ? lastname : ' '+lastname;

	var callerid = xivo_eid('it-protocol-callerid').value;

	if(xivo_is_undef(callerid) === true || callerid.length === 0)
		callerid = '';
	else
		callerid = callerid.replace(/^(?:"(.+)"|([^"]+))\s*<[^<]*>$/,'\$1');

	if(callerid.length === 0 || callerid === name)
		xivo_ast_fm_cpy_user_name['protocol-callerid'] = true;
	else
		xivo_ast_fm_cpy_user_name['protocol-callerid'] = false;

	if(xivo_eid('it-voicemail-fullname') === false)
		return(false);

	var fullname = xivo_eid('it-voicemail-fullname').value;

	if(xivo_is_undef(fullname) === true || fullname === name || fullname.length === 0)
		xivo_ast_fm_cpy_user_name['voicemail-fullname'] = true;
	else
		xivo_ast_fm_cpy_user_name['voicemail-fullname'] = false;
}

function xivo_ast_chg_user_name()
{
	if(xivo_ast_fm_cpy_user_name['protocol-callerid'] === false
	&& xivo_ast_fm_cpy_user_name['voicemail-fullname'] === false)
		return(false);

	var name = '';
	var firstname = xivo_eid('it-ufeatures-firstname').value;
	var lastname = xivo_eid('it-ufeatures-lastname').value;

	if(xivo_is_undef(firstname) === false && firstname.length > 0)
		name += firstname;

	if(xivo_is_undef(lastname) === false && lastname.length > 0)
		name += name.length === 0 ? lastname : ' '+lastname;

	if(xivo_ast_fm_cpy_user_name['protocol-callerid'] === true)
		xivo_eid('it-protocol-callerid').value = name;

	if(xivo_ast_fm_cpy_user_name['voicemail-fullname'] === true)
		xivo_eid('it-voicemail-fullname').value = name;

	return(true);
}

function xivo_ast_chg_user_protocol(protocol)
{
	if(xivo_is_undef(xivo_ast_user_protocol_elt[protocol]) === true)
		return(false);

	xivo_ast_user_protocol = protocol;

	if(xivo_is_undef(xivo_ast_fm_users[xivo_ast_user_protocol]) === true)
		xivo_ast_build_user_protocol_array(xivo_ast_user_protocol);

	xivo_chg_attrib('ast_fm_user-'+xivo_ast_user_protocol,'links',0,1);

	if((voicemail = xivo_eid('it-ufeatures-voicemailid')) !== false
	&& voicemail.value !== ''
	&& voicemail.disabled === false)
		xivo_chg_attrib('ast_fm_user_voicemail','it-voicemail-fullname',0);

	if((host_type = xivo_eid('it-protocol-host-type')) !== false)
		xivo_chg_attrib('ast_fm_user_host',
				'fd-protocol-host-static',
				Number(host_type.value === 'static'));

	if((autoprov_modact = xivo_eid('it-autoprov-modact')) !== false)
		xivo_chg_attrib('ast_fm_user_autoprov-'+xivo_ast_user_protocol,
				'it-autoprov-modact',
				Number(autoprov_modact.value === ''));

	if((codec_active = xivo_eid('it-codec-active')) !== false)
		xivo_chg_attrib('ast_fm_user_codec',
				'it-protocol-disallow',
				Number(codec_active.checked === false));
}

function xivo_ast_user_ingroup()
{
	xivo_fm_move_selected('it-grouplist','it-group');

	if((grouplist = xivo_eid('it-group')) === false || (len = grouplist.length) < 1)
		return(false);

	for(i = 0;i < len;i++)
	{
		if((group = xivo_eid('group-'+grouplist[i].value)) !== false)
			group.style.display = 'table-row';
	}

	if(xivo_eid('it-group').length > 0)
		xivo_eid('no-group').style.display = 'none';

	return(true);
}

function xivo_ast_user_outgroup()
{
	xivo_fm_move_selected('it-group','it-grouplist');

	if((grouplist = xivo_eid('it-grouplist')) === false || (len = grouplist.length) < 1)
		return(false);

	for(i = 0;i < len;i++)
	{
		if((group = xivo_eid('group-'+grouplist[i].value)) !== false)
			group.style.display = 'none';
	}

	if(xivo_eid('it-group').length === 0)
		xivo_eid('no-group').style.display = 'table-row';

	return(true);
}

function xivo_ast_user_voicemail_selection(value)
{
	if(value.length === 0)
	{
		xivo_chg_attrib('ast_fm_user_enablevoicemail',
				'it-voicemail-fullname',
				1);
		return(true);
	}

	xivo_chg_attrib('ast_fm_user_enablevoicemail',
			'it-voicemail-fullname',
			0);

	if(value === 'add')
	{
		if(typeof(xivo_ast_fm_user_voicemail) === 'undefined'
		|| xivo_type_object(xivo_ast_fm_user_voicemail) === false)
			return(false);

		for(property in xivo_ast_fm_user_voicemail)
			xivo_fm_reset_field(xivo_eid(property),true);

		if(xivo_eid('it-ufeatures-firstname') === false
		|| xivo_eid('it-ufeatures-lastname') === false
		|| xivo_eid('it-voicemail-fullname') === false)
			return(true);

		var name = '';
		var firstname = xivo_eid('it-ufeatures-firstname').value;
		var lastname = xivo_eid('it-ufeatures-lastname').value;

		if(xivo_is_undef(firstname) === false)
			name += firstname;

		if(xivo_is_undef(lastname) === false)
			name += name.length === 0 ? lastname : ' '+lastname;

		xivo_eid('it-voicemail-fullname').value = name;

		return(true);
	}

	var param = {'act': 'view',
		     'id': value,
		     'callback': 'xivo_ast_user_voicemail_set_info'};

	if(xivo_ajs_load_script('/service/ipbx/ajs.php/voicemail/',
				param,
				'ipbx-vm-view',
				true) === false);
		return(false);

	return(true);
}

function xivo_ast_user_voicemail_set_info(obj)
{
	if(xivo_is_undef(obj['voicemail']) === true)
		return(false);

	xivo_eid('it-voicemail-fullname').value = obj['voicemail']['fullname'];
	xivo_eid('it-voicemail-mailbox').value = obj['voicemail']['mailbox'];
	xivo_eid('it-voicemail-password').value = obj['voicemail']['password'];
	xivo_eid('it-voicemail-email').value = obj['voicemail']['email'];
	xivo_eid('it-voicemail-tz').value = obj['voicemail']['tz'];
	xivo_eid('it-vmfeatures-skipcheckpass').checked = xivo_bool(obj['vmfeatures']['skipcheckpass']);
	xivo_eid('it-voicemail-attach').value = obj['voicemail']['attach'];
	xivo_eid('it-voicemail-deletevoicemail').checked = xivo_bool(obj['voicemail']['deletevoicemail']);
}

function xivo_ast_user_enable_voicemail()
{
	if((voicemail = xivo_eid('it-ufeatures-voicemailid')) !== false)
		xivo_chg_attrib('ast_fm_user_enablevoicemail',
				'it-voicemail-fullname',
				Number(voicemail.value === ''));
}

function xivo_ast_user_onload()
{
	xivo_ast_build_users_elt();

	if(xivo_eid('it-protocol-protocol') !== false)
		xivo_ast_chg_user_protocol(xivo_eid('it-protocol-protocol').value);

	if((voicemail = xivo_eid('it-ufeatures-voicemailid')) !== false)
		xivo_chg_attrib('ast_fm_user_voicemail',
				'it-voicemail-fullname',
				Number(voicemail.value === ''));

	if((outcallerid_type = xivo_eid('it-ufeatures-outcallerid-type')) !== false)
		xivo_chg_attrib('ast_fm_user_outcallerid',
				'fd-ufeatures-outcallerid-custom',
				Number(outcallerid_type.value === 'custom'));

	if((enableclient = xivo_eid('it-ufeatures-enableclient')) !== false)
		xivo_chg_attrib('ast_fm_user_enableclient',
				'it-ufeatures-loginclient',
				Number(enableclient.checked));

	if((enablerna = xivo_eid('it-ufeatures-enablerna')) !== false)
		xivo_chg_attrib('ast_fm_user_enablerna',
				'it-ufeatures-destrna',
				Number(enablerna.checked));

	if((enablebusy = xivo_eid('it-ufeatures-enablebusy')) !== false)
		xivo_chg_attrib('ast_fm_user_enablebusy',
				'it-ufeatures-destbusy',
				Number(enablebusy.checked));

	if((enableunc = xivo_eid('it-ufeatures-enableunc')) !== false)
		xivo_chg_attrib('ast_fm_user_enableunc',
				'it-ufeatures-destunc',
				Number(enableunc.checked));

	xivo_ast_build_dialaction_array('noanswer');
	xivo_ast_build_dialaction_array('busy');
	xivo_ast_build_dialaction_array('congestion');
	xivo_ast_build_dialaction_array('chanunavail');

	xivo_ast_dialaction_onload();
}

xivo_winload.push('xivo_ast_user_onload();');
