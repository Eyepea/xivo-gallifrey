var xivo_protocol = '';

var xivo_iptl = 0;

var xivo_elt_protocol = new Array();

xivo_elt_protocol['fd-protocol-nat'] = new Array();
xivo_elt_protocol['fd-protocol-nat']['style'] = 'display:block';
xivo_elt_protocol['it-protocol-nat'] = new Array();
xivo_elt_protocol['it-protocol-nat']['property'] = 'disabled|false:boolean';
xivo_elt_protocol['fd-protocol-dtmfmode'] = new Array();
xivo_elt_protocol['fd-protocol-dtmfmode']['style'] = 'display:block';
xivo_elt_protocol['it-protocol-dtmfmode'] = new Array();
xivo_elt_protocol['it-protocol-dtmfmode']['property'] = 'disabled|false:boolean';
xivo_elt_protocol['fd-sip-protocol-host-dynamic'] = new Array();
xivo_elt_protocol['fd-sip-protocol-host-dynamic']['style'] = 'display:none';
xivo_elt_protocol['it-sip-protocol-host-dynamic'] = new Array();
xivo_elt_protocol['it-sip-protocol-host-dynamic']['property'] = 'disabled|true:boolean';
xivo_elt_protocol['fd-iax-protocol-host-dynamic'] = new Array();
xivo_elt_protocol['fd-iax-protocol-host-dynamic']['style'] = 'display:none';
xivo_elt_protocol['it-iax-protocol-host-dynamic'] = new Array();
xivo_elt_protocol['it-iax-protocol-host-dynamic']['property'] = 'disabled|true:boolean';
xivo_elt_protocol['fd-sip-protocol-host-static'] = new Array();
xivo_elt_protocol['fd-sip-protocol-host-static']['style'] = 'display:none';
xivo_elt_protocol['it-sip-protocol-host-static'] = new Array();
xivo_elt_protocol['it-sip-protocol-host-static']['property'] = 'disabled|true:boolean';
xivo_elt_protocol['fd-iax-protocol-host-static'] = new Array();
xivo_elt_protocol['fd-iax-protocol-host-static']['style'] = 'display:none';
xivo_elt_protocol['it-iax-protocol-host-static'] = new Array();
xivo_elt_protocol['it-iax-protocol-host-static']['property'] = 'disabled|true:boolean';
xivo_elt_protocol['fd-sip-protocol-context'] = new Array();
xivo_elt_protocol['fd-sip-protocol-context']['style'] = 'display:none';
xivo_elt_protocol['it-sip-protocol-context'] = new Array();
xivo_elt_protocol['it-sip-protocol-context']['property'] = 'disabled|true:boolean';
xivo_elt_protocol['fd-iax-protocol-context'] = new Array();
xivo_elt_protocol['fd-iax-protocol-context']['style'] = 'display:none';
xivo_elt_protocol['it-iax-protocol-context'] = new Array();
xivo_elt_protocol['it-iax-protocol-context']['property'] = 'disabled|true:boolean';
xivo_elt_protocol['fd-protocol-canreinvite'] = new Array();
xivo_elt_protocol['fd-protocol-canreinvite']['style'] = 'display:block';
xivo_elt_protocol['it-protocol-canreinvite'] = new Array();
xivo_elt_protocol['it-protocol-canreinvite']['property'] = 'disabled|false:boolean';
xivo_elt_protocol['fd-sip-protocol-amaflags'] = new Array();
xivo_elt_protocol['fd-sip-protocol-amaflags']['style'] = 'display:none';
xivo_elt_protocol['it-sip-protocol-amaflags'] = new Array();
xivo_elt_protocol['it-sip-protocol-amaflags']['property'] = 'disabled|true:boolean';
xivo_elt_protocol['fd-iax-protocol-amaflags'] = new Array();
xivo_elt_protocol['fd-iax-protocol-amaflags']['style'] = 'display:none';
xivo_elt_protocol['it-iax-protocol-amaflags'] = new Array();
xivo_elt_protocol['it-iax-protocol-amaflags']['property'] = 'disabled|true:boolean';
xivo_elt_protocol['fd-sip-protocol-qualify'] = new Array();
xivo_elt_protocol['fd-sip-protocol-qualify']['style'] = 'display:none';
xivo_elt_protocol['it-sip-protocol-qualify'] = new Array();
xivo_elt_protocol['it-sip-protocol-qualify']['property'] = 'disabled|ture:boolean';
xivo_elt_protocol['fd-iax-protocol-qualify'] = new Array();
xivo_elt_protocol['fd-iax-protocol-qualify']['style'] = 'display:none';
xivo_elt_protocol['it-iax-protocol-qualify'] = new Array();
xivo_elt_protocol['it-iax-protocol-qualify']['property'] = 'disabled|true:boolean';
xivo_elt_protocol['fd-sip-protocol-disallow'] = new Array();
xivo_elt_protocol['fd-sip-protocol-disallow']['style'] = 'display:none';
xivo_elt_protocol['it-sip-protocol-disallow'] = new Array();
xivo_elt_protocol['it-sip-protocol-disallow']['property'] = 'disabled|true:boolean';
xivo_elt_protocol['fd-iax-protocol-disallow'] = new Array();
xivo_elt_protocol['fd-iax-protocol-disallow']['style'] = 'display:none';
xivo_elt_protocol['it-iax-protocol-disallow'] = new Array();
xivo_elt_protocol['it-iax-protocol-disallow']['property'] = 'disabled|true:boolean';
xivo_elt_protocol['it-sip-codeclist'] = new Array();
xivo_elt_protocol['it-sip-codeclist']['style'] = 'display:none';
xivo_elt_protocol['it-sip-codeclist']['property'] = 'disabled|true:boolean';
xivo_elt_protocol['it-iax-codeclist'] = new Array();
xivo_elt_protocol['it-iax-codeclist']['style'] = 'display:none';
xivo_elt_protocol['it-iax-codeclist']['property'] = 'disabled|true:boolean';
xivo_elt_protocol['it-sip-codec'] = new Array();
xivo_elt_protocol['it-sip-codec']['style'] = 'display:none';
xivo_elt_protocol['it-sip-codec']['property'] = 'disabled|true:boolean';
xivo_elt_protocol['it-iax-codec'] = new Array();
xivo_elt_protocol['it-iax-codec']['style'] = 'display:none';
xivo_elt_protocol['it-iax-codec']['property'] = 'disabled|true:boolean';
xivo_elt_protocol['it-autoprov-modact'] = new Array();
xivo_elt_protocol['it-autoprov-modact']['property'] = 'disabled|false:boolean;className|it-enabled';
xivo_elt_protocol['it-autoprov-vendormodel'] = new Array();
xivo_elt_protocol['it-autoprov-vendormodel']['property'] = 'disabled|false:boolean;className|it-enabled';
xivo_elt_protocol['it-autoprov-macaddr'] = new Array();
xivo_elt_protocol['it-autoprov-macaddr']['property'] = 'disabled|false:boolean;className|it-enabled';

xivo_elt_protocol['links'] = new Array();
xivo_elt_protocol['links']['link'] = new Array();
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('fd-protocol-nat',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('it-protocol-nat',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('fd-protocol-dtmfmode',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('it-protocol-dtmfmode',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('fd-sip-protocol-host-dynamic',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('it-sip-protocol-host-dynamic',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('fd-iax-protocol-host-dynamic',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('it-iax-protocol-host-dynamic',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('fd-sip-protocol-host-static',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('it-sip-protocol-host-static',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('fd-iax-protocol-host-static',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('it-iax-protocol-host-static',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('fd-sip-protocol-context',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('it-sip-protocol-context',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('fd-iax-protocol-context',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('it-iax-protocol-context',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('fd-protocol-canreinvite',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('it-protocol-canreinvite',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('fd-sip-protocol-amaflags',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('it-sip-protocol-amaflags',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('fd-iax-protocol-amaflags',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('it-iax-protocol-amaflags',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('fd-sip-protocol-qualify',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('it-sip-protocol-qualify',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('fd-iax-protocol-qualify',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('it-iax-protocol-qualify',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('fd-sip-protocol-disallow',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('it-sip-protocol-disallow',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('fd-iax-protocol-disallow',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('it-iax-protocol-disallow',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('it-sip-codeclist',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('it-iax-codeclist',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('it-sip-codec',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('it-iax-codec',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('it-autoprov-modact',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('it-autoprov-vendormodel',0,1);
xivo_elt_protocol['links']['link'][xivo_iptl++] = new Array('it-autoprov-macaddr',0,1);

var xivo_fm_protocol = new Array();

xivo_fm_protocol['sip'] = xivo_clone(xivo_elt_protocol);
xivo_fm_protocol['sip']['fd-sip-protocol-host-dynamic']['style'] = 'display:block';
xivo_fm_protocol['sip']['it-sip-protocol-host-dynamic']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['sip']['fd-sip-protocol-host-static']['style'] = 'display:block';
xivo_fm_protocol['sip']['it-sip-protocol-host-static']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['sip']['fd-sip-protocol-context']['style'] = 'display:block';
xivo_fm_protocol['sip']['it-sip-protocol-context']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['sip']['fd-sip-protocol-amaflags']['style'] = 'display:block';
xivo_fm_protocol['sip']['it-sip-protocol-amaflags']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['sip']['fd-sip-protocol-qualify']['style'] = 'display:block';
xivo_fm_protocol['sip']['it-sip-protocol-qualify']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['sip']['fd-sip-protocol-disallow']['style'] = 'display:block';
xivo_fm_protocol['sip']['it-sip-protocol-disallow']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['sip']['it-sip-codeclist']['style'] = 'display:inline';
xivo_fm_protocol['sip']['it-sip-codeclist']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['sip']['it-sip-codec']['style'] = 'display:inline';
xivo_fm_protocol['sip']['it-sip-codec']['property'] = 'disabled|false:boolean';

xivo_attrib_register('fm_protocol-sip',xivo_fm_protocol['sip']);

xivo_fm_protocol['iax'] = xivo_clone(xivo_elt_protocol);
xivo_fm_protocol['iax']['fd-protocol-nat']['style'] = 'display:none';
xivo_fm_protocol['iax']['it-protocol-nat']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['iax']['fd-protocol-dtmfmode']['style'] = 'display:none';
xivo_fm_protocol['iax']['it-protocol-dtmfmode']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['iax']['fd-iax-protocol-host-dynamic']['style'] = 'display:block';
xivo_fm_protocol['iax']['it-iax-protocol-host-dynamic']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['iax']['fd-iax-protocol-host-static']['style'] = 'display:block';
xivo_fm_protocol['iax']['it-iax-protocol-host-static']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['iax']['fd-iax-protocol-context']['style'] = 'display:block';
xivo_fm_protocol['iax']['it-iax-protocol-context']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['iax']['fd-protocol-canreinvite']['style'] = 'display:none';
xivo_fm_protocol['iax']['it-protocol-canreinvite']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['iax']['fd-iax-protocol-amaflags']['style'] = 'display:block';
xivo_fm_protocol['iax']['it-iax-protocol-amaflags']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['iax']['fd-iax-protocol-qualify']['style'] = 'display:block';
xivo_fm_protocol['iax']['it-iax-protocol-qualify']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['iax']['fd-iax-protocol-disallow']['style'] = 'display:block';
xivo_fm_protocol['iax']['it-iax-protocol-disallow']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['iax']['it-iax-codeclist']['style'] = 'display:inline';
xivo_fm_protocol['iax']['it-iax-codeclist']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['iax']['it-iax-codec']['style'] = 'display:inline';
xivo_fm_protocol['iax']['it-iax-codec']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['iax']['it-autoprov-modact']['property'] = 'disabled|true:boolean;className|it-disabled';
xivo_fm_protocol['iax']['it-autoprov-vendormodel']['property'] = 'disabled|true:boolean;className|it-disabled';
xivo_fm_protocol['iax']['it-autoprov-macaddr']['property'] = 'disabled|true:boolean;className|it-disabled';

xivo_attrib_register('fm_protocol-iax',xivo_fm_protocol['iax']);

var xivo_fm_host = new Array();

xivo_fm_host['fd-sip-protocol-host-static'] = new Array();
xivo_fm_host['fd-sip-protocol-host-static']['style'] = new Array('display:none','display:block');
xivo_fm_host['fd-sip-protocol-host-static']['link'] = 'it-sip-protocol-host-static';

xivo_fm_host['it-sip-protocol-host-static'] = new Array();
xivo_fm_host['it-sip-protocol-host-static']['property'] = new Array('disabled|true:boolean','disabled|false:boolean');

xivo_fm_host['fd-iax-protocol-host-static'] = new Array();
xivo_fm_host['fd-iax-protocol-host-static']['style'] = new Array('display:none','display:block');
xivo_fm_host['fd-iax-protocol-host-static']['link'] = 'it-iax-protocol-host-static';

xivo_fm_host['it-iax-protocol-host-static'] = new Array();
xivo_fm_host['it-iax-protocol-host-static']['property'] = new Array('disabled|true:boolean','disabled|false:boolean');

xivo_attrib_register('fm_host',xivo_fm_host);

xivo_fm_grp = new Array();
xivo_fm_grp['in-group'] = new Array();
xivo_fm_grp['in-group']['link'] = new Array();
xivo_fm_grp['in-group']['link'][0] = new Array('it-ufeatures-ringgroup',0,1);

xivo_fm_grp['out-group'] = new Array();
xivo_fm_grp['out-group']['link'] = new Array();
xivo_fm_grp['out-group']['link'][0] = new Array('it-ufeatures-ringgroup',1,1);
xivo_fm_grp['out-group']['link'][1] = new Array('ringgroup',1,1);

xivo_fm_grp['it-ufeatures-ringgroup'] = new Array();
xivo_fm_grp['it-ufeatures-ringgroup']['property'] = new Array('disabled|false:boolean','disabled|true:boolean');
xivo_fm_grp['ringgroup'] = new Array();
xivo_fm_grp['ringgroup']['style'] = new Array('display:block','display:none');

xivo_attrib_register('fm_grp',xivo_fm_grp);

xivo_fm_codec = new Array();
xivo_fm_codec['it-sip-protocol-disallow'] = new Array();
xivo_fm_codec['it-sip-protocol-disallow']['property'] = new Array('disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled');
xivo_fm_codec['it-sip-protocol-disallow']['link'] = 'it-sip-codeclist';

xivo_fm_codec['it-sip-codeclist'] = new Array();
xivo_fm_codec['it-sip-codeclist']['property'] = new Array('disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled;selectedIndex|-1');
xivo_fm_codec['it-sip-codeclist']['link'] = 'it-sip-codec';

xivo_fm_codec['it-sip-codec'] = new Array();
xivo_fm_codec['it-sip-codec']['property'] = new Array('disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled;selectedIndex|-1');

xivo_fm_codec['it-iax-protocol-disallow'] = new Array();
xivo_fm_codec['it-iax-protocol-disallow']['property'] = new Array('disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled');
xivo_fm_codec['it-iax-protocol-disallow']['link'] = 'it-iax-codeclist';

xivo_fm_codec['it-iax-codeclist'] = new Array();
xivo_fm_codec['it-iax-codeclist']['property'] = new Array('disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled');
xivo_fm_codec['it-iax-codeclist']['link'] = 'it-iax-codec';

xivo_fm_codec['it-iax-codec'] = new Array();
xivo_fm_codec['it-iax-codec']['property'] = new Array('disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled;selectedIndex|-1');

xivo_attrib_register('fm_codec',xivo_fm_codec);

xivo_fm_voicemail = new Array();
xivo_fm_voicemail['it-voicemail-fullname'] = new Array();
xivo_fm_voicemail['it-voicemail-fullname']['property'] = new Array('disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled');
xivo_fm_voicemail['it-voicemail-fullname']['link'] = 'it-voicemail-password';

xivo_fm_voicemail['it-voicemail-password'] = new Array();
xivo_fm_voicemail['it-voicemail-password']['property'] = new Array('disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled');
xivo_fm_voicemail['it-voicemail-password']['link'] = 'it-voicemail-email';

xivo_fm_voicemail['it-voicemail-email'] = new Array();
xivo_fm_voicemail['it-voicemail-email']['property'] = new Array('disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled');
xivo_fm_voicemail['it-voicemail-email']['link'] = 'it-voicemail-attach';

xivo_fm_voicemail['it-voicemail-attach'] = new Array();
xivo_fm_voicemail['it-voicemail-attach']['property'] = new Array('disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled');
xivo_fm_voicemail['it-voicemail-attach']['link'] = 'it-voicemail-delete';

xivo_fm_voicemail['it-voicemail-delete'] = new Array();
xivo_fm_voicemail['it-voicemail-delete']['property'] = new Array('disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled');

xivo_attrib_register('fm_voicemail',xivo_fm_voicemail);

xivo_elt_autoprov = new Array();

xivo_elt_autoprov['it-autoprov-modact'] = new Array();
xivo_elt_autoprov['it-autoprov-modact']['property'] = 'disabled|false:boolean;className|it-enabled';
xivo_elt_autoprov['it-autoprov-modact']['link'] = 'it-autoprov-vendormodel';

xivo_elt_autoprov['it-autoprov-vendormodel'] = new Array();
xivo_elt_autoprov['it-autoprov-vendormodel']['property'] = new Array('disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled');
xivo_elt_autoprov['it-autoprov-vendormodel']['link'] = 'it-autoprov-macaddr';

xivo_elt_autoprov['it-autoprov-macaddr'] = new Array();
xivo_elt_autoprov['it-autoprov-macaddr']['property'] = new Array('disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled');

var xivo_fm_autoprov = new Array();

xivo_fm_autoprov['sip'] = xivo_clone(xivo_elt_autoprov);

xivo_attrib_register('fm_autoprov-sip',xivo_fm_autoprov['sip']);

xivo_fm_autoprov['iax'] = xivo_clone(xivo_elt_autoprov);
xivo_fm_autoprov['iax']['it-autoprov-modact']['property'] = 'disabled|true:boolean;className|it-disabled';
xivo_fm_autoprov['iax']['it-autoprov-vendormodel']['property'] = 'disabled|true:boolean;className|it-disabled';
xivo_fm_autoprov['iax']['it-autoprov-macaddr']['property'] = 'disabled|true:boolean;className|it-disabled';

xivo_attrib_register('fm_autoprov-iax',xivo_fm_autoprov['iax']);

xivo_fm_cpyname = new Array();
xivo_fm_cpyname['protocol-callerid'] = false;
xivo_fm_cpyname['voicemail-fullname'] = false;

function xivo_cpyname()
{
	if(xivo_eid('it-ufeatures-firstname') == false || xivo_eid('it-ufeatures-lastname') == false
	|| xivo_eid('it-protocol-callerid') == false)
		return(false);

	var name = '';

	var firstname = xivo_eid('it-ufeatures-firstname').value;
	var lastname = xivo_eid('it-ufeatures-lastname').value;

	if(xivo_is_undef(firstname) == false)
		name += firstname;

	if(xivo_is_undef(lastname) == false)
		name += name == '' ? lastname : ' '+lastname;

	var callerid = xivo_eid('it-protocol-username').value;

	if(xivo_is_undef(callerid) == true || callerid == name || callerid.length == 0)
		xivo_fm_cpyname['protocol-callerid'] = true;

	if(xivo_eid('it-voicemail-fullname') == false)
		return(false);

	var fullname = xivo_eid('it-voicemail-fullname').value;

	if(xivo_is_undef(fullname) == true || fullname == name || fullname.length == 0)
		xivo_fm_cpyname['voicemail-fullname'] = true;
}

function xivo_chgname()
{
	if(xivo_fm_cpyname['protocol-callerid'] == false && xivo_fm_cpyname['voicemail-fullname'] == false)
		return(false);

	var name = '';

	var firstname = xivo_eid('it-ufeatures-firstname').value;
	var lastname = xivo_eid('it-ufeatures-lastname').value;

	if(xivo_is_undef(firstname) == false)
		name += firstname;

	if(xivo_is_undef(lastname) == false)
		name += name.length == 0 ? lastname : ' '+lastname;

	if(xivo_fm_cpyname['protocol-callerid'] == true)
		xivo_eid('it-protocol-callerid').value = name;

	if(xivo_fm_cpyname['voicemail-fullname'] == true)
		xivo_eid('it-voicemail-fullname').value = name;

	return(true);
}

function xivo_chgprotocol(protocol)
{
	if(xivo_is_undef(xivo_fm_protocol[protocol.value]) == true)
		return(false);

	xivo_protocol = protocol.value;
	
	xivo_chg_attrib('fm_protocol-'+xivo_protocol,'links',0,1);

	var host_dynamic = xivo_eid('it-'+xivo_protocol+'-protocol-host-dynamic');

	if(host_dynamic != false)
		xivo_chg_attrib('fm_host','fd-'+xivo_protocol+'-protocol-host-static',(host_dynamic.value == 'dynamic' ? 0 : 1));

	if(xivo_eid('it-autoprov-modact') != false)
		xivo_chg_attrib('fm_autoprov-'+xivo_protocol,'it-autoprov-modact',(xivo_eid('it-autoprov-modact').value != '' ? 0 : 1));

	if(xivo_eid('it-codec-active') != false)
		xivo_chg_attrib('fm_codec','it-'+xivo_protocol+'-protocol-disallow',(xivo_eid('it-codec-active').checked == true ? 0 : 1));
}

function xivo_ingroup()
{
	xivo_fm_move_selected('it-grouplist','it-group');
	xivo_fm_copy_select('it-group','it-usergroup');

	var len = 0;

	if(xivo_eid('it-group') == false || (len = xivo_eid('it-group').length) < 1)
		return(false);

	var group = xivo_eid('it-group');

	for(i = 0;i < len;i++)
	{
		if(xivo_eid('group-'+group[i].value) == false)
			continue;

		xivo_eid('group-'+group[i].value).style.display = 'table-row';
	}

	if(xivo_eid('it-group').length > 0)
		xivo_eid('no-group').style.display = 'none';

	if(xivo_is_undef('it-usergroup') == true)
		return(false);

	if(xivo_eid('it-usergroup').length == 0)
		xivo_chg_attrib('fm_grp','out-group',0,1);
	else
	{
		xivo_chg_attrib('fm_grp','in-group',0,1);

		if(xivo_is_undef('it-ufeatures-ringgroup') == false && xivo_eid('it-ufeatures-ringgroup').checked == true)
			xivo_chg_attrib('fm_grp','ringgroup',0);
	}

	return(true);
}

function xivo_outgroup()
{
	xivo_fm_move_selected('it-group','it-grouplist');
	xivo_fm_copy_select('it-group','it-usergroup');
	
	var len = 0;

	if(xivo_eid('it-grouplist') == false || (len = xivo_eid('it-grouplist').length) < 1)
		return(false);

	var group = xivo_eid('it-grouplist');

	for(i = 0;i < len;i++)
	{
		if(xivo_eid('group-'+group[i].value) == false)
			continue;

		xivo_eid('group-'+group[i].value).style.display = 'none';
	}

	if(xivo_eid('it-group').length == 0)
		xivo_eid('no-group').style.display = 'table-row';

	if(xivo_is_undef('it-usergroup') == true)
		return(false);
	
	if(xivo_eid('it-usergroup').length == 0)
		xivo_chg_attrib('fm_grp','out-group',0,1);
	else
		xivo_chg_attrib('fm_grp','in-group',0,1);

	return(true);
}

xivo_winload += 'if(xivo_eid(\'it-protocol-protocol\') != false)\n' +
		'xivo_chgprotocol(xivo_eid(\'it-protocol-protocol\'));\n' +
		'if(xivo_eid(\'it-voicemail-active\') != false)\n' +
		'xivo_chg_attrib(\'fm_voicemail\',\'it-voicemail-fullname\',(xivo_eid(\'it-voicemail-active\').checked == true ? 0 : 1));\n';
