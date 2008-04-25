var xivo_fm_dcontext = new Array();

xivo_fm_dcontext['fd-dcontext-custom'] = new Array();
xivo_fm_dcontext['fd-dcontext-custom']['style'] = new Array('display:none','display:block');
xivo_fm_dcontext['fd-dcontext-custom']['link'] = 'it-dcontext-custom';

xivo_fm_dcontext['it-dcontext-custom'] = new Array();
xivo_fm_dcontext['it-dcontext-custom']['property'] = new Array('disabled|true:boolean','disabled|false:boolean');

xivo_attrib_register('fm_dcontext',xivo_fm_dcontext);

function xivo_cdr_onload()
{
	if((dcontext = xivo_eid('it-dcontext')) != false)
		xivo_chg_attrib('fm_dcontext','fd-dcontext-custom',(dcontext.value != 'custom' ? 0 : 1));
}

xivo_winload.push('xivo_cdr_onload();');
