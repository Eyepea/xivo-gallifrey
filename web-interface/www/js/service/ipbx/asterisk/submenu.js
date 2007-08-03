xivo_winload += 'if(xivo_eid(xivo_smenu[\'tab\']) != false)\n{\n' +
		'xivo_smenu[\'bak\'][xivo_smenu[\'tab\']] = xivo_eid(xivo_smenu[\'tab\']).className;\n' +
		'xivo_smenu_click(xivo_eid(xivo_smenu[\'tab\']),xivo_smenu[\'class\'],xivo_smenu[\'part\'],xivo_smenu[\'last\']);\n}\n';
