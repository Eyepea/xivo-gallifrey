window.onload = function()
{
	if(xivo_eid('smenu-tab-1') != false)
	{
		xivo_smenu['bak']['smenu-tab-1'] = xivo_eid('smenu-tab-1').className;
		xivo_smenu_click(xivo_eid('smenu-tab-1'),'moc','sb-part-general');
	}
}
