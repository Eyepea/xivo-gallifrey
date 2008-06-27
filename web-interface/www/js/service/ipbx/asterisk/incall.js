function xivo_ast_incall_onload()
{
	xivo_ast_build_dialaction_array('answer');
	xivo_ast_dialaction_onload();
}

xivo_winload.push('xivo_ast_incall_onload();');
