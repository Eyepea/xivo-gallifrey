function xivo_context_entity_enable_add(type,table)
{
	if(xivo_is_string(type) == false
	|| xivo_is_object(table) == false
	|| (entity = xivo_eid('it-context-entity')) == false
	|| entity.value === '')
		return(false);

	return(xivo_table_list('contextnumbers-'+type,table));
}

function xivo_context_entity_status(form,disable) 
{
	var arr = new Array();
	arr['user'] = new Array('numberbeg','numberend');
	arr['group'] = new Array('numberbeg','numberend');
	arr['queue'] = new Array('numberbeg','numberend');
	arr['meetme'] = new Array('numberbeg','numberend');
	arr['incall'] = new Array('numberbeg','numberend','didlength');

	for(var key in arr)
	{
		ref = arr[key];
		nb = ref.length;

		for(i = 0;i < nb;i++)
		{
			xivo_fm_enable_disable_field(form,
						     'contextnumbers['+key+']['+ref[i]+'][]',
						     disable,
						     'ex-contextnumbers-'+key,
						     'tbody');
		}
	}
}
