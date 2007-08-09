xivo_fm_format = new Array();
xivo_fm_format['it-voicemail-attachformat'] = new Array();
xivo_fm_format['it-voicemail-attachformat']['property'] = new Array('disabled|false:boolean;className|it-enabled','disabled|true:boolean;className|it-disabled');

xivo_attrib_register('fm_format',xivo_fm_format);

function xivo_informat()
{
	xivo_fm_move_selected('it-voicemail-formatlist','it-voicemail-format');
	xivo_fm_copy_select('it-voicemail-format','it-voicemail-attachformat');

	if(xivo_is_undef('it-voicemail-attachformat') == true)
		return(false);

	if(xivo_eid('it-voicemail-attachformat').length == 0)
		xivo_chg_attrib('fm_format','it-voicemail-attachformat',1);
	else
		xivo_chg_attrib('fm_format','it-voicemail-attachformat',0);

	return(true);
}

function xivo_outformat()
{
	xivo_fm_move_selected('it-voicemail-format','it-voicemail-formatlist');
	xivo_fm_copy_select('it-voicemail-format','it-voicemail-attachformat');

	if(xivo_is_undef('it-voicemail-attachformat') == true)
		return(false);
	
	if(xivo_eid('it-voicemail-attachformat').length == 0)
		xivo_chg_attrib('fm_format','it-voicemail-attachformat',1);
	else
		xivo_chg_attrib('fm_format','it-voicemail-attachformat',0);

	return(true);
}

function xivo_extenmode(obj)
{
	if(xivo_is_undef(obj) == true
	|| xivo_is_object(obj) == false
	|| xivo_is_object(obj.parentNode) == false
	|| xivo_is_object(obj.parentNode.parentNode) == false)
		return(false);

	if(obj.value == 'extension')
	{
		extension_style = 'block';
		range_style = 'none';
	}
	else
	{
		extension_style = 'none';
		range_style = 'block';
	}

	xivo_etag('div',obj.parentNode.parentNode,0).style.display = extension_style;
	xivo_etag('div',obj.parentNode.parentNode,1).style.display = range_style;

	return(true);
}
