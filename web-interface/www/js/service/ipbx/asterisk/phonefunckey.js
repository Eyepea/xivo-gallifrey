var xivo_elt_phonefunckey = new Array();

var xivo_fm_phonefunckey = new Array();

var xivo_phonefunckey_type = new Array();
xivo_phonefunckey_type['user'] = 1;
xivo_phonefunckey_type['group'] = 1;
xivo_phonefunckey_type['queue'] = 1;
xivo_phonefunckey_type['meetme'] = 1;
xivo_phonefunckey_type['extension'] = 1;
xivo_phonefunckey_type['custom'] = 1;

function xivo_build_phonefunckey_array(id)
{
	if(xivo_is_undef(xivo_fm_phonefunckey[id]) == false
	&& xivo_is_array(xivo_fm_phonefunckey[id]) == true)
		return(true);

	xivo_fm_phonefunckey[id] = new Array();

	xivo_elt_phonefunckey = new Array();
	xivo_elt_phonefunckey['links'] = new Array();
	xivo_elt_phonefunckey['links']['link'] = new Array();

	var i = 0;

	for(property in xivo_phonefunckey_type)
	{
		key = 'fd-phonefunckey-'+property+'-typeval-'+id;
		xivo_elt_phonefunckey[key] = new Array();
		xivo_elt_phonefunckey[key]['style'] = 'display:none';
		xivo_elt_phonefunckey['links']['link'][i++] = new Array(key,0,1);

		key = 'it-phonefunckey-supervision-'+id;
		xivo_elt_phonefunckey[key] = new Array();
		xivo_elt_phonefunckey[key]['property'] = 'disabled|true:boolean';
		xivo_elt_phonefunckey['links']['link'][i++] = new Array(key,0,1);

		key = 'it-phonefunckey-'+property+'-typeval-'+id;
		xivo_elt_phonefunckey[key] = new Array();
		xivo_elt_phonefunckey[key]['style'] = 'display:none';
		xivo_elt_phonefunckey[key]['property'] = 'disabled|true:boolean;className|it-disabled';
		xivo_elt_phonefunckey['links']['link'][i++] = new Array(key,0,1);
	}

	for(property in xivo_phonefunckey_type)
	{

		xivo_fm_phonefunckey[id][property] = xivo_clone(xivo_elt_phonefunckey);
		xivo_fm_phonefunckey[id][property]['fd-phonefunckey-'+property+'-typeval-'+id]['style'] = 'display:inline';

		keyit = 'it-phonefunckey-'+property+'-typeval-'+id;
		xivo_fm_phonefunckey[id][property][keyit]['style'] = 'display:inline';
		xivo_fm_phonefunckey[id][property][keyit]['property'] = 'disabled|false:boolean;className|it-enabled';

		if(property == 'user' || property == 'custom')
			xivo_fm_phonefunckey[id][property]['it-phonefunckey-supervision-'+id]['property'] = 'disabled|false:boolean';

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
	for(id in xivo_fm_phonefunckey)
	{
		if(xivo_eid('it-phonefunckey-'+id+'-type') != false)
			xivo_chgphonefunckey(id,xivo_eid('it-phonefunckey-'+id+'-type'));
	}
}

//xivo_winload.push('xivo_phonefunckey_onload();');
