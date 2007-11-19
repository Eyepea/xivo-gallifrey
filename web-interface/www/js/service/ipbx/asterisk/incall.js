var xivo_itype = 0;

var xivo_elt_type = new Array();

xivo_elt_type['fd-incall-endcall-typeval'] = new Array();
xivo_elt_type['fd-incall-endcall-typeval']['style'] = 'display:none';
xivo_elt_type['it-incall-endcall-typeval'] = new Array();
xivo_elt_type['it-incall-endcall-typeval']['property'] = 'disabled|true:boolean';
xivo_elt_type['fd-incall-user-typeval'] = new Array();
xivo_elt_type['fd-incall-user-typeval']['style'] = 'display:none';
xivo_elt_type['it-incall-user-typeval'] = new Array();
xivo_elt_type['it-incall-user-typeval']['property'] = 'disabled|true:boolean';
xivo_elt_type['fd-incall-group-typeval'] = new Array();
xivo_elt_type['fd-incall-group-typeval']['style'] = 'display:none';
xivo_elt_type['it-incall-group-typeval'] = new Array();
xivo_elt_type['it-incall-group-typeval']['property'] = 'disabled|true:boolean';
xivo_elt_type['fd-incall-queue-typeval'] = new Array();
xivo_elt_type['fd-incall-queue-typeval']['style'] = 'display:none';
xivo_elt_type['it-incall-queue-typeval'] = new Array();
xivo_elt_type['it-incall-queue-typeval']['property'] = 'disabled|true:boolean';
xivo_elt_type['fd-incall-meetme-typeval'] = new Array();
xivo_elt_type['fd-incall-meetme-typeval']['style'] = 'display:none';
xivo_elt_type['it-incall-meetme-typeval'] = new Array();
xivo_elt_type['it-incall-meetme-typeval']['property'] = 'disabled|true:boolean';
xivo_elt_type['fd-incall-schedule-typeval'] = new Array();
xivo_elt_type['fd-incall-schedule-typeval']['style'] = 'display:none';
xivo_elt_type['it-incall-schedule-typeval'] = new Array();
xivo_elt_type['it-incall-schedule-typeval']['property'] = 'disabled|true:boolean';
xivo_elt_type['fd-incall-application-typeval'] = new Array();
xivo_elt_type['fd-incall-application-typeval']['style'] = 'display:none';
xivo_elt_type['it-incall-application-typeval'] = new Array();
xivo_elt_type['it-incall-application-typeval']['property'] = 'disabled|true:boolean';
xivo_elt_type['fd-incall-sound-typeval'] = new Array();
xivo_elt_type['fd-incall-sound-typeval']['style'] = 'display:none';
xivo_elt_type['it-incall-sound-typeval'] = new Array();
xivo_elt_type['it-incall-sound-typeval']['property'] = 'disabled|true:boolean';
xivo_elt_type['fd-incall-custom-typeval'] = new Array();
xivo_elt_type['fd-incall-custom-typeval']['style'] = 'display:none';
xivo_elt_type['it-incall-custom-typeval'] = new Array();
xivo_elt_type['it-incall-custom-typeval']['property'] = 'disabled|true:boolean';

xivo_elt_type['links'] = new Array();
xivo_elt_type['links']['link'] = new Array();
xivo_elt_type['links']['link'][xivo_itype++] = new Array('fd-incall-endcall-typeval',0,1);
xivo_elt_type['links']['link'][xivo_itype++] = new Array('it-incall-endcall-typeval',0,1);
xivo_elt_type['links']['link'][xivo_itype++] = new Array('fd-incall-user-typeval',0,1);
xivo_elt_type['links']['link'][xivo_itype++] = new Array('it-incall-user-typeval',0,1);
xivo_elt_type['links']['link'][xivo_itype++] = new Array('fd-incall-group-typeval',0,1);
xivo_elt_type['links']['link'][xivo_itype++] = new Array('it-incall-group-typeval',0,1);
xivo_elt_type['links']['link'][xivo_itype++] = new Array('fd-incall-queue-typeval',0,1);
xivo_elt_type['links']['link'][xivo_itype++] = new Array('it-incall-queue-typeval',0,1);
xivo_elt_type['links']['link'][xivo_itype++] = new Array('fd-incall-meetme-typeval',0,1);
xivo_elt_type['links']['link'][xivo_itype++] = new Array('it-incall-meetme-typeval',0,1);
xivo_elt_type['links']['link'][xivo_itype++] = new Array('fd-incall-schedule-typeval',0,1);
xivo_elt_type['links']['link'][xivo_itype++] = new Array('it-incall-schedule-typeval',0,1);
xivo_elt_type['links']['link'][xivo_itype++] = new Array('fd-incall-application-typeval',0,1);
xivo_elt_type['links']['link'][xivo_itype++] = new Array('it-incall-application-typeval',0,1);
xivo_elt_type['links']['link'][xivo_itype++] = new Array('fd-incall-sound-typeval',0,1);
xivo_elt_type['links']['link'][xivo_itype++] = new Array('it-incall-sound-typeval',0,1);
xivo_elt_type['links']['link'][xivo_itype++] = new Array('fd-incall-custom-typeval',0,1);
xivo_elt_type['links']['link'][xivo_itype++] = new Array('it-incall-custom-typeval',0,1);

var xivo_fm_type = new Array();

xivo_fm_type['endcall'] = xivo_clone(xivo_elt_type);
xivo_fm_type['endcall']['fd-incall-endcall-typeval']['style'] = 'display:block';
xivo_fm_type['endcall']['it-incall-endcall-typeval']['property'] = 'disabled|false:boolean';

xivo_attrib_register('fm_type-endcall',xivo_fm_type['endcall']);

xivo_fm_type['user'] = xivo_clone(xivo_elt_type);
xivo_fm_type['user']['fd-incall-user-typeval']['style'] = 'display:block';
xivo_fm_type['user']['it-incall-user-typeval']['property'] = 'disabled|false:boolean';

xivo_attrib_register('fm_type-user',xivo_fm_type['user']);

xivo_fm_type['group'] = xivo_clone(xivo_elt_type);
xivo_fm_type['group']['fd-incall-group-typeval']['style'] = 'display:block';
xivo_fm_type['group']['it-incall-group-typeval']['property'] = 'disabled|false:boolean';

xivo_attrib_register('fm_type-group',xivo_fm_type['group']);

xivo_fm_type['queue'] = xivo_clone(xivo_elt_type);
xivo_fm_type['queue']['fd-incall-queue-typeval']['style'] = 'display:block';
xivo_fm_type['queue']['it-incall-queue-typeval']['property'] = 'disabled|false:boolean';

xivo_attrib_register('fm_type-queue',xivo_fm_type['queue']);

xivo_fm_type['meetme'] = xivo_clone(xivo_elt_type);
xivo_fm_type['meetme']['fd-incall-meetme-typeval']['style'] = 'display:block';
xivo_fm_type['meetme']['it-incall-meetme-typeval']['property'] = 'disabled|false:boolean';

xivo_attrib_register('fm_type-meetme',xivo_fm_type['meetme']);

xivo_fm_type['schedule'] = xivo_clone(xivo_elt_type);
xivo_fm_type['schedule']['fd-incall-schedule-typeval']['style'] = 'display:block';
xivo_fm_type['schedule']['it-incall-schedule-typeval']['property'] = 'disabled|false:boolean';

xivo_attrib_register('fm_type-schedule',xivo_fm_type['schedule']);

xivo_fm_type['application'] = xivo_clone(xivo_elt_type);
xivo_fm_type['application']['fd-incall-application-typeval']['style'] = 'display:block';
xivo_fm_type['application']['it-incall-application-typeval']['property'] = 'disabled|false:boolean';

xivo_attrib_register('fm_type-application',xivo_fm_type['application']);

xivo_fm_type['sound'] = xivo_clone(xivo_elt_type);
xivo_fm_type['sound']['fd-incall-sound-typeval']['style'] = 'display:block';
xivo_fm_type['sound']['it-incall-sound-typeval']['property'] = 'disabled|false:boolean';

xivo_attrib_register('fm_type-sound',xivo_fm_type['sound']);

xivo_fm_type['custom'] = xivo_clone(xivo_elt_type);
xivo_fm_type['custom']['fd-incall-custom-typeval']['style'] = 'display:block';
xivo_fm_type['custom']['it-incall-custom-typeval']['property'] = 'disabled|false:boolean';

xivo_attrib_register('fm_type-custom',xivo_fm_type['custom']);

function xivo_chgtype(type)
{
	if(xivo_is_undef(xivo_fm_type[type.value]) == true)
		return(false);

	xivo_chg_attrib('fm_type-'+type.value,'links',0,1);
}

function xivo_incall_onload()
{
	if(xivo_eid('it-incall-type') != false)
		xivo_chgtype(xivo_eid('it-incall-type'));

	if(xivo_eid('it-incall-typefalse') != false)
		xivo_chgtypefalse(xivo_eid('it-incall-typefalse'));
}

xivo_winload.push('xivo_incall_onload();');
