
UPDATE `cticontexts` SET `directories` = 'xivodir' WHERE
	`id`          = 3 AND
	`directories` = 'xivodir,internal';

UPDATE `ctiprofiles` SET `maxgui` = 5 WHERE id = 15;

UPDATE `ctireversedirectories` SET `directories` = '["xivodir"]' WHERE
	`id`          = 1 AND
	`directories` = '["xivodir","internal"]';

ALTER TABLE `ctisheetactions`       ADD `extrarequests` varchar(50) DEFAULT '' AFTER `whom`;
