function xivo_ast_schedule_onload()
{
	xivo_ast_build_dialaction_array('inschedule');
	xivo_ast_build_dialaction_array('outschedule');
	xivo_ast_dialaction_onload();
}

xivo_winload.push('xivo_ast_schedule_onload();');
