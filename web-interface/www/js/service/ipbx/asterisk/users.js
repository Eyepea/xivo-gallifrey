var xivo_protocol = '';

var xivo_isip = 0;

var xivo_fm_protocol = new Array();

xivo_fm_protocol['sip'] = new Array();
xivo_fm_protocol['sip']['fds-sip'] = new Array();
xivo_fm_protocol['sip']['fds-sip']['link'] = new Array();
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('fd-protocol-nat',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('it-protocol-nat',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('fd-protocol-dtmfmode',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('it-protocol-dtmfmode',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('fd-sip-protocol-host-dynamic',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('it-sip-protocol-host-dynamic',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('fd-iax-protocol-host-dynamic',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('it-iax-protocol-host-dynamic',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('fd-sip-protocol-host-static',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('it-sip-protocol-host-static',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('fd-iax-protocol-host-static',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('it-iax-protocol-host-static',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('fd-sip-protocol-context',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('it-sip-protocol-context',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('fd-iax-protocol-context',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('it-iax-protocol-context',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('fd-protocol-canreinvite',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('it-protocol-canreinvite',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('fd-sip-protocol-amaflags',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('it-sip-protocol-amaflags',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('fd-iax-protocol-amaflags',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('it-iax-protocol-amaflags',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('fd-sip-protocol-qualify',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('it-sip-protocol-qualify',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('fd-iax-protocol-qualify',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('it-iax-protocol-qualify',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('fd-sip-protocol-disallow',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('it-sip-protocol-disallow',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('fd-iax-protocol-disallow',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('it-iax-protocol-disallow',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('it-sip-codeclist',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('it-iax-codeclist',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('it-sip-codec',0,1);
xivo_fm_protocol['sip']['fds-sip']['link'][xivo_isip++] = new Array('it-iax-codec',0,1);

xivo_fm_protocol['sip']['fd-protocol-nat'] = new Array();
xivo_fm_protocol['sip']['fd-protocol-nat']['style'] = 'display:block';
xivo_fm_protocol['sip']['it-protocol-nat'] = new Array();
xivo_fm_protocol['sip']['it-protocol-nat']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['sip']['fd-protocol-dtmfmode'] = new Array();
xivo_fm_protocol['sip']['fd-protocol-dtmfmode']['style'] = 'display:block';
xivo_fm_protocol['sip']['it-protocol-dtmfmode'] = new Array();
xivo_fm_protocol['sip']['it-protocol-dtmfmode']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['sip']['fd-sip-protocol-host-dynamic'] = new Array();
xivo_fm_protocol['sip']['fd-sip-protocol-host-dynamic']['style'] = 'display:block';
xivo_fm_protocol['sip']['it-sip-protocol-host-dynamic'] = new Array();
xivo_fm_protocol['sip']['it-sip-protocol-host-dynamic']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['sip']['fd-iax-protocol-host-dynamic'] = new Array();
xivo_fm_protocol['sip']['fd-iax-protocol-host-dynamic']['style'] = 'display:none';
xivo_fm_protocol['sip']['it-iax-protocol-host-dynamic'] = new Array();
xivo_fm_protocol['sip']['it-iax-protocol-host-dynamic']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['sip']['fd-sip-protocol-host-static'] = new Array();
xivo_fm_protocol['sip']['fd-sip-protocol-host-static']['style'] = 'display:block';
xivo_fm_protocol['sip']['it-sip-protocol-host-static'] = new Array();
xivo_fm_protocol['sip']['it-sip-protocol-host-static']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['sip']['fd-iax-protocol-host-static'] = new Array();
xivo_fm_protocol['sip']['fd-iax-protocol-host-static']['style'] = 'display:none';
xivo_fm_protocol['sip']['it-iax-protocol-host-static'] = new Array();
xivo_fm_protocol['sip']['it-iax-protocol-host-static']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['sip']['fd-sip-protocol-context'] = new Array();
xivo_fm_protocol['sip']['fd-sip-protocol-context']['style'] = 'display:block';
xivo_fm_protocol['sip']['it-sip-protocol-context'] = new Array();
xivo_fm_protocol['sip']['it-sip-protocol-context']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['sip']['fd-iax-protocol-context'] = new Array();
xivo_fm_protocol['sip']['fd-iax-protocol-context']['style'] = 'display:none';
xivo_fm_protocol['sip']['it-iax-protocol-context'] = new Array();
xivo_fm_protocol['sip']['it-iax-protocol-context']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['sip']['fd-protocol-canreinvite'] = new Array();
xivo_fm_protocol['sip']['fd-protocol-canreinvite']['style'] = 'display:block';
xivo_fm_protocol['sip']['it-protocol-canreinvite'] = new Array();
xivo_fm_protocol['sip']['it-protocol-canreinvite']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['sip']['fd-sip-protocol-amaflags'] = new Array();
xivo_fm_protocol['sip']['fd-sip-protocol-amaflags']['style'] = 'display:block';
xivo_fm_protocol['sip']['it-sip-protocol-amaflags'] = new Array();
xivo_fm_protocol['sip']['it-sip-protocol-amaflags']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['sip']['fd-iax-protocol-amaflags'] = new Array();
xivo_fm_protocol['sip']['fd-iax-protocol-amaflags']['style'] = 'display:none';
xivo_fm_protocol['sip']['it-iax-protocol-amaflags'] = new Array();
xivo_fm_protocol['sip']['it-iax-protocol-amaflags']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['sip']['fd-sip-protocol-qualify'] = new Array();
xivo_fm_protocol['sip']['fd-sip-protocol-qualify']['style'] = 'display:block';
xivo_fm_protocol['sip']['it-sip-protocol-qualify'] = new Array();
xivo_fm_protocol['sip']['it-sip-protocol-qualify']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['sip']['fd-iax-protocol-qualify'] = new Array();
xivo_fm_protocol['sip']['fd-iax-protocol-qualify']['style'] = 'display:none';
xivo_fm_protocol['sip']['it-iax-protocol-qualify'] = new Array();
xivo_fm_protocol['sip']['it-iax-protocol-qualify']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['sip']['fd-sip-protocol-disallow'] = new Array();
xivo_fm_protocol['sip']['fd-sip-protocol-disallow']['style'] = 'display:block';
xivo_fm_protocol['sip']['it-sip-protocol-disallow'] = new Array();
xivo_fm_protocol['sip']['it-sip-protocol-disallow']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['sip']['fd-iax-protocol-disallow'] = new Array();
xivo_fm_protocol['sip']['fd-iax-protocol-disallow']['style'] = 'display:none';
xivo_fm_protocol['sip']['it-iax-protocol-disallow'] = new Array();
xivo_fm_protocol['sip']['it-iax-protocol-disallow']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['sip']['it-sip-codeclist'] = new Array();
xivo_fm_protocol['sip']['it-sip-codeclist']['style'] = 'display:block';
xivo_fm_protocol['sip']['it-sip-codeclist']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['sip']['it-iax-codeclist'] = new Array();
xivo_fm_protocol['sip']['it-iax-codeclist']['style'] = 'display:none';
xivo_fm_protocol['sip']['it-iax-codeclist']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['sip']['it-sip-codec'] = new Array();
xivo_fm_protocol['sip']['it-sip-codec']['style'] = 'display:block';
xivo_fm_protocol['sip']['it-sip-codec']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['sip']['it-iax-codec'] = new Array();
xivo_fm_protocol['sip']['it-iax-codec']['style'] = 'display:none';
xivo_fm_protocol['sip']['it-iax-codec']['property'] = 'disabled|true:boolean';

xivo_attrib_register('fm_protocol-sip',xivo_fm_protocol['sip']);

var xivo_iiax = 0;

xivo_fm_protocol['iax'] = new Array();
xivo_fm_protocol['iax']['fds-iax'] = new Array();
xivo_fm_protocol['iax']['fds-iax']['link'] = new Array();
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('fd-protocol-nat',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('it-protocol-nat',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('fd-protocol-dtmfmode',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('it-protocol-dtmfmode',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('fd-sip-protocol-host-dynamic',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('it-sip-protocol-host-dynamic',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('fd-iax-protocol-host-dynamic',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('it-iax-protocol-host-dynamic',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('fd-sip-protocol-host-static',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('it-sip-protocol-host-static',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('fd-iax-protocol-host-static',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('it-iax-protocol-host-static',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('fd-sip-protocol-context',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('it-sip-protocol-context',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('fd-iax-protocol-context',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('it-iax-protocol-context',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('fd-protocol-canreinvite',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('it-protocol-canreinvite',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('fd-sip-protocol-amaflags',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('it-sip-protocol-amaflags',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('fd-iax-protocol-amaflags',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('it-iax-protocol-amaflags',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('fd-sip-protocol-qualify',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('it-sip-protocol-qualify',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('fd-iax-protocol-qualify',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('it-iax-protocol-qualify',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('fd-sip-protocol-disallow',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('it-sip-protocol-disallow',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('fd-iax-protocol-disallow',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('it-iax-protocol-disallow',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('it-sip-codeclist',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('it-iax-codeclist',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('it-sip-codec',0,1);
xivo_fm_protocol['iax']['fds-iax']['link'][xivo_iiax++] = new Array('it-iax-codec',0,1);

xivo_fm_protocol['iax']['fd-protocol-nat'] = new Array();
xivo_fm_protocol['iax']['fd-protocol-nat']['style'] = 'display:none';
xivo_fm_protocol['iax']['it-protocol-nat'] = new Array();
xivo_fm_protocol['iax']['it-protocol-nat']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['iax']['fd-protocol-dtmfmode'] = new Array();
xivo_fm_protocol['iax']['fd-protocol-dtmfmode']['style'] = 'display:none';
xivo_fm_protocol['iax']['it-protocol-dtmfmode'] = new Array();
xivo_fm_protocol['iax']['it-protocol-dtmfmode']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['iax']['fd-sip-protocol-host-dynamic'] = new Array();
xivo_fm_protocol['iax']['fd-sip-protocol-host-dynamic']['style'] = 'display:none';
xivo_fm_protocol['iax']['it-sip-protocol-host-dynamic'] = new Array();
xivo_fm_protocol['iax']['it-sip-protocol-host-dynamic']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['iax']['fd-iax-protocol-host-dynamic'] = new Array();
xivo_fm_protocol['iax']['fd-iax-protocol-host-dynamic']['style'] = 'display:block';
xivo_fm_protocol['iax']['it-iax-protocol-host-dynamic'] = new Array();
xivo_fm_protocol['iax']['it-iax-protocol-host-dynamic']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['iax']['fd-sip-protocol-host-static'] = new Array();
xivo_fm_protocol['iax']['fd-sip-protocol-host-static']['style'] = 'display:none';
xivo_fm_protocol['iax']['it-sip-protocol-host-static'] = new Array();
xivo_fm_protocol['iax']['it-sip-protocol-host-static']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['iax']['fd-iax-protocol-host-static'] = new Array();
xivo_fm_protocol['iax']['fd-iax-protocol-host-static']['style'] = 'display:block';
xivo_fm_protocol['iax']['it-iax-protocol-host-static'] = new Array();
xivo_fm_protocol['iax']['it-iax-protocol-host-static']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['iax']['fd-sip-protocol-context'] = new Array();
xivo_fm_protocol['iax']['fd-sip-protocol-context']['style'] = 'display:none';
xivo_fm_protocol['iax']['it-sip-protocol-context'] = new Array();
xivo_fm_protocol['iax']['it-sip-protocol-context']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['iax']['fd-iax-protocol-context'] = new Array();
xivo_fm_protocol['iax']['fd-iax-protocol-context']['style'] = 'display:block';
xivo_fm_protocol['iax']['it-iax-protocol-context'] = new Array();
xivo_fm_protocol['iax']['it-iax-protocol-context']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['iax']['fd-protocol-canreinvite'] = new Array();
xivo_fm_protocol['iax']['fd-protocol-canreinvite']['style'] = 'display:none';
xivo_fm_protocol['iax']['it-protocol-canreinvite'] = new Array();
xivo_fm_protocol['iax']['it-protocol-canreinvite']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['iax']['fd-sip-protocol-amaflags'] = new Array();
xivo_fm_protocol['iax']['fd-sip-protocol-amaflags']['style'] = 'display:none';
xivo_fm_protocol['iax']['it-sip-protocol-amaflags'] = new Array();
xivo_fm_protocol['iax']['it-sip-protocol-amaflags']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['iax']['fd-iax-protocol-amaflags'] = new Array();
xivo_fm_protocol['iax']['fd-iax-protocol-amaflags']['style'] = 'display:block';
xivo_fm_protocol['iax']['it-iax-protocol-amaflags'] = new Array();
xivo_fm_protocol['iax']['it-iax-protocol-amaflags']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['iax']['fd-sip-protocol-qualify'] = new Array();
xivo_fm_protocol['iax']['fd-sip-protocol-qualify']['style'] = 'display:none';
xivo_fm_protocol['iax']['it-sip-protocol-qualify'] = new Array();
xivo_fm_protocol['iax']['it-sip-protocol-qualify']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['iax']['fd-iax-protocol-qualify'] = new Array();
xivo_fm_protocol['iax']['fd-iax-protocol-qualify']['style'] = 'display:block';
xivo_fm_protocol['iax']['it-iax-protocol-qualify'] = new Array();
xivo_fm_protocol['iax']['it-iax-protocol-qualify']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['iax']['fd-sip-protocol-disallow'] = new Array();
xivo_fm_protocol['iax']['fd-sip-protocol-disallow']['style'] = 'display:none';
xivo_fm_protocol['iax']['it-sip-protocol-disallow'] = new Array();
xivo_fm_protocol['iax']['it-sip-protocol-disallow']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['iax']['fd-iax-protocol-disallow'] = new Array();
xivo_fm_protocol['iax']['fd-iax-protocol-disallow']['style'] = 'display:block';
xivo_fm_protocol['iax']['it-iax-protocol-disallow'] = new Array();
xivo_fm_protocol['iax']['it-iax-protocol-disallow']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['iax']['it-sip-codeclist'] = new Array();
xivo_fm_protocol['iax']['it-sip-codeclist']['style'] = 'display:none';
xivo_fm_protocol['iax']['it-sip-codeclist']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['iax']['it-iax-codeclist'] = new Array();
xivo_fm_protocol['iax']['it-iax-codeclist']['style'] = 'display:block';
xivo_fm_protocol['iax']['it-iax-codeclist']['property'] = 'disabled|false:boolean';
xivo_fm_protocol['iax']['it-sip-codec'] = new Array();
xivo_fm_protocol['iax']['it-sip-codec']['style'] = 'display:none';
xivo_fm_protocol['iax']['it-sip-codec']['property'] = 'disabled|true:boolean';
xivo_fm_protocol['iax']['it-iax-codec'] = new Array();
xivo_fm_protocol['iax']['it-iax-codec']['style'] = 'display:block';
xivo_fm_protocol['iax']['it-iax-codec']['property'] = 'disabled|false:boolean';

xivo_attrib_register('fm_protocol-iax',xivo_fm_protocol['iax']);

var xivo_fm_host = new Array();

xivo_fm_host['fd-sip-protocol-host-static'] = new Array();
xivo_fm_host['fd-sip-protocol-host-static']['style'] = new Array('','display:none','display:block');
xivo_fm_host['fd-sip-protocol-host-static']['link'] = 'it-sip-protocol-host-static';

xivo_fm_host['it-sip-protocol-host-static'] = new Array();
xivo_fm_host['it-sip-protocol-host-static']['property'] = new Array('','disabled|true:boolean','disabled|false:boolean');

xivo_fm_host['fd-iax-protocol-host-static'] = new Array();
xivo_fm_host['fd-iax-protocol-host-static']['style'] = new Array('','display:none','display:block');
xivo_fm_host['fd-iax-protocol-host-static']['link'] = 'it-iax-protocol-host-static';

xivo_fm_host['it-iax-protocol-host-static'] = new Array();
xivo_fm_host['it-iax-protocol-host-static']['property'] = new Array('','disabled|true:boolean','disabled|false:boolean');

xivo_attrib_register('fm_host',xivo_fm_host);

xivo_fm_grp = new Array();
xivo_fm_grp['in-group'] = new Array();
xivo_fm_grp['in-group']['link'] = new Array();
xivo_fm_grp['in-group']['link'][0] = new Array('it-ufeatures-ringgroup',1,1);

xivo_fm_grp['out-group'] = new Array();
xivo_fm_grp['out-group']['link'] = new Array();
xivo_fm_grp['out-group']['link'][0] = new Array('it-ufeatures-ringgroup',2,1);
xivo_fm_grp['out-group']['link'][1] = new Array('ringgroup',2,1);

xivo_fm_grp['it-ufeatures-ringgroup'] = new Array();
xivo_fm_grp['it-ufeatures-ringgroup']['property'] = new Array('','disabled|false:boolean','disabled|true:boolean');
xivo_fm_grp['ringgroup'] = new Array();
xivo_fm_grp['ringgroup']['style'] = new Array('','display:block','display:none');

xivo_attrib_register('fm_grp',xivo_fm_grp);


function xivo_chgprotocol(protocol)
{
	if(xivo_is_undef(xivo_fm_protocol[protocol.value]) == true)
		return(false);

	xivo_protocol = protocol.value;
	
	xivo_chg_attrib('fm_protocol-'+xivo_protocol,'fds-'+xivo_protocol,0,1);

	var host_dynamic = xivo_eid('it-'+xivo_protocol+'-protocol-host-dynamic');

	if(host_dynamic != false)
		xivo_chg_attrib('fm_host','fd-'+xivo_protocol+'-protocol-host-static',(host_dynamic.value == 'dynamic' ? 1 : 2));
}

function xivo_ingroup()
{
	xivo_fm_move_selected('it-grouplist','it-group');
	xivo_fm_copy_select('it-group','it-usergroup');

	if(xivo_is_undef('it-usergroup') == true)
		return(false);

	if(xivo_eid('it-usergroup').length == 0)
		xivo_chg_attrib('fm_grp','out-group',0,1);
	else
	{
		xivo_chg_attrib('fm_grp','in-group',0,1);

		if(xivo_is_undef('it-ufeatures-ringgroup') == false && xivo_eid('it-ufeatures-ringgroup').checked == true)
			xivo_chg_attrib('fm_grp','ringgroup',1);
	}

	return(true);
}

function xivo_outgroup()
{
	xivo_fm_move_selected('it-group','it-grouplist');
	xivo_fm_copy_select('it-group','it-usergroup');
	
	if(xivo_is_undef('it-usergroup') == true)
		return(false);
	
	if(xivo_eid('it-usergroup').length == 0)
		xivo_chg_attrib('fm_grp','out-group',0,1);
	else
		xivo_chg_attrib('fm_grp','in-group',0,1);

	return(true);
}

window.onload = function()
{
	if(xivo_eid('it-protocol-protocol') != false)
		xivo_chgprotocol(xivo_eid('it-protocol-protocol'));
}
