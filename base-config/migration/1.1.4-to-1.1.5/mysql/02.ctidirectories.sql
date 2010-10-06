
CREATE TABLE `ctidirectoryfields` (
	`dir_id` int(10) unsigned,
	`fieldname` varchar(255),
	`value` varchar(255),
	PRIMARY KEY(`dir_id`, `fieldname`)
) ENGINE MyISAM DEFAULT CHARSET=utf8;


INSERT INTO `ctidirectoryfields` SELECT id, 'phone', substr(`field_phone`, 3, length(`field_phone`)-4) FROM `ctidirectories`
	WHERE length(`field_phone`) > 0;
INSERT INTO `ctidirectoryfields` SELECT id, 'fullname', substr(`field_fullname`, 3, length(`field_fullname`)-4) FROM `ctidirectories`
	WHERE length(`field_fullname`) > 0;
INSERT INTO `ctidirectoryfields` SELECT id, 'company', substr(`field_company`, 3, length(`field_company`)-4) FROM `ctidirectories`
	WHERE length(`field_company`) > 0;
INSERT INTO `ctidirectoryfields` SELECT id, 'mail', substr(`field_mail`, 3, length(`field_mail`)-4) FROM `ctidirectories`
	WHERE length(`field_mail`) > 0;
INSERT INTO `ctidirectoryfields` SELECT id, 'firstname', substr(`field_firstname`, 3, length(`field_firstname`)-4) FROM `ctidirectories`
	WHERE length(`field_firstname`) > 0;
INSERT INTO `ctidirectoryfields` SELECT id, 'lastname', substr(`field_lastname`, 3, length(`field_lastname`)-4) FROM `ctidirectories`
	WHERE length(`field_lastname`) > 0;

ALTER TABLE `ctidirectories`
	DROP `field_phone`,
	DROP `field_fullname`,
	DROP `field_company`,
	DROP `field_mail`,
	DROP `field_firstname`,
	DROP `field_lastname`;
