var xivo_trunk = '';

var xivo_iuser = 0;

var xivo_fm_trunk = new Array();

xivo_fm_trunk['user'] = new Array();
xivo_fm_trunk['user']['fds-user'] = new Array();
xivo_fm_trunk['user']['fds-user']['link'] = new Array();
xivo_fm_trunk['user']['fds-user']['link'][xivo_iuser++] = new Array('it-trunk-username',0,1);
xivo_fm_trunk['user']['fds-user']['link'][xivo_iuser++] = new Array('fd-trunk-port',0,1);
xivo_fm_trunk['user']['fds-user']['link'][xivo_iuser++] = new Array('it-trunk-port',0,1);
xivo_fm_trunk['user']['fds-user']['link'][xivo_iuser++] = new Array('fd-trunk-host-dynamic',0,1);
xivo_fm_trunk['user']['fds-user']['link'][xivo_iuser++] = new Array('it-trunk-host-dynamic',0,1);
xivo_fm_trunk['user']['fds-user']['link'][xivo_iuser++] = new Array('fd-trunk-host-static',0,1);
xivo_fm_trunk['user']['fds-user']['link'][xivo_iuser++] = new Array('it-trunk-host-static',0,1);
xivo_fm_trunk['user']['fds-user']['link'][xivo_iuser++] = new Array('fd-trunk-qualify',0,1);
xivo_fm_trunk['user']['fds-user']['link'][xivo_iuser++] = new Array('it-trunk-qualify',0,1);
xivo_fm_trunk['user']['fds-user']['link'][xivo_iuser++] = new Array('fd-trunk-fromuser',0,1);
xivo_fm_trunk['user']['fds-user']['link'][xivo_iuser++] = new Array('it-trunk-fromuser',0,1);
xivo_fm_trunk['user']['fds-user']['link'][xivo_iuser++] = new Array('fd-trunk-fromdomain',0,1);
xivo_fm_trunk['user']['fds-user']['link'][xivo_iuser++] = new Array('it-trunk-fromdomain',0,1);

xivo_fm_trunk['user']['it-trunk-username'] = new Array();
xivo_fm_trunk['user']['it-trunk-username']['property'] = 'readOnly|false:boolean;className|it-enabled';
xivo_fm_trunk['user']['fd-trunk-port'] = new Array();
xivo_fm_trunk['user']['fd-trunk-port']['style'] = 'display:none';
xivo_fm_trunk['user']['it-trunk-port'] = new Array();
xivo_fm_trunk['user']['it-trunk-port']['property'] = 'disabled|true:boolean';
xivo_fm_trunk['user']['fd-trunk-host-dynamic'] = new Array();
xivo_fm_trunk['user']['fd-trunk-host-dynamic']['style'] = 'display:none';
xivo_fm_trunk['user']['it-trunk-host-dynamic'] = new Array();
xivo_fm_trunk['user']['it-trunk-host-dynamic']['property'] = 'disabled|true:boolean';
xivo_fm_trunk['user']['fd-trunk-host-static'] = new Array();
xivo_fm_trunk['user']['fd-trunk-host-static']['style'] = 'display:none';
xivo_fm_trunk['user']['it-trunk-host-static'] = new Array();
xivo_fm_trunk['user']['it-trunk-host-static']['property'] = 'disabled|true:boolean';
xivo_fm_trunk['user']['fd-trunk-qualify'] = new Array();
xivo_fm_trunk['user']['fd-trunk-qualify']['style'] = 'display:none';
xivo_fm_trunk['user']['it-trunk-qualify'] = new Array();
xivo_fm_trunk['user']['it-trunk-qualify']['property'] = 'disabled|true:boolean';
xivo_fm_trunk['user']['fd-trunk-fromuser'] = new Array();
xivo_fm_trunk['user']['fd-trunk-fromuser']['style'] = 'display:none';
xivo_fm_trunk['user']['it-trunk-fromuser'] = new Array();
xivo_fm_trunk['user']['it-trunk-fromuser']['property'] = 'disabled|true:boolean';
xivo_fm_trunk['user']['fd-trunk-fromdomain'] = new Array();
xivo_fm_trunk['user']['fd-trunk-fromdomain']['style'] = 'display:none';
xivo_fm_trunk['user']['it-trunk-fromdomain'] = new Array();
xivo_fm_trunk['user']['it-trunk-fromdomain']['property'] = 'disabled|true:boolean';

xivo_attrib_register('fm_trunk-user',xivo_fm_trunk['user']);

var xivo_ipeer = 0;

xivo_fm_trunk['peer'] = new Array();
xivo_fm_trunk['peer']['fds-peer'] = xivo_clone(xivo_fm_trunk['user']['fds-user']);

xivo_fm_trunk['peer']['it-trunk-username'] = new Array();
xivo_fm_trunk['peer']['it-trunk-username']['property'] = 'readOnly|false:boolean;className|it-enabled';
xivo_fm_trunk['peer']['fd-trunk-port'] = new Array();
xivo_fm_trunk['peer']['fd-trunk-port']['style'] = 'display:block';
xivo_fm_trunk['peer']['it-trunk-port'] = new Array();
xivo_fm_trunk['peer']['it-trunk-port']['property'] = 'disabled|false:boolean';
xivo_fm_trunk['peer']['fd-trunk-host-dynamic'] = new Array();
xivo_fm_trunk['peer']['fd-trunk-host-dynamic']['style'] = 'display:block';
xivo_fm_trunk['peer']['it-trunk-host-dynamic'] = new Array();
xivo_fm_trunk['peer']['it-trunk-host-dynamic']['property'] = 'disabled|false:boolean';
xivo_fm_trunk['peer']['fd-trunk-host-static'] = new Array();
xivo_fm_trunk['peer']['fd-trunk-host-static']['style'] = 'display:block';
xivo_fm_trunk['peer']['it-trunk-host-static'] = new Array();
xivo_fm_trunk['peer']['it-trunk-host-static']['property'] = 'disabled|false:boolean';
xivo_fm_trunk['peer']['fd-trunk-qualify'] = new Array();
xivo_fm_trunk['peer']['fd-trunk-qualify']['style'] = 'display:block';
xivo_fm_trunk['peer']['it-trunk-qualify'] = new Array();
xivo_fm_trunk['peer']['it-trunk-qualify']['property'] = 'disabled|false:boolean';
xivo_fm_trunk['peer']['fd-trunk-fromuser'] = new Array();
xivo_fm_trunk['peer']['fd-trunk-fromuser']['style'] = 'display:block';
xivo_fm_trunk['peer']['it-trunk-fromuser'] = new Array();
xivo_fm_trunk['peer']['it-trunk-fromuser']['property'] = 'disabled|false:boolean';
xivo_fm_trunk['peer']['fd-trunk-fromdomain'] = new Array();
xivo_fm_trunk['peer']['fd-trunk-fromdomain']['style'] = 'display:block';
xivo_fm_trunk['peer']['it-trunk-fromdomain'] = new Array();
xivo_fm_trunk['peer']['it-trunk-fromdomain']['property'] = 'disabled|false:boolean';

xivo_attrib_register('fm_trunk-peer',xivo_fm_trunk['peer']);

xivo_fm_trunk['friend'] = xivo_clone(xivo_fm_trunk['peer']);
xivo_fm_trunk['friend']['fds-friend'] = xivo_clone(xivo_fm_trunk['peer']['fds-peer']);
xivo_fm_trunk['friend']['it-trunk-username']['property'] = 'readOnly|true:boolean;className|it-readonly';

delete(xivo_fm_trunk['friend']['fds-peer']);

xivo_attrib_register('fm_trunk-friend',xivo_fm_trunk['friend']);

var xivo_fm_host = new Array();

xivo_fm_host['fd-trunk-host-static'] = new Array();
xivo_fm_host['fd-trunk-host-static']['style'] = new Array('','display:none','display:block');
xivo_fm_host['fd-trunk-host-static']['link'] = 'it-trunk-host-static';

xivo_fm_host['it-trunk-host-static'] = new Array();
xivo_fm_host['it-trunk-host-static']['property'] = new Array('','disabled|true:boolean','disabled|false:boolean');

xivo_fm_codec = new Array();

xivo_fm_codec['it-trunk-disallow'] = new Array();
xivo_fm_codec['it-trunk-disallow']['property'] = new Array('','disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled');
xivo_fm_codec['it-trunk-disallow']['link'] = 'it-codeclist';

xivo_fm_codec['it-codeclist'] = new Array();
xivo_fm_codec['it-codeclist']['property'] = new Array('','disabled|false:boolean;className|it-enabled codeclisted','disabled|true:boolean;className|it-disabled codeclisted;selectedIndex|-1');
xivo_fm_codec['it-codeclist']['link'] = 'it-codec';

xivo_fm_codec['it-codec'] = new Array();
xivo_fm_codec['it-codec']['property'] = new Array('','disabled|false:boolean;className|it-enabled codecselected','disabled|true:boolean;className|it-disabled codecselected;selectedIndex|-1');

xivo_attrib_register('fm_codec',xivo_fm_codec);

xivo_attrib_register('fm_host',xivo_fm_host);

xivo_fm_register = new Array();

xivo_fm_register['it-register-username'] = new Array();
xivo_fm_register['it-register-username']['property'] = new Array('','disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled');
xivo_fm_register['it-register-username']['link'] = 'it-register-password';

xivo_fm_register['it-register-password'] = new Array();
xivo_fm_register['it-register-password']['property'] = new Array('','disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled');
xivo_fm_register['it-register-password']['link'] = 'it-register-authuser';

xivo_fm_register['it-register-authuser'] = new Array();
xivo_fm_register['it-register-authuser']['property'] = new Array('','disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled');
xivo_fm_register['it-register-authuser']['link'] = 'it-register-host';

xivo_fm_register['it-register-host'] = new Array();
xivo_fm_register['it-register-host']['property'] = new Array('','disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled');
xivo_fm_register['it-register-host']['link'] = 'it-register-port';

xivo_fm_register['it-register-port'] = new Array();
xivo_fm_register['it-register-port']['property'] = new Array('','disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled');
xivo_fm_register['it-register-port']['link'] = 'it-register-contact';

xivo_fm_register['it-register-contact'] = new Array();
xivo_fm_register['it-register-contact']['property'] = new Array('','disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled');

xivo_attrib_register('fm_register',xivo_fm_register);

function xivo_chgtrunk(trunk)
{
	if(xivo_is_undef(xivo_fm_trunk[trunk.value]) == true)
		return(false);

	xivo_trunk = trunk.value;

	xivo_chg_attrib('fm_trunk-'+xivo_trunk,'fds-'+xivo_trunk,0,1);

	if(xivo_trunk == 'friend' && xivo_eid('it-trunk-name') != false && xivo_eid('it-trunk-username') != false)
		xivo_eid('it-trunk-username').value = xivo_eid('it-trunk-name').value;

	if(xivo_trunk == 'user')
		return(true);

	var host_dynamic = xivo_eid('it-trunk-host-dynamic');

	if(host_dynamic != false)
		xivo_chg_attrib('fm_host','fd-trunk-host-static',(host_dynamic.value == 'dynamic' ? 1 : 2));
}

window.onload = function()
{
	if(xivo_eid('it-trunk-type') != false)
		xivo_chgtrunk(xivo_eid('it-trunk-type'));

	if(xivo_eid('smenu-tab-1') != false)
	{
		xivo_smenu['bak']['smenu-tab-1'] = xivo_eid('smenu-tab-1').className;
		xivo_smenu_click(xivo_eid('smenu-tab-1'),'moc','sb-part-general');
	}

	if(xivo_eid('it-register-active') != false)
		xivo_chg_attrib('fm_register','it-register-username',(xivo_eid('it-register-active').checked == true ? 1 : 2));

	if(xivo_eid('it-codec-active') != false)
		xivo_chg_attrib('fm_codec','it-trunk-disallow',(xivo_eid('it-codec-active').checked == true ? 1 : 2));
}

