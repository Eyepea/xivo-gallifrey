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

	if(option > 2 && option < 12)
	{
		id.value = value + xivo_str_repeat('X',option);
		return(true);
	}

	return(false);
}
