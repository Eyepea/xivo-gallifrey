xivo_menu_active();

window.onload = function()
{
	if(xivo_winload != '')
		eval(xivo_winload);
	xivo_fm_onfocus_onblur();
}
