
ALTER TABLE `ctisheetactions`
	CHANGE `sheet_info`   `sheet_info` TEXT,
	CHANGE `systray_info` `systray_info` TEXT,
	CHANGE `sheet_qtui`   `sheet_qtui` TEXT,
	CHANGE `action_info`  `action_info` TEXT;

INSERT INTO `ctisheetactions` VALUES(6,'dial','sheet_action_dial','["default"]','dest','["agentsup","agent","client"]','{"10": [ "","text","Inconnu","Appel {xivo-direction} de {xivo-calleridnum}" ],"20": [ "Numéro entrant","phone","Inconnu","{xivo-calleridnum}" ],"30": [ "Nom","text","Inconnu","{db-fullname}" ],"40": [ "Numéro appelé","phone","Inconnu","{xivo-calledidnum}" ]}','{"10": [ "","title","","Appel {xivo-direction}" ],"20": [ "","body","Inconnu","appel de {xivo-calleridnum} pour {xivo-calledidnum}" ],"30": [ "","body","Inconnu","{db-fullname} (selon {xivo-directory})" ],"40": [ "","body","","le {xivo-date}, il est {xivo-time}" ]}','','{"10": [ "","urlauto","","http://www.google.fr/search?q={xivo-calleridnum}" ]}','',1)
	ON DUPLICATE KEY UPDATE
		`sheet_info`   = '{"10": [ "","text","Inconnu","Appel {xivo-direction} de {xivo-calleridnum}" ],"20": [ "Numéro entrant","phone","Inconnu","{xivo-calleridnum}" ],"30": [ "Nom","text","Inconnu","{db-fullname}" ],"40": [ "Numéro appelé","phone","Inconnu","{xivo-calledidnum}" ]}',
		`systray_info` = '{"10": [ "" t","title","","Appel {xivo-direction}" ],"20": [ "","body","Inconnu","appel de {xivo-calleridnum} pour {xivo-calledidnum}" ],"30": [ "","body","Inconnu","{db-fullname} (selon {xivo-directory})" ],"40": [ "","body","","le {xivo-date}, il est {xivo-time}" ]}',
		`sheet_qtui`   = '',
		`action_info`  = '{"10": [ "","urlauto","","http://www.google.fr/search?q={xivo-calleridnum}" ]}';

INSERT INTO `ctisheetactions` VALUES(7,'queue','sheet_action_queue','["default"]','dest','["agentsup","agent","client"]','{"10": [ "","text","Inconnu","Appel {xivo-direction} de la File {xivo-queuename}" ],"20": [ "Numéro entrant","phone","Inconnu","{xivo-calleridnum}" ],"30": [ "Nom","text","Inconnu","{db-fullname}" ]}','{"10": [ "","title","","Appel {xivo-direction} de la File {xivo-queuename}" ],"20": [ "","body","Inconnu","appel de {xivo-calleridnum} pour {xivo-calledidnum}" ],"30": [ "","body","Inconnu","{db-fullname} (selon {xivo-directory})" ],"40": [ "","body","","le {xivo-date}, il est {xivo-time}" ]}','file:///etc/pf-xivo/ctiservers/form.ui','{}','',1)
	ON DUPLICATE KEY UPDATE
		`sheet_info`   = '{"10": [ "","text","Inconnu","Appel {xivo-direction} de la File {xivo-queuename}" ],"20": [ "Numéro entrant","phone","Inconnu","{xivo-calleridnum}" ],"30": [ "Nom","text","Inconnu","{db-fullname}" ]}',
		`systray_info` = '{"10": [ "","title","","Appel {xivo-direction} de la File {xivo-queuename}" ],""
20": [ "","body","Inconnu","appel de {xivo-calleridnum} pour {xivo-calledidnum}" ],"30": [ "","body","Inconnu","{db-fullname} (selon {xivo-directory})" ],"40": [ "","body","","le {xivo-date}, il est {xivo-time}" ]}',
		`sheet_qtui`   = 'file:///etc/pf-xivo/ctiservers/form.ui',
		`action_info`  = '{}';
INSERT INTO `ctisheetactions` VALUES(8,'custom1','sheet_action_custom1','["default"]','all','["agentsup","agent","client"]','{"10": [ "","text","Inconnu","Appel {xivo-direction} (Custom)" ],"20": [ "Numéro entrant","phone","Inconnu","{xivo-calleridnum}" ],"30": [ "Nom","text","Inconnu","{db-fullname}" ]}','{"10": [ "","title","","Appel {xivo-direction} (Custom)" ],"20": [ "","body","Inconnu","appel de {xivo-calleridnum} pour {xivo-calledidnum}" ],"30": [ "","body","Inconnu","{db-fullname} (selon {xivo-directory})" ],"40": [ "","body","","le {xivo-date}, il est {xivo-time}" ]}','','{}','',1)
	ON DUPLICATE KEY UPDATE
		`sheet_info`   = '{"10": [ "","text","Inconnu","Appel {xivo-direction} (Custom)" ],"20": [ "Nummro entrant","phone","Inconnu","{xivo-calleridnum}" ],"30": [ "Nom","text","Inconnu","{db-fullname}" ]}',
		`systray_info` = '{"10": [ "","title","","Appel {xivo-direction} (Custom)" ],"20": [ "","body","Inconnu","appel de {xivo-calleridnum} pour {xivo-calledidnum}" ],"30": [ "","body","Inconnu","{db-fullname} (selon {xivo-directory})" ],"40": [ "","body","","le {xivo-date}, il est {xivo-time}" ]}',
		`sheet_qtui`   = '',
		`action_info`  = '{}';
