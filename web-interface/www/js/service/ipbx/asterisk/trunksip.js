var xivo_iprotocol = 0;

var xivo_elt_protocol = new Array();

xivo_elt_protocol['fd-protocol-port'] = new Array();
xivo_elt_protocol['fd-protocol-port']['style'] = 'display:block';
xivo_elt_protocol['it-protocol-port'] = new Array();
xivo_elt_protocol['it-protocol-port']['property'] = 'disabled|false:boolean';
xivo_elt_protocol['fd-protocol-host-dynamic'] = new Array();
xivo_elt_protocol['fd-protocol-host-dynamic']['style'] = 'display:block';
xivo_elt_protocol['it-protocol-host-dynamic'] = new Array();
xivo_elt_protocol['it-protocol-host-dynamic']['property'] = 'disabled|false:boolean';
xivo_elt_protocol['fd-protocol-host-static'] = new Array();
xivo_elt_protocol['fd-protocol-host-static']['style'] = 'display:block';
xivo_elt_protocol['it-protocol-host-static'] = new Array();
xivo_elt_protocol['it-protocol-host-static']['property'] = 'disabled|false:boolean';
xivo_elt_protocol['fd-protocol-qualify'] = new Array();
xivo_elt_protocol['fd-protocol-qualify']['style'] = 'display:block';
xivo_elt_protocol['it-protocol-qualify'] = new Array();
xivo_elt_protocol['it-protocol-qualify']['property'] = 'disabled|false:boolean';
xivo_elt_protocol['fd-protocol-fromuser'] = new Array();
xivo_elt_protocol['fd-protocol-fromuser']['style'] = 'display:block';
xivo_elt_protocol['it-protocol-fromuser'] = new Array();
xivo_elt_protocol['it-protocol-fromuser']['property'] = 'disabled|false:boolean';
xivo_elt_protocol['fd-protocol-fromdomain'] = new Array();
xivo_elt_protocol['fd-protocol-fromdomain']['style'] = 'display:block';
xivo_elt_protocol['it-protocol-fromdomain'] = new Array();
xivo_elt_protocol['it-protocol-fromdomain']['property'] = 'disabled|false:boolean';

xivo_elt_protocol['links'] = new Array();
xivo_elt_protocol['links']['link'] = new Array();
xivo_elt_protocol['links']['link'][xivo_iprotocol++] = new Array('it-protocol-username',0,1);
xivo_elt_protocol['links']['link'][xivo_iprotocol++] = new Array('fd-protocol-port',0,1);
xivo_elt_protocol['links']['link'][xivo_iprotocol++] = new Array('it-protocol-port',0,1);
xivo_elt_protocol['links']['link'][xivo_iprotocol++] = new Array('fd-protocol-host-dynamic',0,1);
xivo_elt_protocol['links']['link'][xivo_iprotocol++] = new Array('it-protocol-host-dynamic',0,1);
xivo_elt_protocol['links']['link'][xivo_iprotocol++] = new Array('fd-protocol-host-static',0,1);
xivo_elt_protocol['links']['link'][xivo_iprotocol++] = new Array('it-protocol-host-static',0,1);
xivo_elt_protocol['links']['link'][xivo_iprotocol++] = new Array('fd-protocol-qualify',0,1);
xivo_elt_protocol['links']['link'][xivo_iprotocol++] = new Array('it-protocol-qualify',0,1);
xivo_elt_protocol['links']['link'][xivo_iprotocol++] = new Array('fd-protocol-fromuser',0,1);
xivo_elt_protocol['links']['link'][xivo_iprotocol++] = new Array('it-protocol-fromuser',0,1);
xivo_elt_protocol['links']['link'][xivo_iprotocol++] = new Array('fd-protocol-fromdomain',0,1);
xivo_elt_protocol['links']['link'][xivo_iprotocol++] = new Array('it-protocol-fromdomain',0,1);

var xivo_fm_protocol = new Array();

xivo_fm_protocol['user'] = xivo_clone(xivo_elt_protocol);
xivo_fm_protocol['user']['fd-protocol-port']['style'] = 'display:none';
xivo_fm_protocol['user']['it-protocol-port']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['user']['fd-protocol-host-dynamic']['style'] = 'display:none';
xivo_fm_protocol['user']['it-protocol-host-dynamic']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['user']['fd-protocol-host-static']['style'] = 'display:none';
xivo_fm_protocol['user']['it-protocol-host-static']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['user']['fd-protocol-qualify']['style'] = 'display:none';
xivo_fm_protocol['user']['it-protocol-qualify']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['user']['fd-protocol-fromuser']['style'] = 'display:none';
xivo_fm_protocol['user']['it-protocol-fromuser']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['user']['fd-protocol-fromdomain']['style'] = 'display:none';
xivo_fm_protocol['user']['it-protocol-fromdomain']['property'] = 'disabled|true:boolean';

xivo_attrib_register('fm_protocol-user',xivo_fm_protocol['user']);

xivo_fm_protocol['peer'] = xivo_clone(xivo_elt_protocol);

xivo_attrib_register('fm_protocol-peer',xivo_fm_protocol['peer']);

xivo_fm_protocol['friend'] = xivo_clone(xivo_elt_protocol);

xivo_attrib_register('fm_protocol-friend',xivo_fm_protocol['friend']);

var xivo_fm_host = new Array();

xivo_fm_host['fd-protocol-host-static'] = new Array();
xivo_fm_host['fd-protocol-host-static']['style'] = new Array('display:none','display:block');
xivo_fm_host['fd-protocol-host-static']['link'] = 'it-protocol-host-static';

xivo_fm_host['it-protocol-host-static'] = new Array();
xivo_fm_host['it-protocol-host-static']['property'] = new Array('disabled|true:boolean','disabled|false:boolean');

xivo_fm_codec = new Array();

xivo_fm_codec['it-protocol-disallow'] = new Array();
xivo_fm_codec['it-protocol-disallow']['property'] = new Array('disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled');
xivo_fm_codec['it-protocol-disallow']['link'] = 'it-codeclist';

xivo_fm_codec['it-codeclist'] = new Array();
xivo_fm_codec['it-codeclist']['property'] = new Array('disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled;selectedIndex|-1');
xivo_fm_codec['it-codeclist']['link'] = 'it-codec';

xivo_fm_codec['it-codec'] = new Array();
xivo_fm_codec['it-codec']['property'] = new Array('disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled;selectedIndex|-1');

xivo_attrib_register('fm_codec',xivo_fm_codec);

xivo_attrib_register('fm_host',xivo_fm_host);

xivo_fm_register = new Array();

xivo_fm_register['it-register-username'] = new Array();
xivo_fm_register['it-register-username']['property'] = new Array('disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled');
xivo_fm_register['it-register-username']['link'] = 'it-register-password';

xivo_fm_register['it-register-password'] = new Array();
xivo_fm_register['it-register-password']['property'] = new Array('disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled');
xivo_fm_register['it-register-password']['link'] = 'it-register-authuser';

xivo_fm_register['it-register-authuser'] = new Array();
xivo_fm_register['it-register-authuser']['property'] = new Array('disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled');
xivo_fm_register['it-register-authuser']['link'] = 'it-register-host';

xivo_fm_register['it-register-host'] = new Array();
xivo_fm_register['it-register-host']['property'] = new Array('disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled');
xivo_fm_register['it-register-host']['link'] = 'it-register-port';

xivo_fm_register['it-register-port'] = new Array();
xivo_fm_register['it-register-port']['property'] = new Array('disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled');
xivo_fm_register['it-register-port']['link'] = 'it-register-contact';

xivo_fm_register['it-register-contact'] = new Array();
xivo_fm_register['it-register-contact']['property'] = new Array('disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled');

xivo_attrib_register('fm_register',xivo_fm_register);

function xivo_ast_chg_trunk_type(type)
{
	if(xivo_is_undef(xivo_fm_protocol[type.value]) == true)
		return(false);

	var trunk_type = type.value;

	xivo_chg_attrib('fm_protocol-'+trunk_type,'links',0,1);

	if(trunk_type == 'user')
		return(true);

	var host_dynamic = xivo_eid('it-protocol-host-dynamic');

	if(host_dynamic != false)
		xivo_chg_attrib('fm_host','fd-protocol-host-static',(host_dynamic.value == 'dynamic' ? 0 : 1));
}

function xivo_ast_trunk_sip_onload()
{
	if(xivo_eid('it-protocol-type') != false)
		xivo_ast_chg_trunk_type(xivo_eid('it-protocol-type'));

	if((regactive = xivo_eid('it-register-active')) != false)
		xivo_chg_attrib('fm_register','it-register-username',(regactive.checked == true ? 0 : 1));

	if((codecative = xivo_eid('it-codec-active')) != false)
		xivo_chg_attrib('fm_codec','it-protocol-disallow',(codecative.checked == true ? 0 : 1));
}

xivo_winload.push('xivo_ast_trunk_sip_onload();');
