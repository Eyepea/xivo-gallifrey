function xivo_fm_select_add_attrldap(id,value)
{
	if(xivo_chk_attrldap(value) == false)
		return(false);

	return(xivo_fm_select_add_entry(id,value,value));
}

function xivo_chk_attrldap(value)
{
	if(xivo_is_undef(value) == true
	|| xivo_is_string(value) == false)
		return(false);

	var regstr = new RegExp('^(?:[a-zA-Z0-9-]+|[0-9]+(?:\\.[0-9]+)*)$');

	if(value.match(regstr) == null)
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

	xivo_eid('fd-ldapserver-additionaltext').style.display = display;
	xivo_eid('it-ldapserver-additionaltext').disabled = disabled;
}

function xivo_ldapserver_onload()
{
	if(xivo_eid('it-ldapserver-additionaltype') != false)
		xivo_chg_additionaltype(xivo_eid('it-ldapserver-additionaltype').value);
}

xivo_winload.push('xivo_ldapserver_onload();');
