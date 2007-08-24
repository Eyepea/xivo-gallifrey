var xivo_imode = 0;

var xivo_elt_mode = new Array();

xivo_elt_mode['fd-outcall-numlen'] = new Array();
xivo_elt_mode['fd-outcall-numlen']['style'] = 'display:none';
xivo_elt_mode['it-outcall-numlen'] = new Array();
xivo_elt_mode['it-outcall-numlen']['property'] = 'disabled|true:boolean';
xivo_elt_mode['fd-extenumbers-exten'] = new Array();
xivo_elt_mode['fd-extenumbers-exten']['style'] = 'display:none';
xivo_elt_mode['it-extenumbers-exten'] = new Array();
xivo_elt_mode['it-extenumbers-exten']['property'] = 'disabled|true:boolean';
xivo_elt_mode['fd-extenumbers-range'] = new Array();
xivo_elt_mode['fd-extenumbers-range']['style'] = 'display:none';
xivo_elt_mode['it-extenumbers-rangebeg'] = new Array();
xivo_elt_mode['it-extenumbers-rangebeg']['property'] = 'disabled|true:boolean';
xivo_elt_mode['it-extenumbers-rangeend'] = new Array();
xivo_elt_mode['it-extenumbers-rangeend']['property'] = 'disabled|true:boolean';

xivo_elt_mode['links'] = new Array();
xivo_elt_mode['links']['link'] = new Array();
xivo_elt_mode['links']['link'][xivo_imode++] = new Array('fd-outcall-numlen',0,1);
xivo_elt_mode['links']['link'][xivo_imode++] = new Array('it-outcall-numlen',0,1);
xivo_elt_mode['links']['link'][xivo_imode++] = new Array('fd-extenumbers-exten',0,1);
xivo_elt_mode['links']['link'][xivo_imode++] = new Array('it-extenumbers-exten',0,1);
xivo_elt_mode['links']['link'][xivo_imode++] = new Array('fd-extenumbers-range',0,1);
xivo_elt_mode['links']['link'][xivo_imode++] = new Array('it-extenumbers-rangebeg',0,1);
xivo_elt_mode['links']['link'][xivo_imode++] = new Array('it-extenumbers-rangeend',0,1);

var xivo_fm_mode = new Array();

xivo_fm_mode['numlen'] = xivo_clone(xivo_elt_mode);
xivo_fm_mode['numlen']['fd-outcall-numlen']['style'] = 'display:block';
xivo_fm_mode['numlen']['it-outcall-numlen']['property'] = 'disabled|false:boolean';

xivo_attrib_register('fm_mode-numlen',xivo_fm_mode['numlen']);

xivo_fm_mode['extension'] = xivo_clone(xivo_elt_mode);
xivo_fm_mode['extension']['fd-extenumbers-exten']['style'] = 'display:block';
xivo_fm_mode['extension']['it-extenumbers-exten']['property'] = 'disabled|false:boolean';

xivo_attrib_register('fm_mode-extension',xivo_fm_mode['extension']);

xivo_fm_mode['range'] = xivo_clone(xivo_elt_mode);
xivo_fm_mode['range']['fd-extenumbers-range']['style'] = 'display:block';
xivo_fm_mode['range']['it-extenumbers-rangebeg']['property'] = 'disabled|false:boolean';
xivo_fm_mode['range']['it-extenumbers-rangeend']['property'] = 'disabled|false:boolean';

xivo_attrib_register('fm_mode-range',xivo_fm_mode['range']);

function xivo_chgmode(mode)
{
	if(xivo_is_undef(xivo_fm_mode[mode.value]) == true)
		return(false);

	xivo_chg_attrib('fm_mode-'+mode.value,'links',0,1);
}

xivo_winload += 'if(xivo_eid(\'it-outcall-mode\') != false)\n' +
		'xivo_chgmode(xivo_eid(\'it-outcall-mode\'));\n';
