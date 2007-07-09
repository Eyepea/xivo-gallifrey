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
