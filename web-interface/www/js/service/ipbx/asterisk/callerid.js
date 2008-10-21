var xivo_ast_fm_callerid = {
	'fd-callerid-callerdisplay':
		{style: [{display: 'block'},{display: 'none'}],
		 link: 'it-callerid-callerdisplay'},
	'it-callerid-callerdisplay':
		{property: [{readOnly: false, className: 'it-enabled'},
			    {readOnly: true, className: 'it-readonly'}]}};

xivo_attrib_register('ast_fm_callerid',xivo_ast_fm_callerid);

function xivo_ast_chg_callerid_mode(obj)
{
	if(xivo_type_object(obj) === false
	|| xivo_is_string(obj.value) === false)
		return(false);

	xivo_chg_attrib('ast_fm_callerid',
			'fd-callerid-callerdisplay',
			Number(obj.value.length < 1));

	return(true);
}

function xivo_ast_callerid_onload()
{
	return(xivo_ast_chg_callerid_mode(xivo_eid('it-callerid-mode')));
}

xivo_winload.push('xivo_ast_callerid_onload();');
