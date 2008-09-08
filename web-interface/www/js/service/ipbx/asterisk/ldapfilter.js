function xivo_fm_select_add_attrldap(id,value)
{
	if(xivo_chk_attrldap(value) === false)
		return(false);

	return(xivo_fm_select_add_entry(id,value,value));
}

function xivo_chk_attrldap(value)
{
	if(xivo_is_string(value) === false
	|| value.match(/^(?:[a-zA-Z0-9-]+|[0-9]+(?:\.[0-9]+)*)$/) === null)
		return(false);

	return(value);
}

function xivo_chg_additionaltype(type)
{
	var display = 'none';
	var disabled = true;

	if(type == 'custom')
	{
		display = 'block';
		disabled = false;
	}

	xivo_eid('fd-ldapfilter-additionaltext').style.display = display;
	xivo_eid('it-ldapfilter-additionaltext').disabled = disabled;
}

function xivo_ldapfilter_onload()
{
	if(xivo_eid('it-ldapfilter-additionaltype') != false)
		xivo_chg_additionaltype(xivo_eid('it-ldapfilter-additionaltype').value);
}

xivo_winload.push('xivo_ldapfilter_onload();');
