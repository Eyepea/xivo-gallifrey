var xivo_fm_trunk = new Array();

xivo_fm_trunk['fd-trunk-host-static'] = new Array();
xivo_fm_trunk['fd-trunk-host-static']['style'] = new Array('','display:none','display:block');
xivo_fm_trunk['fd-trunk-host-static']['link'] = 'it-trunk-host-static';

xivo_fm_trunk['it-trunk-host-static'] = new Array();
xivo_fm_trunk['it-trunk-host-static']['property'] = new Array('','disabled:true:boolean','disabled:false:boolean');

xivo_attrib_constructor('fm_trunk',xivo_fm_trunk);

window.onload = function()
{
	if(xivo_eid('it-trunk-host-dynamic') != false)
		xivo_chg_attrib('fm_trunk','fd-trunk-host-static',(xivo_eid('it-trunk-host-dynamic').value == 'dynamic' ? 1 : 2));
}
