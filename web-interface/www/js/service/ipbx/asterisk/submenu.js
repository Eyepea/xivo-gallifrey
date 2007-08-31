function xivo_submenu_onload()
{
	if(xivo_eid(xivo_smenu['tab']) == false)
		return(false);

	xivo_smenu['bak'][xivo_smenu['tab']] = xivo_eid(xivo_smenu['tab']).className;
	xivo_smenu_click(xivo_eid(xivo_smenu['tab']),xivo_smenu['class'],xivo_smenu['part'],xivo_smenu['last']);
}

xivo_winload.push('xivo_submenu_onload();');
