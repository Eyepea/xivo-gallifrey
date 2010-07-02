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

var xivo_ast_user_protocol = null;
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
	'protocol-park': {it: false, fd: false},
	'protocol-cfwdall': {it: false, fd: false},
	'protocol-cfwdbusy': {it: false, fd: false},
	'protocol-cfwdnoanswer': {it: false, fd: false},
	'protocol-pickupexten': {it: false, fd: false},
	'protocol-pickupcontext': {it: false, fd: false},
	'protocol-pickupmodeanswer': {it: false, fd: false},
	'protocol-dnd': {it: false, fd: false},
	'protocol-directrtp': {it: false, fd: false},
	'protocol-earlyrtp': {it: false, fd: false},
	'protocol-private': {it: false, fd: false},
	'protocol-privacy': {it: false, fd: false},
	'protocol-mwilamp': {it: false, fd: false},
	'protocol-mwioncall': {it: false, fd: false},
	'protocol-echocancel': {it: false, fd: false},
	'protocol-silencesuppression': {it: false, fd: false},
	'protocol-incominglimit': {it: false, fd: false},
	'protocol-keepalive': {it: false, fd: false},
	'protocol-tzoffset': {it: false, fd: false},
	'protocol-imageversion': {it: false, fd: false},
	'protocol-trustphoneip': {it: false, fd: false},
	'protocol-secondary_dialtone_digits': {it: false, fd: false},
	'protocol-secondary_dialtone_tone': {it: false, fd: false},
	'protocol-audio_tos': {it: false, fd: false},
	'protocol-audio_cos': {it: false, fd: false},
	'protocol-video_tos': {it: false, fd: false},
	'protocol-video_cos': {it: false, fd: false},
	'protocol-adhocnumber': {it: false, fd: false},
	'protocol-requirecalltoken': {it: false, fd: false},

	
	'userfeatures-firstname': {it: true},
	'userfeatures-lastname': {it: true},
	'userfeatures-number': {it: true},
	'userfeatures-ringseconds': {it: true},
	'userfeatures-simultcalls': {it: true},
	'userfeatures-musiconhold': {it: true},
	'userfeatures-enableclient': {it: true},
	'userfeatures-loginclient': {it: true},
	'userfeatures-passwdclient': {it: true},
	'userfeatures-profileclient': {it: true},
	'userfeatures-enablehint': {it: true},
	'userfeatures-enablevoicemail': {it: true},
	'userfeatures-enablexfer': {it: true},
	'userfeatures-enableautomon': {it: true},
	'userfeatures-callrecord': {it: true},
	'userfeatures-incallfilter': {it: true},
	'userfeatures-enablednd': {it: true},
	'userfeatures-enablerna': {it: true},
	'userfeatures-destrna': {it: true},
	'userfeatures-enablebusy': {it: true},
	'userfeatures-destbusy': {it: true},
	'userfeatures-enableunc': {it: true},
	'userfeatures-destunc': {it: true},
	'userfeatures-bsfilter': {it: true},
	'userfeatures-agentid': {it: true},
	'userfeatures-outcallerid-type': {it: true},
	'userfeatures-outcallerid-custom': {it: true},
	'userfeatures-preprocess-subroutine': {it: true},
	'userfeatures-description': {it: true},

	'userfeatures-voicemailid': {it: true, fd: false},
	'voicemail-option': {it: true},
	'voicemail-suggest': {it: false, fd: false},
	'voicemail-fullname': {it: false},
	'voicemail-mailbox': {it: false},
	'voicemail-password': {it: false},
	'voicemail-email': {it: false},
	'voicemail-tz': {it: false},
	'voicemailfeatures-skipcheckpass': {it: false},
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

var xivo_ast_fm_user_pickupcontext = {
	'fd-protocol-pickupcontext':
		{style: [{display: 'none'}, {display: 'block'}],
		 link: 'it-protocol-pickupcontext'},
	'it-protocol-pickupcontext':
		{property: [{disabled: true}, {disabled: false}]}};

xivo_attrib_register('ast_fm_user_pickupcontext',xivo_ast_fm_user_pickupcontext);


var xivo_ast_fm_user_enableclient = {
	'it-userfeatures-loginclient':
		{property: [{readOnly: true, className: 'it-readonly'},
			    {readOnly: false, className: 'it-enabled'}],
		 link: 'it-userfeatures-passwdclient'},
	'it-userfeatures-passwdclient':
		{property: [{readOnly: true, className: 'it-readonly'},
			    {readOnly: false, className: 'it-enabled'}],
		 link: 'it-userfeatures-profileclient'},
	'it-userfeatures-profileclient':
		{property: [{disabled: true, className: 'it-readonly'},
			    {disabled: false, className: 'it-enabled'}]}};

xivo_attrib_register('ast_fm_user_enableclient',xivo_ast_fm_user_enableclient);

var xivo_ast_fm_user_enablerna = {
	'it-userfeatures-destrna':
		{property: [{readOnly: true, className: 'it-readonly'},
			    {readOnly: false, className: 'it-enabled'}]}};

xivo_attrib_register('ast_fm_user_enablerna',xivo_ast_fm_user_enablerna);

var xivo_ast_fm_user_enablebusy = {
	'it-userfeatures-destbusy':
		{property: [{readOnly: true, className: 'it-readonly'},
			    {readOnly: false, className: 'it-enabled'}]}};

xivo_attrib_register('ast_fm_user_enablebusy',xivo_ast_fm_user_enablebusy);

var xivo_ast_fm_user_enableunc = {
	'it-userfeatures-destunc':
		{property: [{readOnly: true, className: 'it-readonly'},
			    {readOnly: false, className: 'it-enabled'}]}};

xivo_attrib_register('ast_fm_user_enableunc',xivo_ast_fm_user_enableunc);

var xivo_ast_fm_user_voicemailoption = {
	'fd-voicemail-suggest':
		{style: [{display: 'none'}, {display: 'block'}],
		 link: 'it-voicemail-suggest'},
	'it-voicemail-suggest':
		{property: [{disabled: true, className: 'it-disabled'},
			    {disabled: false, className: 'it-enabled'}]}};

xivo_attrib_register('ast_fm_user_voicemailoption',xivo_ast_fm_user_voicemailoption);

var xivo_ast_fm_user_outcallerid = {
	'fd-userfeatures-outcallerid-custom':
		{style: [{display: 'none'}, {display: 'block'}],
		 link: 'it-outcallerid-custom'},
	'it-userfeatures-outcallerid-custom':
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
		 link: 'it-voicemailfeatures-skipcheckpass'},
	'it-voicemailfeatures-skipcheckpass':
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

var xivo_ast_fm_user_enablevoicemail = dwho_clone(xivo_ast_fm_user_voicemail);
xivo_ast_fm_user_enablevoicemail['it-protocol-buggymwi']['link'] = 'it-userfeatures-enablevoicemail';
xivo_ast_fm_user_enablevoicemail['it-userfeatures-enablevoicemail'] = {property: [{checked: true},{checked: false}]};

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

xivo_attrib_register('ast_fm_user_autoprov-sip',dwho_clone(xivo_ast_fm_user_autoprov));

var xivo_ast_fm_user_autoprov_iax = dwho_clone(xivo_ast_fm_user_autoprov);
xivo_ast_fm_user_autoprov_iax['it-autoprov-modact']['property'] = {disabled: true, className: 'it-disabled'};
xivo_ast_fm_user_autoprov_iax['it-autoprov-vendormodel']['property'] = {disabled: true, className: 'it-disabled'};
xivo_ast_fm_user_autoprov_iax['it-autoprov-macaddr']['property'] = {disabled: true, className: 'it-disabled'};

xivo_attrib_register('ast_fm_user_autoprov-iax',xivo_ast_fm_user_autoprov_iax);

var xivo_ast_fm_cpy_user_name = {'protocol-callerid': false, 'voicemail-fullname': false};

function xivo_ast_get_user_protocol_elt_info(protocol,element)
{
	if(dwho_type_object(xivo_ast_user_protocol_elt) === false
	|| dwho_type_object(xivo_ast_user_protocol_elt[protocol]) === false
	|| dwho_is_undef(xivo_ast_user_protocol_elt[protocol][protocol+'-'+element]) === true)
		return(false);

	return(xivo_ast_user_protocol_elt[protocol][protocol+'-'+element]);
}

function xivo_ast_build_users_elt()
{
	if(dwho_type_object(xivo_ast_users_elt_default) === false
	|| dwho_type_object(xivo_ast_users_elt) === false)
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

			if(dwho_is_undef(key['it']) === false)
			{
				if(protocol_elt_info !== false
				&& dwho_is_undef(protocol_elt_info['it']) === false)
				{
					it_protocol = true;
					it_key = 'it-'+protocol+'-'+property;

					if(key['it'] === true)
						xivo_ast_users_elt[it_key] = dwho_clone(it_true);
					else if(key['it'] === false)
						xivo_ast_users_elt[it_key] = dwho_clone(it_false);
					else
						xivo_ast_users_elt[it_key] = dwho_clone(key['it']);

					xivo_ast_users_elt['links']['link'][i++] = [it_key,0,1];
				}
			}

			if(dwho_is_undef(key['fd']) === false)
			{
				if(protocol_elt_info !== false
				&& dwho_is_undef(protocol_elt_info['fd']) === false)
				{
					fd_protocol = true;
					fd_key = 'fd-'+protocol+'-'+property;

					if(key['fd'] === true)
						xivo_ast_users_elt[fd_key] = dwho_clone(fd_true);
					else if(key['fd'] === false)
						xivo_ast_users_elt[fd_key] = dwho_clone(fd_false);
					else
						xivo_ast_users_elt[fd_key] = dwho_clone(key['fd']);

					xivo_ast_users_elt['links']['link'][i++] = [fd_key,0,1];
				}
			}
		}

		if(it_protocol === false && dwho_is_undef(key['it']) === false)
		{
			changed = true;

			it_key = 'it-'+property;

			if(key['it'] === true)
				xivo_ast_users_elt[it_key] = dwho_clone(it_true);
			else if(key['it'] === false)
				xivo_ast_users_elt[it_key] = dwho_clone(it_false);
			else
				xivo_ast_users_elt[it_key] = dwho_clone(key['it']);

			xivo_ast_users_elt['links']['link'][i++] = [it_key,0,1];
		}

		if(fd_protocol === false && dwho_is_undef(key['fd']) === false)
		{
			changed = true;

			fd_key = 'fd-'+property;

			if(key['fd'] === true)
				xivo_ast_users_elt[fd_key] = dwho_clone(fd_true);
			else if(key['fd'] === false)
				xivo_ast_users_elt[fd_key] = dwho_clone(fd_false);
			else
				xivo_ast_users_elt[fd_key] = dwho_clone(key['fd']);

			xivo_ast_users_elt['links']['link'][i++] = [fd_key,0,1];
		}

		if(changed === false && dwho_type_object(key) === true)
		{
			xivo_ast_users_elt[property] = dwho_clone(key);
			xivo_ast_users_elt['links']['link'][i++] = [property,0,1];
		}
	}
}

function xivo_ast_build_user_protocol_array(protocol)
{
	if(dwho_type_object(xivo_ast_users_elt_default) === false
	|| dwho_type_object(xivo_ast_users_elt) === false
	|| dwho_type_object(xivo_ast_user_protocol_elt[protocol]) === false)
		return(false);

	var it_true = {property: {disabled: false, className: 'it-enabled'}};
	var fd_true = {style: {display: 'block'}};

	var it_false = {property: {disabled: true, className: 'it-disabled'}};
	var fd_false = {style: {display: 'none'}};

	xivo_ast_fm_users[protocol] = dwho_clone(xivo_ast_users_elt);

	for(property in xivo_ast_user_protocol_elt[protocol])
	{
		key = xivo_ast_user_protocol_elt[protocol][property];

		if(dwho_is_undef(key['it']) === false)
		{
			it_key = 'it-'+property;

			if(key['it'] === true)
				xivo_ast_fm_users[protocol][it_key] = dwho_clone(it_true);
			else if(key['it'] === false)
				xivo_ast_fm_users[protocol][it_key] = dwho_clone(it_false);
			else
				xivo_ast_fm_users[protocol][it_key] = dwho_clone(key['it']);
		}

		if(dwho_is_undef(key['fd']) === false)
		{
			fd_key = 'fd-'+property;

			if(key['fd'] === true)
				xivo_ast_fm_users[protocol][fd_key] = dwho_clone(fd_true);
			else if(key['fd'] === false)
				xivo_ast_fm_users[protocol][fd_key] = dwho_clone(fd_false);
			else
				xivo_ast_fm_users[protocol][fd_key] = dwho_clone(key['fd']);
		}
	}

	xivo_attrib_register('ast_fm_user-'+protocol,xivo_ast_fm_users[protocol]);
}

function xivo_ast_user_cpy_name()
{
	if(dwho_eid('it-userfeatures-firstname') === false
	|| dwho_eid('it-userfeatures-lastname') === false
	|| dwho_eid('it-protocol-callerid') === false)
		return(false);

	var name = '';
	var firstname = dwho_eid('it-userfeatures-firstname').value;
	var lastname = dwho_eid('it-userfeatures-lastname').value;

	if(dwho_is_undef(firstname) === false && firstname.length > 0)
		name += firstname;

	if(dwho_is_undef(lastname) === false && lastname.length > 0)
		name += name.length === 0 ? lastname : ' '+lastname;

	var callerid = dwho_eid('it-protocol-callerid').value;

	if(dwho_is_undef(callerid) === true || callerid.length === 0)
		callerid = '';
	else
		callerid = callerid.replace(/^(?:"(.+)"|([^"]+))\s*<[^<]*>$/,'\$1');

	if(callerid.length === 0 || callerid === name)
		xivo_ast_fm_cpy_user_name['protocol-callerid'] = true;
	else
		xivo_ast_fm_cpy_user_name['protocol-callerid'] = false;

	if(dwho_eid('it-voicemail-fullname') === false)
		return(false);

	var fullname = dwho_eid('it-voicemail-fullname').value;

	if(dwho_is_undef(fullname) === true || fullname === name || fullname.length === 0)
		xivo_ast_fm_cpy_user_name['voicemail-fullname'] = true;
	else
		xivo_ast_fm_cpy_user_name['voicemail-fullname'] = false;
}

function xivo_ast_user_chg_name()
{
	if(xivo_ast_fm_cpy_user_name['protocol-callerid'] === false
	&& xivo_ast_fm_cpy_user_name['voicemail-fullname'] === false)
		return(false);

	var name = '';
	var firstname = dwho_eid('it-userfeatures-firstname').value;
	var lastname = dwho_eid('it-userfeatures-lastname').value;

	if(dwho_is_undef(firstname) === false && firstname.length > 0)
		name += firstname;

	if(dwho_is_undef(lastname) === false && lastname.length > 0)
		name += name.length === 0 ? lastname : ' '+lastname;

	if(xivo_ast_fm_cpy_user_name['protocol-callerid'] === true)
		dwho_eid('it-protocol-callerid').value = name;

	if(xivo_ast_fm_cpy_user_name['voicemail-fullname'] === true)
		dwho_eid('it-voicemail-fullname').value = name;

	return(true);
}

function xivo_ast_user_chg_host_type()
{
	if((host_type = dwho_eid('it-protocol-host-type')) !== false)
		xivo_chg_attrib('ast_fm_user_host',
				'fd-protocol-host-static',
				Number(host_type.value === 'static'));
}

function xivo_ast_user_chg_autoprov_modact()
{
	if(xivo_ast_user_protocol !== null
	&& (autoprov_modact = dwho_eid('it-autoprov-modact')) !== false)
		xivo_chg_attrib('ast_fm_user_autoprov-'+xivo_ast_user_protocol,
				'it-autoprov-modact',
				Number(autoprov_modact.value === ''));
}

function xivo_ast_user_chg_enableclient()
{
	if(xivo_ast_user_protocol !== 'custom'
	&& (enableclient = dwho_eid('it-userfeatures-enableclient')) !== false)
		xivo_chg_attrib('ast_fm_user_enableclient',
				'it-userfeatures-loginclient',
				Number(enableclient.checked));
}

function xivo_ast_user_chg_enablerna()
{
	if(xivo_ast_user_protocol !== 'custom'
	&& (enablerna = dwho_eid('it-userfeatures-enablerna')) !== false)
		xivo_chg_attrib('ast_fm_user_enablerna',
				'it-userfeatures-destrna',
				Number(enablerna.checked));
}

function xivo_ast_user_chg_enablebusy()
{
	if(xivo_ast_user_protocol !== 'custom'
	&& (enablebusy = dwho_eid('it-userfeatures-enablebusy')) !== false)
		xivo_chg_attrib('ast_fm_user_enablebusy',
				'it-userfeatures-destbusy',
				Number(enablebusy.checked));
}

function xivo_ast_user_chg_enableunc()
{
	if(xivo_ast_user_protocol !== 'custom'
	&& (enableunc = dwho_eid('it-userfeatures-enableunc')) !== false)
		xivo_chg_attrib('ast_fm_user_enableunc',
				'it-userfeatures-destunc',
				Number(enableunc.checked));
}

function xivo_ast_user_chg_protocol(protocol)
{
	if(dwho_is_undef(xivo_ast_user_protocol_elt[protocol]) === true)
		return(false);

	xivo_ast_user_protocol = protocol;

	if(dwho_is_undef(xivo_ast_fm_users[xivo_ast_user_protocol]) === true)
		xivo_ast_build_user_protocol_array(xivo_ast_user_protocol);

	xivo_chg_attrib('ast_fm_user-'+xivo_ast_user_protocol,'links',0,1);

	if(xivo_ast_user_protocol !== 'custom'
	&& (voicemail_option = dwho_eid('it-voicemail-option')) !== false)
	{
		if(voicemail_option.value === 'add')
			xivo_chg_attrib('ast_fm_user_enablevoicemail',
					'it-voicemail-fullname',
					0);
		else
			xivo_ast_user_chg_voicemail(voicemail_option.value);
	}

	xivo_ast_user_chg_host_type();
	xivo_ast_user_chg_autoprov_modact();
	xivo_ast_user_chg_enableclient();
	xivo_ast_user_chg_enablerna();
	xivo_ast_user_chg_enablebusy();
	xivo_ast_user_chg_enableunc();

	if((codec_active = dwho_eid('it-codec-active')) !== false)
		xivo_chg_attrib('ast_fm_user_codec',
				'it-protocol-disallow',
				Number(codec_active.checked === false));

	dwho_eid('sb-list-addons').style.display = 'none';
	dwho_eid('fld-softkeys').style.display   = 'none';

	if(xivo_ast_user_protocol === 'sccp')
	{
		pickupexten = dwho_eid('it-protocol-pickupexten');
		xivo_chg_attrib('ast_fm_user_pickupcontext',
			'fd-protocol-pickupcontext',
			Number(pickupexten.value === 'on')
		);

		dwho_eid('sb-list-addons').style.display = 'block';
		dwho_eid('fld-softkeys').style.display   = 'block';
	}
}

function xivo_ast_user_ingroup()
{
	dwho.form.move_selected('it-grouplist','it-group');

	if((grouplist = dwho_eid('it-group')) === false || (len = grouplist.length) < 1)
		return(false);

	for(i = 0;i < len;i++)
	{
		if((group = dwho_eid('group-'+grouplist[i].value)) !== false)
			group.style.display = 'table-row';
	}

	if(dwho_eid('it-group').length > 0)
		dwho_eid('no-group').style.display = 'none';

	return(true);
}

function xivo_ast_user_outgroup()
{
	dwho.form.move_selected('it-group','it-grouplist');

	if((grouplist = dwho_eid('it-grouplist')) === false || (len = grouplist.length) < 1)
		return(false);

	for(i = 0;i < len;i++)
	{
		if((group = dwho_eid('group-'+grouplist[i].value)) !== false)
			group.style.display = 'none';
	}

	if(dwho_eid('it-group').length === 0)
		dwho_eid('no-group').style.display = 'table-row';

	return(true);
}

function xivo_ast_user_http_search_voicemail(dwsptr)
{
	new dwho.http('/service/ipbx/ui.php/pbx_settings/users/voicemail/search/?' + dwho_sess_str,
		      {'callbackcomplete':	function(xhr) { dwsptr.set(xhr,dwsptr.get_search_value()); },
		       'method':		'post',
		       'cache':			false},
		      {'search':	dwsptr.get_search_value()},
		      true);
}

var xivo_ast_user_suggest_voicemail = new dwho.suggest({'requestor': xivo_ast_user_http_search_voicemail});

function xivo_ast_user_voicemail_reset_search()
{
	dwho.form.reset_field(dwho_eid('it-userfeatures-voicemailid'),true);

	for(property in xivo_ast_fm_user_voicemail)
		dwho.form.reset_field(dwho_eid(property),true);

	xivo_chg_attrib('ast_fm_user_enablevoicemail',
			'it-voicemail-fullname',
			0);
}

function xivo_ast_user_chg_voicemail(option)
{
	xivo_chg_attrib('ast_fm_user_voicemailoption',
			'fd-voicemail-suggest',
			Number(option === 'search'));

	switch(option)
	{
		case 'add':
			if((voicemail_option = dwho_eid('it-voicemail-option')) !== false)
				reset_field_empty = voicemail_option.defaultValue !== option;
			else
				reset_field_empty = true;

			for(property in xivo_ast_fm_user_voicemail)
				dwho.form.reset_field(dwho_eid(property),reset_field_empty);

			xivo_chg_attrib('ast_fm_user_enablevoicemail',
					'it-voicemail-fullname',
					0);

			if(dwho_eid('it-userfeatures-firstname') === false
			|| dwho_eid('it-userfeatures-lastname') === false
			|| dwho_eid('it-voicemail-fullname') === false)
				break;

			var name = '';
			var firstname = dwho_eid('it-userfeatures-firstname').value;
			var lastname = dwho_eid('it-userfeatures-lastname').value;

			if(dwho_has_len(firstname) === true)
				name += firstname;

			if(dwho_has_len(lastname) === true)
				name += name.length === 0 ? lastname : ' '+lastname;

			dwho_eid('it-voicemail-fullname').value = name;
			break;
		case 'search':
			dwho_eid('it-voicemail-suggest').value = '';
			xivo_ast_user_voicemail_reset_search();
			break;
		case 'none':
		default:
			dwho.form.reset_field(dwho_eid('it-userfeatures-voicemailid'),false);

			for(property in xivo_ast_fm_user_enablevoicemail)
				dwho.form.reset_field(dwho_eid(property),false);

			xivo_chg_attrib('ast_fm_user_enablevoicemail',
					'it-voicemail-fullname',
					Number((option === 'none'
					        || dwho_eid('it-userfeatures-voicemailid').value === '')));
	}
}

function xivo_ast_user_http_get_voicemail(obj)
{
	if(dwho_is_object(obj) === false
	|| dwho_has_len(obj.value) === false)
	{
		xivo_chg_attrib('ast_fm_user_voicemailoption',
				'fd-voicemail-suggest',
				1);

		return(xivo_ast_user_voicemail_reset_search());
	}

	new dwho.http('/service/ipbx/ui.php/pbx_settings/users/voicemail/view/?' + dwho_sess_str,
		      {'callbackcomplete':	function(xhr) { xivo_ast_user_voicemail_set_info(xhr); },
		       'method':		'post',
		       'cache':			false},
		      {'id':		obj.value},
		       true);

	return(true);
}

function xivo_ast_user_suggest_event_voicemail()
{
	xivo_ast_user_suggest_voicemail.set_option(
		'result_field',
		'it-userfeatures-voicemailid');
	xivo_ast_user_suggest_voicemail.set_option(
		'result_onsetfield',
		xivo_ast_user_http_get_voicemail);

	xivo_ast_user_suggest_voicemail.set_field(this.id);
}

function xivo_ast_user_voicemail_set_info(request)
{
	if(dwho_has_len(request.responseText) === false)
		return(null);

	obj = eval('(' + request.responseText + ')');

	if(dwho_is_object(obj.voicemail) === false)
		return(false);

	dwho_eid('it-voicemail-fullname').value			= obj['voicemail']['fullname'];
	dwho_eid('it-voicemail-mailbox').value			= obj['voicemail']['mailbox'];
	dwho_eid('it-voicemail-password').value			= obj['voicemail']['password'];
	dwho_eid('it-voicemail-email').value			= obj['voicemail']['email'];
	dwho_eid('it-voicemail-tz').value			= obj['voicemail']['tz'];
	dwho_eid('it-voicemailfeatures-skipcheckpass').checked	= dwho_bool(obj['voicemailfeatures']['skipcheckpass']);
	dwho_eid('it-voicemail-attach').value			= obj['voicemail']['attach'];
	dwho_eid('it-voicemail-deletevoicemail').checked	= dwho_bool(obj['voicemail']['deletevoicemail']);

	return(true);
}

function xivo_ast_user_onload()
{
	xivo_ast_build_users_elt();

	if((firstname = dwho_eid('it-userfeatures-firstname')) !== false)
	{
		dwho.dom.add_event('change',firstname,xivo_ast_user_cpy_name);
		dwho.dom.add_event('focus',firstname,xivo_ast_user_cpy_name);
		dwho.dom.add_event('blur',firstname,xivo_ast_user_chg_name);
	}

	if((lastname = dwho_eid('it-userfeatures-lastname')) !== false)
	{
		dwho.dom.add_event('change',lastname,xivo_ast_user_chg_name);
		dwho.dom.add_event('focus',lastname,xivo_ast_user_cpy_name);
		dwho.dom.add_event('blur',lastname,xivo_ast_user_chg_name);
	}

	if((xnumber = dwho_eid('it-userfeatures-number')) !== false)
	{
		dwho.dom.add_event('change',xnumber,xivo_ast_user_chg_name);
		dwho.dom.add_event('focus',xnumber,xivo_ast_user_cpy_name);
	}

	if((protocol = dwho_eid('it-protocol-protocol')) !== false)
	{
		xivo_ast_user_chg_protocol(protocol.value);

		dwho.dom.add_event('change',
				   protocol,
				   function()
				   {
					xivo_ast_user_chg_protocol(this.value);
					xivo_ast_user_chg_name();
					dwho.form.set_onfocus(this);
				   });
		dwho.dom.add_event('focus',protocol,xivo_ast_user_cpy_name);

		dwho.dom.add_event('change',
				   dwho_eid('it-protocol-host-type'),
				   xivo_ast_user_chg_host_type);

		dwho.dom.add_event('change',
				   dwho_eid('it-autoprov-modact'),
				   xivo_ast_user_chg_autoprov_modact);
	}

	if((voicemailoption = dwho_eid('it-voicemail-option')) !== false)
	{
		dwho_eid('it-voicemail-suggest').setAttribute('autocomplete','off');

		dwho.dom.add_event('focus',
				   dwho_eid('it-voicemail-suggest'),
				   xivo_ast_user_suggest_event_voicemail);

		var voicemailoption_fn = function()
					 {
					 	xivo_ast_user_chg_voicemail(voicemailoption.value);
					 };

		dwho.dom.add_event('change',voicemailoption,voicemailoption_fn);
	}

	if((outcallerid_type = dwho_eid('it-userfeatures-outcallerid-type')) !== false)
	{
		var outcallerid_type_fn = function()
					  {
						xivo_chg_attrib('ast_fm_user_outcallerid',
								'fd-userfeatures-outcallerid-custom',
								Number(outcallerid_type.value === 'custom'));
					  };

		outcallerid_type_fn();

		dwho.dom.add_event('change',outcallerid_type,outcallerid_type_fn);
	}

	if((pickupexten = dwho_eid('it-protocol-pickupexten')) !== false)
	{
		var pickupexten_fn = function()
					  {
						xivo_chg_attrib('ast_fm_user_pickupcontext',
								'fd-protocol-pickupcontext',
								Number(pickupexten.value === 'on'));
					  };

		pickupexten_fn();
		dwho.dom.add_event('change', pickupexten, pickupexten_fn);
	}

	dwho.dom.add_event('change',
			   dwho_eid('it-userfeatures-enableclient'),
			   xivo_ast_user_chg_enableclient);

	dwho.dom.add_event('change',
			   dwho_eid('it-userfeatures-enablerna'),
			   xivo_ast_user_chg_enablerna);

	dwho.dom.add_event('change',
			   dwho_eid('it-userfeatures-enablebusy'),
			   xivo_ast_user_chg_enablebusy);

	dwho.dom.add_event('change',
			   dwho_eid('it-userfeatures-enableunc'),
			   xivo_ast_user_chg_enableunc);


	xivo_ast_build_dialaction_array('noanswer');
	xivo_ast_build_dialaction_array('busy');
	xivo_ast_build_dialaction_array('congestion');
	xivo_ast_build_dialaction_array('chanunavail');

	xivo_ast_dialaction_onload();
}

dwho.dom.set_onload(xivo_ast_user_onload);
