var xivo_fm_musiconhold = new Array();

xivo_fm_musiconhold['fd-application'] = new Array();
xivo_fm_musiconhold['fd-application']['style'] = new Array('','display:none','display:block');
xivo_fm_musiconhold['fd-application']['link'] = 'it-application';

xivo_fm_musiconhold['it-application'] = new Array();
xivo_fm_musiconhold['it-application']['property'] = new Array('','disabled|true:boolean','disabled|false:boolean');

xivo_attrib_register('fm_musiconhold',xivo_fm_musiconhold);

window.onload = function()
{
	if(xivo_is_undef('it-application') == false)
		xivo_chg_attrib('fm_musiconhold','fd-application',(xivo_eid('it-mode').value != 'custom' ? 1 : 2));
}
