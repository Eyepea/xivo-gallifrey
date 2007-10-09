var xivo_elt_protocol = new Array();

function xivo_inqueue()
{
	xivo_fm_move_selected('it-queuelist','it-queue');

	var len = 0;

	if(xivo_eid('it-queue') == false || (len = xivo_eid('it-queue').length) < 1)
		return(false);

	var queue = xivo_eid('it-queue');

	for(i = 0;i < len;i++)
	{
		if(xivo_eid('queue-'+queue[i].value) == false)
			continue;

		xivo_eid('queue-'+queue[i].value).style.display = 'table-row';
	}

	if(xivo_eid('it-queue').length > 0)
		xivo_eid('no-queue').style.display = 'none';
}

function xivo_outqueue()
{
	xivo_fm_move_selected('it-queue','it-queuelist');

	var len = 0;

	if(xivo_eid('it-queuelist') == false || (len = xivo_eid('it-queuelist').length) < 1)
		return(false);

	var queue = xivo_eid('it-queuelist');

	for(i = 0;i < len;i++)
	{
		if(xivo_eid('queue-'+queue[i].value) == false)
			continue;

		xivo_eid('queue-'+queue[i].value).style.display = 'none';
	}

	if(xivo_eid('it-queue').length == 0)
		xivo_eid('no-queue').style.display = 'table-row';
}

function xivo_exten_pattern(id,option)
{
	if((id = xivo_eid(id)) == false || xivo_is_undef(id.value) == true)
		return(false);

	var value = id.value;

	if(value.charAt(0) == '_')
		value = xivo_substr(value,1);

	value = value.replace(/[X\.]/gi,'');

	if(option == '*')
	{
		id.value = value + '.';
		return(true);
	}

	option = Number(option);

	if(option > 0 && option < 40)
	{
		id.value = value + xivo_str_repeat('X',option);
		return(true);
	}

	return(false);
}

function xivo_chk_exten_pattern(id,value)
{
	if(xivo_is_undef(value) == true || xivo_is_string(value) == false)
		return(false);

	var len = value.length;

	if(len == 0 || len > 40)
		return(false);

	if(value.charAt(0) == '_')
		value = xivo_substr(value,1);
	
	if(value.match(/^[0-9NXZ\*#\-\[\]]+[\.\!]?$/) == null)
		return(false);

	return(value);
}

function xivo_fm_select_add_exten(id,value)
{
	if((pattern = xivo_chk_exten_pattern(id,value)) == false)
		return(false);

	return(xivo_fm_select_add_entry(id,pattern,pattern));
}
