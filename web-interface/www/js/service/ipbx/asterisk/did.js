var xivo_itype = 0;

var xivo_elt_type = new Array();

xivo_elt_type['fd-dfeatures-user-typeid'] = new Array();
xivo_elt_type['fd-dfeatures-user-typeid']['style'] = 'display:none';
xivo_elt_type['it-dfeatures-user-typeid'] = new Array();
xivo_elt_type['it-dfeatures-user-typeid']['property'] = 'disabled|true:boolean';
xivo_elt_type['fd-dfeatures-group-typeid'] = new Array();
xivo_elt_type['fd-dfeatures-group-typeid']['style'] = 'display:none';
xivo_elt_type['it-dfeatures-group-typeid'] = new Array();
xivo_elt_type['it-dfeatures-group-typeid']['property'] = 'disabled|true:boolean';
xivo_elt_type['fd-dfeatures-meetme-typeid'] = new Array();
xivo_elt_type['fd-dfeatures-meetme-typeid']['style'] = 'display:none';
xivo_elt_type['it-dfeatures-meetme-typeid'] = new Array();
xivo_elt_type['it-dfeatures-meetme-typeid']['property'] = 'disabled|true:boolean';
xivo_elt_type['fd-dfeatures-custom'] = new Array();
xivo_elt_type['fd-dfeatures-custom']['style'] = 'display:none';
xivo_elt_type['it-dfeatures-custom'] = new Array();
xivo_elt_type['it-dfeatures-custom']['property'] = 'disabled|true:boolean';

xivo_elt_type['links'] = new Array();
xivo_elt_type['links']['link'] = new Array();
xivo_elt_type['links']['link'][xivo_itype++] = new Array('fd-dfeatures-user-typeid',0,1);
xivo_elt_type['links']['link'][xivo_itype++] = new Array('it-dfeatures-user-typeid',0,1);
xivo_elt_type['links']['link'][xivo_itype++] = new Array('fd-dfeatures-group-typeid',0,1);
xivo_elt_type['links']['link'][xivo_itype++] = new Array('it-dfeatures-group-typeid',0,1);
xivo_elt_type['links']['link'][xivo_itype++] = new Array('fd-dfeatures-meetme-typeid',0,1);
xivo_elt_type['links']['link'][xivo_itype++] = new Array('it-dfeatures-meetme-typeid',0,1);
xivo_elt_type['links']['link'][xivo_itype++] = new Array('fd-dfeatures-custom',0,1);
xivo_elt_type['links']['link'][xivo_itype++] = new Array('it-dfeatures-custom',0,1);

var xivo_fm_type = new Array();

xivo_fm_type['user'] = xivo_clone(xivo_elt_type);
xivo_fm_type['user']['fd-dfeatures-user-typeid']['style'] = 'display:block';
xivo_fm_type['user']['it-dfeatures-user-typeid']['property'] = 'disabled|false:boolean';

xivo_attrib_register('fm_type-user',xivo_fm_type['user']);

xivo_fm_type['group'] = xivo_clone(xivo_elt_type);
xivo_fm_type['group']['fd-dfeatures-group-typeid']['style'] = 'display:block';
xivo_fm_type['group']['it-dfeatures-group-typeid']['property'] = 'disabled|false:boolean';

xivo_attrib_register('fm_type-group',xivo_fm_type['group']);

xivo_fm_type['meetme'] = xivo_clone(xivo_elt_type);
xivo_fm_type['meetme']['fd-dfeatures-meetme-typeid']['style'] = 'display:block';
xivo_fm_type['meetme']['it-dfeatures-meetme-typeid']['property'] = 'disabled|false:boolean';

xivo_attrib_register('fm_type-meetme',xivo_fm_type['meetme']);

xivo_fm_type['custom'] = xivo_clone(xivo_elt_type);
xivo_fm_type['custom']['fd-dfeatures-custom']['style'] = 'display:block';
xivo_fm_type['custom']['it-dfeatures-custom']['property'] = 'disabled|false:boolean';

xivo_attrib_register('fm_type-custom',xivo_fm_type['custom']);

function xivo_chgtype(type)
{
	if(xivo_is_undef(xivo_fm_type[type.value]) == true)
		return(false);

	xivo_chg_attrib('fm_type-'+type.value,'links',0,1);
}

xivo_winload += 'if(xivo_eid(\'it-dfeatures-type\') != false)\n' +
		'xivo_chgtype(xivo_eid(\'it-dfeatures-type\'));\n';
