function xivo_ast_inqueue()
{
	xivo_fm_move_selected('it-queuelist','it-queue');

	if((queuelist = xivo_eid('it-queue')) === false
	|| (len = queuelist.length) < 1)
		return(false);

	for(i = 0;i < len;i++)
	{
		if((queue = xivo_eid('queue-'+queuelist[i].value)) !== false)
			queue.style.display = 'table-row';
	}

	if(xivo_eid('it-queue').length > 0)
		xivo_eid('no-queue').style.display = 'none';

	return(true);
}

function xivo_ast_outqueue()
{
	xivo_fm_move_selected('it-queue','it-queuelist');

	if((queuelist = xivo_eid('it-queuelist')) === false
	|| (len = queuelist.length) < 1)
		return(false);

	for(i = 0;i < len;i++)
	{
		if((queue = xivo_eid('queue-'+queuelist[i].value)) !== false)
			queue.style.display = 'none';
	}

	if(xivo_eid('it-queue').length === 0)
		xivo_eid('no-queue').style.display = 'table-row';

	return(true);
}

function xivo_exten_pattern(id,option)
{
	if((id = xivo_eid(id)) === false || xivo_is_undef(id.value) === true)
		return(false);

	var value = id.value;

	if(value.charAt(0) === '_')
		value = xivo_substr(value,1);

	value = value.replace(/[X\.]/gi,'');

	if(option === '*')
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

function xivo_get_exten_buffer(letter,value)
{
	if(xivo_substr(value,-1) === '.')
		return('*');

	var chr = '';

	if(letter.indexOf('N') > -1)
		chr += 'N';

	if(letter.indexOf('X') > -1)
		chr += 'X';

	if(letter.indexOf('Z') > -1)
		chr += 'Z';

	if(chr.length === 0)
		return(false);

	var regstr = new RegExp('['+chr+']*$','i');

	if((buffer = value.match(regstr)) === null)
		return(false);

	return(buffer[0].length);
}

function xivo_chk_exten_pattern(value)
{
	if(xivo_is_undef(value) === true || xivo_is_string(value) === false)
		return(false);

	var len = value.length;

	if(len === 0 || len > 40)
		return(false);

	if(value.charAt(0) === '_')
		value = xivo_substr(value,1);
	
	if(value.match(/^[0-9NXZ\*#\-\[\]]+[\.\!]?$/) === null)
		return(false);

	return(value);
}

function xivo_fm_select_add_exten(id,value)
{
	if((pattern = xivo_chk_exten_pattern(value)) === false)
		return(false);

	return(xivo_fm_select_add_entry(id,pattern,pattern));
}
