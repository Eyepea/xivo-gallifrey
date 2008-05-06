var xivo_elt_dialstatus = new Array();
xivo_elt_dialstatus['noanswer'] = new Array();
xivo_elt_dialstatus['busy'] = new Array();
xivo_elt_dialstatus['congestion'] = new Array();
xivo_elt_dialstatus['chanunavail'] = new Array();

var xivo_fm_dialstatus = new Array();
xivo_fm_dialstatus['noanswer'] = new Array();
xivo_fm_dialstatus['busy'] = new Array();
xivo_fm_dialstatus['congestion'] = new Array();
xivo_fm_dialstatus['chanunavail'] = new Array();

var xivo_dialstatus_type = new Array();
xivo_dialstatus_type['endcall'] = 1;
xivo_dialstatus_type['user'] = 1;
xivo_dialstatus_type['group'] = 1;
xivo_dialstatus_type['queue'] = 1;
xivo_dialstatus_type['meetme'] = 1;
xivo_dialstatus_type['voicemail'] = 1;
xivo_dialstatus_type['schedule'] = 1;
xivo_dialstatus_type['application'] = 1;
xivo_dialstatus_type['custom'] = 1;
xivo_dialstatus_type['sound'] = 1;

function xivo_build_dialstatus_array(stat)
{
	if(xivo_is_undef(xivo_elt_dialstatus[stat]) == true
	|| xivo_is_array(xivo_elt_dialstatus[stat]) == false)
		return(false);

	xivo_elt_dialstatus[stat]['links'] = new Array();
	xivo_elt_dialstatus[stat]['links']['link'] = new Array();

	var i = 0;

	for(property in xivo_dialstatus_type)
	{
		key = 'fd-dialstatus-'+stat+'-'+property+'-typeval';
		xivo_elt_dialstatus[stat][key] = new Array();
		xivo_elt_dialstatus[stat][key]['style'] = 'display:none';
		xivo_elt_dialstatus[stat]['links']['link'][i++] = new Array(key,0,1);

		key = 'it-dialstatus-'+stat+'-'+property+'-typeval';
		xivo_elt_dialstatus[stat][key] = new Array();
		xivo_elt_dialstatus[stat][key]['property'] = 'disabled|true:boolean;className|it-disabled';
		xivo_elt_dialstatus[stat]['links']['link'][i++] = new Array(key,0,1);
	}

	for(property in xivo_dialstatus_type)
	{
		keyfd = 'fd-dialstatus-'+stat+'-'+property+'-typeval';
		keyit = 'it-dialstatus-'+stat+'-'+property+'-typeval';

		xivo_fm_dialstatus[stat][property] = xivo_clone(xivo_elt_dialstatus[stat]);
		xivo_fm_dialstatus[stat][property][keyfd]['style'] = 'display:block';
		xivo_fm_dialstatus[stat][property][keyit]['property'] = 'disabled|false:boolean;className|it-enabled';

		xivo_attrib_register('fm_dialstatus-'+stat+'-'+property,xivo_fm_dialstatus[stat][property]);
	}
}

xivo_build_dialstatus_array('noanswer');
xivo_build_dialstatus_array('busy');
xivo_build_dialstatus_array('congestion');
xivo_build_dialstatus_array('chanunavail');

function xivo_chgdialstatus(stat,type)
{
	if(xivo_is_undef(xivo_fm_dialstatus[stat]) == true
	|| xivo_is_array(xivo_fm_dialstatus[stat]) == false
	|| xivo_is_undef(type.value) == true
	|| xivo_is_undef(xivo_fm_dialstatus[stat][type.value]) == true
	|| xivo_is_array(xivo_fm_dialstatus[stat][type.value]) == false
	|| (xivo_is_undef(type.disabled) == false && type.disabled == true) === true)
		return(false);

	xivo_chg_attrib('fm_dialstatus-'+stat+'-'+type.value,'links',0,1);
}

function xivo_dialstatus_onload()
{
	for(stat in xivo_fm_dialstatus)
	{
		if(xivo_eid('it-dialstatus-'+stat+'-type') != false)
			xivo_chgdialstatus(stat,xivo_eid('it-dialstatus-'+stat+'-type'));
	}
}

xivo_winload.push('xivo_dialstatus_onload();');
