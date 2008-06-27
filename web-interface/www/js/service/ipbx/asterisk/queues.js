function xivo_ast_queue_onload()
{
	xivo_ast_build_dialaction_array('noanswer');
	xivo_ast_build_dialaction_array('busy');
	xivo_ast_build_dialaction_array('congestion');
	xivo_ast_build_dialaction_array('chanunavail');
	xivo_ast_dialaction_onload();
}

xivo_winload.push('xivo_ast_queue_onload();');
