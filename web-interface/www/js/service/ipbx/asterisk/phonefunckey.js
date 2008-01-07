var xivo_elt_phonefunckey = new Array();

var xivo_fm_phonefunckey = new Array();

var xivo_phonefunckey_type = new Array();
xivo_phonefunckey_type['user'] = 1;
xivo_phonefunckey_type['group'] = 1;
xivo_phonefunckey_type['queue'] = 1;
xivo_phonefunckey_type['meetme'] = 1;
xivo_phonefunckey_type['extension'] = 1;
xivo_phonefunckey_type['bosssecretary'] = 1;
xivo_phonefunckey_type['custom'] = 1;

function xivo_build_phonefunckey_array(id)
{
	if(xivo_is_undef(xivo_fm_phonefunckey[id]) == false
	&& xivo_is_array(xivo_fm_phonefunckey[id]) == true)
		return(true);

	xivo_fm_phonefunckey[id] = new Array();

	xivo_elt_phonefunckey[id] = new Array();
	xivo_elt_phonefunckey[id]['links'] = new Array();
	xivo_elt_phonefunckey[id]['links']['link'] = new Array();

	var i = 0;

	for(property in xivo_phonefunckey_type)
	{
		key = 'fd-phonefunckey-'+property+'-typeval-'+id;
		xivo_elt_phonefunckey[id][key] = new Array();
		xivo_elt_phonefunckey[id][key]['style'] = 'display:none';
		xivo_elt_phonefunckey[id]['links']['link'][i++] = new Array(key,0,1);

		key = 'it-phonefunckey-supervision-'+id;
		xivo_elt_phonefunckey[id][key] = new Array();
		xivo_elt_phonefunckey[id][key]['property'] = 'className|it-disabled';
		xivo_elt_phonefunckey[id]['links']['link'][i++] = new Array(key,0,1);

		key = 'it-phonefunckey-'+property+'-typeval-'+id;
		xivo_elt_phonefunckey[id][key] = new Array();
		xivo_elt_phonefunckey[id][key]['style'] = 'display:none';
		xivo_elt_phonefunckey[id][key]['property'] = 'disabled|true:boolean;className|it-disabled';
		xivo_elt_phonefunckey[id]['links']['link'][i++] = new Array(key,0,1);
	}

	for(property in xivo_phonefunckey_type)
	{
		xivo_fm_phonefunckey[id][property] = xivo_clone(xivo_elt_phonefunckey[id]);
		xivo_fm_phonefunckey[id][property]['fd-phonefunckey-'+property+'-typeval-'+id]['style'] = 'display:inline';

		keyit = 'it-phonefunckey-'+property+'-typeval-'+id;
		xivo_fm_phonefunckey[id][property][keyit]['style'] = 'display:inline';
		xivo_fm_phonefunckey[id][property][keyit]['property'] = 'disabled|false:boolean;className|it-enabled';

		if(property == 'user'
		|| property == 'bosssecretary'
		|| property == 'custom')
		{
			keyit = 'it-phonefunckey-supervision-'+id;
			xivo_fm_phonefunckey[id][property][keyit]['property'] = 'className|it-enabled';
		}

		xivo_attrib_register('fm_phonefunckey-'+id+'-'+property,xivo_fm_phonefunckey[id][property]);
	}
}

function xivo_chgphonefunckey(type)
{
	if(xivo_is_undef(type.value) == true
	|| (xivo_is_undef(type.disabled) == false && type.disabled == true) === true
	|| (rs = type.id.match(/-(\d+)$/)) == null)
		return(false);

	xivo_build_phonefunckey_array(rs[1]);

	xivo_chg_attrib('fm_phonefunckey-'+rs[1]+'-'+type.value,'links',0,1);
}

function xivo_phonefunckey_onload()
{
	if(xivo_is_undef(xivo_tlist['phonefunckey']) == true
	|| xivo_is_undef(xivo_tlist['phonefunckey']['cnt']) == true)
		return(false);

	for(var i = 0;i < xivo_tlist['phonefunckey']['cnt'];i++)
	{
		if((eid = xivo_eid('it-phonefunckey-type-'+i)) != false)
			xivo_chgphonefunckey(eid);
	}
}

xivo_winload.push('xivo_phonefunckey_onload();');
