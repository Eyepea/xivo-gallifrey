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

var xivo_nb_timezone = 0;
var xivo_node_timezone = '';

function xivo_timezone(obj,del)
{
	del = xivo_is_undef(del) == true || del == 0 ? 0 : 1;

	if(xivo_node_timezone === false)
		return(false);

	if(xivo_node_timezone == ''
	&& (xivo_node_timezone = xivo_etag('tr',xivo_eid('ex-timezone'),0)) == false)
		return(false);

	if(del == 1)
	{
		if(xivo_is_object(obj) == false
		|| xivo_is_object(obj.parentNode) == false
		|| xivo_is_object(obj.parentNode.parentNode) == false)
			return(false);

		node = obj.parentNode.parentNode;
		node.innerHTML = '';

		xivo_nb_timezone--;

		if(xivo_nb_timezone == 0)
			xivo_eid('no-timezone').style.display = 'table-row';
	}
	else
	{
		if(xivo_eid('ex-timezone') == false)
			return(false);

		xivo_nb_timezone++;

		if(xivo_nb_timezone > 0)
			xivo_eid('no-timezone').style.display = 'none';

		var xivo_node_tzclone = xivo_node_timezone.cloneNode(true);

		xivo_etag('input',xivo_node_tzclone,0).disabled = false;
		xivo_etag('input',xivo_node_tzclone,1).disabled = false;
		xivo_etag('select',xivo_node_tzclone,0).disabled = false;

		xivo_eid('timezones').appendChild(xivo_node_tzclone);
	}

	return(true);
}
