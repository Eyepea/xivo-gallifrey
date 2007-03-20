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

xivo_fm_trunk['user']['it-trunk-username'] = new Array();
xivo_fm_trunk['user']['it-trunk-username']['property'] = 'readOnly|false:boolean';
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

xivo_attrib_register('fm_trunk-user',xivo_fm_trunk['user']);

var xivo_ipeer = 0;

xivo_fm_trunk['peer'] = new Array();
xivo_fm_trunk['peer']['fds-peer'] = new Array();
xivo_fm_trunk['peer']['fds-peer']['link'] = new Array();
xivo_fm_trunk['peer']['fds-peer']['link'][xivo_ipeer++] = new Array('it-trunk-username',0,1);
xivo_fm_trunk['peer']['fds-peer']['link'][xivo_ipeer++] = new Array('fd-trunk-port',0,1);
xivo_fm_trunk['peer']['fds-peer']['link'][xivo_ipeer++] = new Array('it-trunk-port',0,1);
xivo_fm_trunk['peer']['fds-peer']['link'][xivo_ipeer++] = new Array('fd-trunk-host-dynamic',0,1);
xivo_fm_trunk['peer']['fds-peer']['link'][xivo_ipeer++] = new Array('it-trunk-host-dynamic',0,1);
xivo_fm_trunk['peer']['fds-peer']['link'][xivo_ipeer++] = new Array('fd-trunk-host-static',0,1);
xivo_fm_trunk['peer']['fds-peer']['link'][xivo_ipeer++] = new Array('it-trunk-host-static',0,1);
xivo_fm_trunk['peer']['fds-peer']['link'][xivo_ipeer++] = new Array('fd-trunk-qualify',0,1);
xivo_fm_trunk['peer']['fds-peer']['link'][xivo_ipeer++] = new Array('it-trunk-qualify',0,1);

xivo_fm_trunk['peer']['it-trunk-username'] = new Array();
xivo_fm_trunk['peer']['it-trunk-username']['property'] = 'readOnly|false:boolean';
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

xivo_attrib_register('fm_trunk-peer',xivo_fm_trunk['peer']);

xivo_fm_trunk['friend'] = xivo_clone(xivo_fm_trunk['peer']);
xivo_fm_trunk['friend']['fds-friend'] = xivo_clone(xivo_fm_trunk['peer']['fds-peer']);
xivo_fm_trunk['friend']['it-trunk-username']['property'] = 'readOnly|true:boolean';

delete(xivo_fm_trunk['friend']['fds-peer']);

xivo_attrib_register('fm_trunk-friend',xivo_fm_trunk['friend']);

var xivo_fm_host = new Array();

xivo_fm_host['fd-trunk-host-static'] = new Array();
xivo_fm_host['fd-trunk-host-static']['style'] = new Array('','display:none','display:block');
xivo_fm_host['fd-trunk-host-static']['link'] = 'it-trunk-host-static';

xivo_fm_host['it-trunk-host-static'] = new Array();
xivo_fm_host['it-trunk-host-static']['property'] = new Array('','disabled|true:boolean','disabled|false:boolean');

xivo_attrib_register('fm_host',xivo_fm_host);

function xivo_chgtrunk(trunk)
{
	if(xivo_is_undef(xivo_fm_trunk[trunk.value]) == true)
		return(false);

	xivo_trunk = trunk.value;

	xivo_chg_attrib('fm_trunk-'+xivo_trunk,'fds-'+xivo_trunk,0,1)

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
}

